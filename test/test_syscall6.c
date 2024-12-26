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
        printf("Memoria total reservada: %lu MB\n", stats.total_reserved_mb);
        printf("Memoria total utilizada: %lu MB\n", stats.total_used_mb);
    } else {
        perror("Error al ejecutar la syscall");
    }

    return 0;
}
