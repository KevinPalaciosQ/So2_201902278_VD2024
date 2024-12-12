#ifndef _SYSCALL_USAC_H_
#define _SYSCALL_USAC_H_

//asmlinkage long sys_capture_memory_snapshot_201902278(void);

#include <linux/types.h>

// Estructura para almacenar información de memoria
struct mem_snapshot {
    __u64 total_memory;
    __u64 free_memory;
    __u64 active_memory;
    __u64 swap_used;
    __u64 cache_size;
};

// Prototipo de la syscall
long sys_capture_memory_snapshot_201902278(struct mem_snapshot __user *snapshot);
#endif 