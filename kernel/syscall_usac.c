#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/vmstat.h>

struct mem_snapshot {
    __u64 total_memory;
    __u64 free_memory;
    __u64 active_memory;
    __u64 swap_used;
    __u64 cache_size;
};


// Implementación de la syscall
SYSCALL_DEFINE1(capture_memory_snapshot_201902278, struct mem_snapshot __user *, snapshot)
{
    struct mem_snapshot ksnapshot;
    struct sysinfo i;

    // Obtén información del sistema
    si_meminfo(&i);

    // llenar la estructura de memoria
    ksnapshot.total_memory = i.totalram << (PAGE_SHIFT - 10);  // En KB
    ksnapshot.free_memory = i.freeram << (PAGE_SHIFT - 10);    // En KB
    ksnapshot.active_memory = global_node_page_state(NR_ACTIVE_ANON) << (PAGE_SHIFT - 10);
    ksnapshot.swap_used = (i.totalswap - i.freeswap) << (PAGE_SHIFT - 10);
    ksnapshot.cache_size = global_node_page_state(NR_FILE_PAGES) << (PAGE_SHIFT - 10);

    // Dar replica de datos al usuario
    if (copy_to_user(snapshot, &ksnapshot, sizeof(struct mem_snapshot)))
        return -EFAULT;

    return 0;
}

