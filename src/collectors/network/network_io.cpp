#include <fstream>
#include <sstream>
#include <string>

struct NetworkIO {
    long rx;
    long tx;
};

NetworkIO getNetworkIO() {

    NetworkIO result = {-1, -1};
    std::ifstream file("/proc/net/dev");
    if (!file.is_open()) {
        return result;
    }

    long totalRx = 0;
    long totalTx = 0;
    bool hayInterfaz = false;
    std::string line;

    while (std::getline(file, line)) {
        // Las dos líneas de cabecera no tienen ':'; las de interfaz sí
        // ("  eth0: 1234 ..." o "eth0:1234 ...").
        const std::string::size_type separador = line.find(':');
        if (separador == std::string::npos) {
            continue;
        }

        // Nombre de la interfaz sin espacios alrededor.
        std::string iface = line.substr(0, separador);
        const std::string::size_type inicio = iface.find_first_not_of(" \t");
        if (inicio == std::string::npos) {
            continue;
        }
        iface = iface.substr(inicio, iface.find_last_not_of(" \t") - inicio + 1);

        // La interfaz de loopback no representa tráfico de red real.
        if (iface == "lo") {
            continue;
        }

        // Campo 1: bytes recibidos; campo 9: bytes transmitidos.
        std::istringstream ss(line.substr(separador + 1));
        long rx = 0;
        long tx = 0;
        long skip = 0;

        ss >> rx;
        for (int i = 0; i < 7; i++) ss >> skip;
        ss >> tx;

        if (ss.fail()) {
            continue;
        }

        totalRx += rx;
        totalTx += tx;
        hayInterfaz = true;
    }

    if (hayInterfaz) {
        result.rx = totalRx;
        result.tx = totalTx;
    }
    return result;
}
