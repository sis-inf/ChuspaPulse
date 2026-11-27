#include "formatter_prometheus.hpp"

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <set>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace chuspapulse::formatters {

namespace {

/// Muestras de una misma métrica, en el orden en que aparecen.
struct FamiliaMetrica {
    std::string nombre;
    std::string original;  ///< Nombre interno, solo si hubo colisión.
    std::vector<std::string> muestras;
};

/// Nombre base de una métrica y sus etiquetas ya escapadas.
struct NombreSeparado {
    std::string base;
    std::string etiquetas;  ///< Vacío o con la forma {nombre="valor",...}.
};

/// Hash FNV-1a de 32 bits en hexadecimal (8 caracteres). Determinista.
std::string hashCorto(const std::string& texto) {
    std::uint32_t hash = 2166136261u;
    for (const unsigned char c : texto) {
        hash ^= c;
        hash *= 16777619u;
    }
    std::ostringstream out;
    out << std::hex << std::setw(8) << std::setfill('0') << hash;
    return out.str();
}

/// Escapa el texto de una línea HELP como pide el formato Prometheus:
/// '\' -> "\\" y salto de línea -> "\n".
std::string escaparHelp(const std::string& texto) {
    std::string resultado;
    resultado.reserve(texto.size());
    for (const char c : texto) {
        if (c == '\\') {
            resultado += "\\\\";
        } else if (c == '\n') {
            resultado += "\\n";
        } else {
            resultado.push_back(c);
        }
    }
    return resultado;
}

/// Escapa el valor de una etiqueta como pide el formato Prometheus:
/// '\' -> "\\", '"' -> "\"" y salto de línea -> "\n".
std::string escaparValorEtiqueta(const std::string& valor) {
    std::string resultado;
    resultado.reserve(valor.size());
    for (const char c : valor) {
        if (c == '\\') {
            resultado += "\\\\";
        } else if (c == '"') {
            resultado += "\\\"";
        } else if (c == '\n') {
            resultado += "\\n";
        } else {
            resultado.push_back(c);
        }
    }
    return resultado;
}

/// Indica si el texto es un nombre de etiqueta válido: [a-zA-Z_][a-zA-Z0-9_]*.
bool esNombreEtiqueta(const std::string& nombre) {
    if (nombre.empty()) {
        return false;
    }
    for (std::size_t i = 0; i < nombre.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(nombre[i]);
        const bool letra = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                           c == '_';
        const bool digito = c >= '0' && c <= '9';
        if (!letra && !(digito && i > 0)) {
            return false;
        }
    }
    return true;
}

/// Indica si la comilla en la posición pos cierra el valor de una etiqueta:
/// va seguida de la '}' final o de ',' y de otra etiqueta (nombre=").
/// Requiere pos + 1 < etiquetas.size().
bool cierraValor(const std::string& etiquetas, std::size_t pos) {
    if (pos + 1 == etiquetas.size() - 1) {
        return true;  // seguida de la '}' final
    }
    if (etiquetas[pos + 1] != ',') {
        return false;
    }
    const std::size_t igual = etiquetas.find('=', pos + 2);
    return igual != std::string::npos &&
           esNombreEtiqueta(etiquetas.substr(pos + 2, igual - (pos + 2))) &&
           igual + 1 < etiquetas.size() && etiquetas[igual + 1] == '"';
}

/// Escapa un conjunto de etiquetas con la forma {nombre="valor",...}. Los
/// valores se toman literales: se escapan '\', '"' y salto de línea.
/// @return false si el texto no tiene esa forma (salida no se modifica).
bool escaparEtiquetas(const std::string& etiquetas, std::string& salida) {
    if (etiquetas.size() < 2 || etiquetas.front() != '{' ||
        etiquetas.back() != '}') {
        return false;
    }

    const std::size_t fin = etiquetas.size() - 1;  // posición de la '}' final
    std::string resultado = "{";
    std::size_t pos = 1;

    while (pos < fin) {
        const std::size_t igual = etiquetas.find('=', pos);
        if (igual == std::string::npos || igual + 1 >= fin ||
            etiquetas[igual + 1] != '"') {
            return false;
        }

        const std::string nombre = etiquetas.substr(pos, igual - pos);
        if (!esNombreEtiqueta(nombre)) {
            return false;
        }

        std::size_t cierre = igual + 2;
        while (cierre < fin &&
               !(etiquetas[cierre] == '"' && cierraValor(etiquetas, cierre))) {
            ++cierre;
        }
        if (cierre >= fin) {
            return false;
        }

        resultado += nombre + "=\"" +
                     escaparValorEtiqueta(
                         etiquetas.substr(igual + 2, cierre - (igual + 2))) +
                     "\"";

        pos = cierre + 1;
        if (pos < fin) {
            resultado += ",";
            ++pos;  // salta la ','
        }
    }

    resultado += "}";
    salida = resultado;
    return true;
}

/// Convierte un valor al texto que exige el formato de exposición de
/// Prometheus: "NaN", "+Inf" y "-Inf" para valores no finitos (el texto que
/// produce un ostream para ellos depende de la plataforma). El resto se escribe
/// con 15 dígitos significativos.
std::string formatearValor(double valor) {
    if (std::isnan(valor)) {
        return "NaN";
    }
    if (std::isinf(valor)) {
        return valor > 0 ? "+Inf" : "-Inf";
    }
    std::ostringstream out;
    out << std::setprecision(15) << valor;
    return out.str();
}

/// Separa "cpu.usage{core=\"0\"}" en nombre base y etiquetas.
std::pair<std::string, std::string> separarEtiquetas(const std::string& nombre) {
    const std::size_t inicio = nombre.find('{');
    if (inicio == std::string::npos) {
        return {nombre, ""};
    }
    return {nombre.substr(0, inicio), nombre.substr(inicio)};
}

/// Separa el nombre interno de una métrica en nombre base y etiquetas ya
/// escapadas. Si las etiquetas no tienen la forma {nombre="valor",...}, no se
/// interpretan: todo el nombre pasa a ser el nombre de la métrica (después se
/// normaliza), para no generar una línea inválida.
NombreSeparado separarNombre(const std::string& nombre) {
    const auto [base, etiquetas] = separarEtiquetas(nombre);
    if (etiquetas.empty()) {
        return {base, ""};
    }

    std::string escapadas;
    if (escaparEtiquetas(etiquetas, escapadas)) {
        return {base, escapadas};
    }
    return {nombre, ""};
}

} // namespace

std::string FormatterPrometheus::normalizarNombre(const std::string& nombre) {
    std::string resultado = "chuspapulse_";
    resultado.reserve(resultado.size() + nombre.size());

    for (const unsigned char c : nombre) {
        const bool valido = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                            (c >= '0' && c <= '9') || c == '_';
        resultado.push_back(valido ? static_cast<char>(c) : '_');
    }

    return resultado;
}

std::string FormatterPrometheus::formato() const {
    return "prometheus";
}

std::string FormatterPrometheus::contentType() const {
    return "text/plain; version=0.0.4";
}

std::string FormatterPrometheus::formatear(
    const chuspapulse::core::Snapshot& snapshot) const {

    // Un snapshot sin métricas produce una exposición vacía, que es válida.
    if (snapshot.metricas.empty()) {
        return "";
    }

    // Detectar colisiones: nombres internos distintos que normalizan igual.
    std::unordered_map<std::string, std::set<std::string>> originalesPorNombre;
    for (const auto& metrica : snapshot.metricas) {
        const std::string base = separarNombre(metrica.nombre).base;
        originalesPorNombre[normalizarNombre(base)].insert(base);
    }

    // Agrupar las muestras por métrica para declarar HELP y TYPE una sola vez.
    std::vector<FamiliaMetrica> familias;
    std::unordered_map<std::string, std::size_t> indices;

    for (const auto& metrica : snapshot.metricas) {
        const auto [base, etiquetas] = separarNombre(metrica.nombre);

        std::string nombre = normalizarNombre(base);
        const bool colision = originalesPorNombre[nombre].size() > 1;
        if (colision) {
            // Sufijo derivado del nombre original: no depende del orden.
            nombre += "_" + hashCorto(base);
        }

        std::ostringstream muestra;
        muestra << nombre << etiquetas << " " << formatearValor(metrica.valor);

        auto [it, nueva] = indices.try_emplace(nombre, familias.size());
        if (nueva) {
            familias.push_back({nombre, colision ? base : "", {}});
        }

        familias[it->second].muestras.push_back(muestra.str());
    }

    std::ostringstream output;

    for (std::size_t i = 0; i < familias.size(); ++i) {
        if (i > 0) {
            output << "\n";
        }

        output << "# HELP " << familias[i].nombre << " Metrica de chuspapulse";
        if (!familias[i].original.empty()) {
            output << " (nombre original: " << escaparHelp(familias[i].original)
                   << ")";
        }
        output << "\n";
        output << "# TYPE " << familias[i].nombre << " gauge\n";

        for (const auto& muestra : familias[i].muestras) {
            output << muestra << "\n";
        }
    }

    return output.str();
}

// Prometheus expone solo el estado actual, por lo que se serializa el último
// snapshot del historial; un historial vacío produce una exposición vacía.
std::string FormatterPrometheus::formatearHistorial(
    const std::vector<chuspapulse::core::Snapshot>& snapshots) const {

    if (snapshots.empty()) {
        return "";
    }

    return formatear(snapshots.back());
}

} // namespace chuspapulse::formatters
