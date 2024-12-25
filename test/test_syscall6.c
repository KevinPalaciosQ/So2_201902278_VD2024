#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

// Definir la estructura en espacio de usuario (debe coincidir con la del kernel)
struct memstats {
    unsigned long reserved_memory;  // Memoria reservada (en KB)
    unsigned long used_memory;      // Memoria utilizada (en KB)
};

// Códigos ANSI para colores (opcional)
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

int main() {
    struct memstats stats; // Variable para almacenar las estadísticas

    // Número de syscall (ajusta este número según el registrado en syscall_64.tbl)
    long syscall_number = 553;

    // Invocar la syscall
    long result = syscall(syscall_number, &stats);

    // Validar el resultado
    if (result == 0) {
        // Convertir KB a MB
        unsigned long reserved_mb = stats.reserved_memory / 1024;
        unsigned long used_mb = stats.used_memory / 1024;

        // Mostrar la salida en formato de tabla
        printf("\n+---------------------------+---------------------------+\n");
        printf("| " BLUE "Memoria Reservada (MB)" RESET "    | " GREEN "Memoria Utilizada (MB)" RESET "    |\n");
        printf("+---------------------------+---------------------------+\n");
        printf("| %25lu | %25lu |\n", reserved_mb, used_mb);
        printf("+---------------------------+---------------------------+\n\n");
    } else {
        // Manejar errores
        perror("Error al ejecutar la syscall");
        printf("Código de error: %ld (errno: %d)\n", result, errno);
    }

    return 0;
}
