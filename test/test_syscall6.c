#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

// Define el número de syscall que asignaste en el kernel
#define SYSCALL_NUMBER 552

// Estructura total_memory_stats (debe coincidir con la definición del kernel)
struct total_memory_stats {
    unsigned long total_reserved_mb; // Memoria total reservada en MB
    unsigned long total_used_mb;     // Memoria total utilizada en MB (excluyendo buffers y cachés)
    unsigned long total_swap_mb;     // Memoria swap total en MB
    unsigned long used_swap_mb;      // Memoria swap utilizada en MB
};

int main() {
    struct total_memory_stats stats;

    // Llamar a la syscall
    long result = syscall(SYSCALL_NUMBER, &stats);

    if (result == 0) {
        printf("Uso de memoria general:\n");
        printf("  - Memoria total reservada: %lu MB\n", stats.total_reserved_mb);
        printf("  - Memoria total utilizada: %lu MB\n", stats.total_used_mb);
        printf("  - Memoria swap total: %lu MB\n", stats.total_swap_mb);
        printf("  - Memoria swap utilizada: %lu MB\n", stats.used_swap_mb);
    } else {
        perror("Error al ejecutar la syscall");
        printf("Código de error: %ld (errno: %d)\n", result, errno);
    }

    return 0;
}
