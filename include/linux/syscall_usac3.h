#ifndef _SYSCALL_USAC3_H
#define _SYSCALL_USAC3_H

#include <linux/types.h>

// Estructura para almacenar las estadísticas de I/O
struct io_stats {
    unsigned long lectura_bytes;
    unsigned long escribir_bytes;
    unsigned long leer_operations;
    unsigned long escribir_operations;
    unsigned long io_wait_time;
};

long sys_get_io_throttle_201902278(pid_t pid, struct io_stats *stats);

#endif 
