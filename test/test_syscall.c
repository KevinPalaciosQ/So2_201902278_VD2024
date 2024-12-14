#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define __NR_get_io_throttle 549 

// Estructura para almacenar las estadísticas de I/O
struct io_stats {
    unsigned long read_bytes;
    unsigned long write_bytes;
    unsigned long read_operations;
    unsigned long write_operations;
    unsigned long io_wait_time;
};

// Función para invocar la syscall
long get_io_throttle(pid_t pid, struct io_stats *stats) {
    return syscall(__NR_get_io_throttle, pid, stats);
}

int main() {
    pid_t pid;
    struct io_stats stats;

    printf("Ingrese el PID del proceso: ");
    scanf("%d", &pid);

    // Llamada a la syscall para obtener las estadísticas de I/O
    long result = get_io_throttle(pid, &stats);

    if (result == 0) {
        // Mostrar las estadísticas
        printf("Estadísticas de I/O para el proceso %d:\n", pid);
        printf("Bytes leídos: %lu\n", stats.read_bytes);
        printf("Bytes escritos: %lu\n", stats.write_bytes);
        printf("Operaciones de lectura: %lu\n", stats.read_operations);
        printf("Operaciones de escritura: %lu\n", stats.write_operations);
        printf("Tiempo de espera por I/O (en jiffies): %lu\n", stats.io_wait_time);
    } else {
        perror("Error al obtener las estadísticas de I/O");
    }

    return 0;
}