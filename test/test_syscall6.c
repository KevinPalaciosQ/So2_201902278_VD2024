#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

// Define el número de syscall que asignaste en el kernel
#define SYSCALL_NUMBER 552

// Estructura total_memory_stats (debe coincidir con la definición del kernel)
struct total_memory_stats {
    unsigned long total_reserved_mb; // Memoria total reservada en MB
    unsigned long total_used_mb;     // Memoria total utilizada en MB
};

int main() {
    struct total_memory_stats stats;

    if (syscall(SYSCALL_NUMBER, &stats) == 0) {
        // Imprimir tabla con colores
        printf("\033[1;34m+---------------------+----------------+\033[0m\n");
        printf("\033[1;34m| Categoría           | MB             |\033[0m\n");
        printf("\033[1;34m+---------------------+----------------+\033[0m\n");
        printf("\033[1;32m| Memoria reservada   | %14lu |\033[0m\n", stats.total_reserved_mb);
        printf("\033[1;32m| Memoria utilizada   | %14lu |\033[0m\n", stats.total_used_mb);
        printf("\033[1;34m+---------------------+----------------+\033[0m\n");
    } else {
        perror("\033[1;31mError al ejecutar la syscall\033[0m");
    }

    return 0;
}
