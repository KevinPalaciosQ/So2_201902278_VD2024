#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <errno.h>

// Estructura que debe coincidir con la definida en el módulo del kernel
struct io_stats {
    unsigned long read_bytes;
    unsigned long write_bytes;
    unsigned long read_operations;
    unsigned long write_operations;
    unsigned long io_wait_time;
};

// Define el número de syscall asignado
#define SYS_GET_IO_THROTTLE 549 // Asegúrate de que este número coincida con el de tu syscall

int main(int argc, char *argv[]) {
    struct io_stats stats;
    pid_t pid;
    int ret;

    // Verificar que el usuario haya proporcionado un PID
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <pid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Convertir el argumento a PID
    pid = atoi(argv[1]);

    // Llamar a la syscall
    ret = syscall(SYS_GET_IO_THROTTLE, pid, &stats);
    if (ret < 0) {
        perror("Error al llamar a get_io_throttle_201902278");
        return EXIT_FAILURE;
    }

    // Mostrar estadísticas obtenidas
    printf("Estadísticas de I/O para el proceso con PID %d:\n", pid);
    printf("Bytes leídos: %lu\n", stats.read_bytes);
    printf("Bytes escritos: %lu\n", stats.write_bytes);
    printf("Operaciones de lectura: %lu\n", stats.read_operations);
    printf("Operaciones de escritura: %lu\n", stats.write_operations);
    printf("Tiempo de espera de I/O: %lu ns\n", stats.io_wait_time);

    return EXIT_SUCCESS;
}
