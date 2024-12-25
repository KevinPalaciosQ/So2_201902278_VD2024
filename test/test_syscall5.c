#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define MAX_PROCESSES 128
#define SYSCALL_NUMBER 553

struct process_memory_stats {
    pid_t pid;                         // PID del proceso
    unsigned long reserved_kb;         // Memoria reservada en KB
    unsigned long committed_kb;        // Memoria comprometida en KB
    unsigned long committed_percent;   // Porcentaje de memoria comprometida
    int oom_score;                     // OOM Score
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <PID o 0 para todos los procesos>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    struct process_memory_stats stats[MAX_PROCESSES];

    // Invocar la syscall
    long result = syscall(SYSCALL_NUMBER, pid, stats, MAX_PROCESSES);

    if (result >= 0) {
        printf("Se obtuvieron datos de %ld procesos:\n", result);
        printf("+-------+-----------+---------------+---------------+-----------+\n");
        printf("|  PID  | VmSize (KB) |Committed (KB)| %% Commit      | OOM Score |\n");
        printf("+-------+-----------+---------------+---------------+-----------+\n");
        for (int i = 0; i < result; i++) {
            printf("| %5d | %10lu | %13lu | %13lu%% | %9d |\n",
                   stats[i].pid,
                   stats[i].reserved_kb,
                   stats[i].committed_kb,
                   stats[i].committed_percent,
                   stats[i].oom_score);
        }
    } else {
        perror("Error al ejecutar la syscall");
        printf("Código de error: %ld (errno: %d)\n", result, errno);
    }

    return 0;
}
