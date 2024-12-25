#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_SYSCALLS 1024
#define SYSCALL_TRACK_USAGE 550

struct syscall_usage {
    unsigned long count;
    struct timespec last_called;
};

int main() {
    struct syscall_usage stats[MAX_SYSCALLS];
    int result;

    memset(stats, 0, sizeof(stats));

    // Abrir archivos
    int fd = open("/home/kevin/Desktop/pruebaSopes2", O_RDONLY);
    if (fd == -1) {
        perror("Error abriendo archivo");
        return 1;
    }
    close(fd);

    // Leer
    read(0, NULL, 0);

    //Escribir
    write(1, NULL, 0);

    // syscall
    result = syscall(SYSCALL_TRACK_USAGE, stats);
    if (result < 0) {
        perror("Error llamando syscall_usage");
        return 1;
    }

    //Prueba de fork
    pid_t pid = fork();
    if (pid == 0) {
        printf("=================================\n");
        printf("Procesos hijos creados.\n");
        exit(0);
    } else {
        wait(NULL);
    }
    printf("=================================\n");
    printf("> Estadisticas de Syscall Usage <\n");
    printf("=================================\n");
    for (int i = 0; i < MAX_SYSCALLS; i++) {
        if (stats[i].count > 0) {
            // converción del tiempo.
            char time_str[100];
            struct tm *tm_info;
            // Convertir segundos
            tm_info = localtime(&stats[i].last_called.tv_sec);
            // Formatear la fecha y hora
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
            // Imprimir la información
            printf("Syscall ID %d: Count = %lu, Last Called = %s.%09ld\n",
                   i, stats[i].count,
                   time_str, stats[i].last_called.tv_nsec);
        }
    }

    return 0;
}