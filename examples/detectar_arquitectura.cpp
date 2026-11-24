
#include <string>

/**
 * Detecta la arquitectura para la cual fue compilado
 * el programa mediante macros del preprocesador.
 *
 * @return Descripcion de la arquitectura detectada.
 */
std::string obtener_arquitectura() {
#if defined(__x86_64__) || defined(_M_X64)
    return "x86 de 64 bits";

#elif defined(__i386__) || defined(_M_IX86)
    return "x86 de 32 bits";

#elif defined(__aarch64__) || defined(_M_ARM64)
    return "ARM de 64 bits";

#elif defined(__arm__) || defined(_M_ARM)
    return "ARM";

#elif defined(__riscv) && (__riscv_xlen == 64)
    return "RISC-V de 64 bits";

#elif defined(__riscv) && (__riscv_xlen == 32)
    return "RISC-V de 32 bits";

#else
    return "Desconocida";
#endif
}
