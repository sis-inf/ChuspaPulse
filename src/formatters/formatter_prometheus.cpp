#include "formatter_prometheus.hpp"

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

/// Separa "cpu.usage{core=\"0\"}" en nombre base y etiquetas.
std::pair<std::string, std::string> separarEtiquetas(const std::string& nombre) {
    const std::size_t inicio = nombre.find('{');
    if (inicio == std::string::npos) {
        return {nombre, ""};
    }
    return {nombre.substr(0, inicio), nombre.substr(inicio)};
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

    if (snapshot.metricas.empty()) {
        return "";
    }

    // Detectar colisiones: nombres internos distintos que normalizan igual.
    std::unordered_map<std::string, std::set<std::string>> originalesPorNombre;
    for (const auto& metrica : snapshot.metricas) {
        const std::string base = separarEtiquetas(metrica.nombre).first;
        originalesPorNombre[normalizarNombre(base)].insert(base);
    }

    // Agrupar las muestras por métrica para declarar HELP y TYPE una sola vez.
    std::vector<FamiliaMetrica> familias;
    std::unordered_map<std::string, std::size_t> indices;

    for (const auto& metrica : snapshot.metricas) {
        const auto [base, etiquetas] = separarEtiquetas(metrica.nombre);

        std::string nombre = normalizarNombre(base);
        const bool colision = originalesPorNombre[nombre].size() > 1;
        if (colision) {
            // Sufijo derivado del nombre original: no depende del orden.
            nombre += "_" + hashCorto(base);
        }

        std::ostringstream muestra;
        muestra << std::setprecision(15)
                << nombre << etiquetas << " " << metrica.valor;

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

std::string FormatterPrometheus::formatearHistorial(
    const std::vector<chuspapulse::core::Snapshot>& snapshots) const {

    if (snapshots.empty()) {
        return "";
    }

    return formatear(snapshots.back());
}

} // namespace chuspapulse::formatters
