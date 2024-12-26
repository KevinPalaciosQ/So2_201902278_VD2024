/*
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
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

struct total_memory_stats {
    unsigned long total_reserved_kb;
    unsigned long total_commited_kb;
};

#define SYSCALL_NUMBER 552 

int main() {
    struct total_memory_stats stats;

    // Invocar la syscall
    long result = syscall(SYSCALL_NUMBER, &stats);

    if (result == 0) {
        // Mostrar resultados en una tabla con colores
        printf(CYAN "+-----------------------+------------------+\n" RESET);
        printf(CYAN "| " YELLOW "Estadística           " CYAN "| " YELLOW "Valor (KB)       " CYAN "|\n" RESET);
        printf(CYAN "+-----------------------+------------------+\n" RESET);
        printf(CYAN "| " GREEN "Total Reservado       " CYAN "| " MAGENTA "%16lu " CYAN "|\n" RESET, stats.total_reserved_kb);
        printf(CYAN "| " GREEN "Total Comprometido    " CYAN "| " MAGENTA "%16lu " CYAN "|\n" RESET, stats.total_commited_kb);
        printf(CYAN "+-----------------------+------------------+\n" RESET);
    } else {
        // Manejo de errores
        fprintf(stderr, RED "Error al ejecutar la syscall: " RESET);
        perror("");
        fprintf(stderr, RED "Código de error: %ld (errno: %d)\n" RESET, result, errno);
    }

    return 0;
}

