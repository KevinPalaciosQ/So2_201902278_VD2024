#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define MAX_PROCESSES 128
#define SYSCALL_NUMBER 553

// Definición de colores ANSI
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"

struct memory_stats {
    pid_t process_id;                   // PID del proceso
    unsigned long virtual_mem_kb;       // Memoria virtual reservada en KB
    unsigned long virtual_mem_mb;       // Memoria virtual reservada en MB
    unsigned long physical_mem_kb;      // Memoria física comprometida en KB
    unsigned long physical_mem_mb;      // Memoria física comprometida en MB
    unsigned long commit_percentage;    // Porcentaje de memoria comprometida
    int oom_adjust_score;               // OOM Score
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, RED "Uso: %s <PID o 0 para todos los procesos>\n" RESET, argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    struct memory_stats stats[MAX_PROCESSES];

    // Invocar la syscall
    long result = syscall(SYSCALL_NUMBER, pid, stats, MAX_PROCESSES);

    if (result >= 0) {
        printf(GREEN "Se obtuvieron datos de %ld procesos:\n" RESET, result);
        printf(CYAN "+-------+----------------+----------------+-----------------+-----------------+------------+---------------+\n" RESET);
        printf(CYAN "|  PID  | Virtual Mem KB | Virtual Mem MB | Physical Mem KB | Physical Mem MB | %% Commit   | OOM Score    |\n" RESET);
        printf(CYAN "+-------+----------------+----------------+-----------------+-----------------+------------+---------------+\n" RESET);

        for (int i = 0; i < result; i++) {
            printf("| " MAGENTA "%5d" RESET " | " YELLOW "%14lu" RESET " | " YELLOW "%14lu" RESET " | " GREEN "%15lu" RESET " | " GREEN "%15lu" RESET " | " RED "%10lu%%" RESET " | " BLUE "%12d" RESET " |\n",
                   stats[i].process_id,
                   stats[i].virtual_mem_kb,
                   stats[i].virtual_mem_mb,
                   stats[i].physical_mem_kb,
                   stats[i].physical_mem_mb,
                   stats[i].commit_percentage,
                   stats[i].oom_adjust_score);
        }

        printf(CYAN "+-------+----------------+----------------+-----------------+-----------------+------------+---------------+\n" RESET);
    } else {
        perror(RED "Error al ejecutar la syscall" RESET);
        printf(RED "Código de error: %ld (errno: %d)\n" RESET, result, errno);
    }

    return 0;
}
