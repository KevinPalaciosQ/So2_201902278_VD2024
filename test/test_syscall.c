#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <errno.h>

// Estructura para almacenar las estadísticas de I/O
struct io_stats {
    unsigned long read_bytes;
    unsigned long write_bytes;
    unsigned long read_operations;
    unsigned long write_operations;
    unsigned long io_wait_time;
};

int main() {
    pid_t pid;
    struct io_stats stats;
    long ret;

    // Solicitar el PID al usuario
    printf("Ingrese el PID del proceso: ");
    if (scanf("%d", &pid) != 1) {
        fprintf(stderr, "Error: no se pudo leer el PID.\n");
        return 1;
    }

    // Invocar la syscall
    ret = syscall(549, pid, &stats);

    if (ret < 0) {
        perror("Error al obtener las estadísticas de I/O");
        return errno;
    }

    // Mostrar los resultados
    printf("Estadísticas de I/O para el PID %d:\n", pid);
    printf("Bytes leídos: %lu\n", stats.read_bytes);
    printf("Bytes escritos: %lu\n", stats.write_bytes);
    printf("Tiempo en espera de I/O: %lu ns\n", stats.io_wait_time);

    return 0;
}
