#ifndef _SYSCALL_USAC3_H
#define _SYSCALL_USAC3_H

#include <linux/types.h>

// Estructura para almacenar las estadísticas de I/O
struct io_stats {
    unsigned long read_bytes; 
    unsigned long write_bytes;//ya
    unsigned long read_operations;//ya
    unsigned long write_operations;
    unsigned long io_wait_time;//ya
};

// Prototipo de la syscall (declaración en espacio de usuario)
long sys_get_io_throttle_201902278(pid_t pid, struct io_stats *stats);

#endif // SYSCALL_USACCC_3

