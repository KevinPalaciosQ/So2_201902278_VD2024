#include <linux/kernel.h>    // Funciones básicas y macros del kernel
#include <linux/syscalls.h>  // Declaraciones para definir syscalls
#include <linux/mm.h>        // Definiciones relacionadas con la memoria, como PAGE_SIZE
#include <linux/sysinfo.h>   // Estructura sysinfo y función si_meminfo
#include <linux/uaccess.h>   // Funciones para transferir datos entre kernel y espacio de usuario

// Estructura de estadísticas de memoria
struct total_memory_stats {
    unsigned long total_reserved_mb; // Memoria total reservada en MB
    unsigned long total_used_mb;     // Memoria total utilizada en MB (excluyendo buffers y cachés)
    unsigned long total_swap_mb;     // Memoria swap total en MB
    unsigned long used_swap_mb;      // Memoria swap utilizada en MB
};

// Definición del syscall
SYSCALL_DEFINE1(memory_usage_general_201902278, struct total_memory_stats __user *, stats) {
    struct total_memory_stats kmem_stats;
    struct sysinfo si;

    if (!stats)
        return -EINVAL;

    // Obtener información del sistema
    si_meminfo(&si);

    // Memoria total y libre en páginas
    unsigned long total_ram = si.totalram;
    unsigned long free_ram = si.freeram;

    // Memoria swap total y libre en páginas
    unsigned long total_swap = si.totalswap;
    unsigned long free_swap = si.freeswap;

    // Cálculo de memoria total reservada (convertir páginas a MB)
    kmem_stats.total_reserved_mb = (total_ram * PAGE_SIZE) / (1024 * 1024);

    // Cálculo de memoria utilizada (total - libre - buffers/cachés)
    kmem_stats.total_used_mb = (((total_ram - free_ram - si.bufferram) * PAGE_SIZE) / (1024 * 1024));

    // Cálculo de memoria swap total y utilizada
    kmem_stats.total_swap_mb = (total_swap * PAGE_SIZE) / (1024 * 1024);
    kmem_stats.used_swap_mb = ((total_swap - free_swap) * PAGE_SIZE) / (1024 * 1024);

    // Copiar resultados al espacio de usuario
    if (copy_to_user(stats, &kmem_stats, sizeof(kmem_stats)))
        return -EFAULT;

    return 0;
}
