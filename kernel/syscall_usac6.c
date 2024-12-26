#include <linux/kernel.h>    // Funciones básicas y macros del kernel
#include <linux/syscalls.h>  // Declaraciones para definir syscalls
#include <linux/mm.h>        // Definiciones relacionadas con la memoria, como PAGE_SIZE
#include <linux/mm_types.h>  // Tipos de datos para estructuras de memoria
#include <linux/uaccess.h>   // Funciones para transferir datos entre kernel y espacio de usuario

// Estructura de estadísticas de memoria
struct total_memory_stats {
    unsigned long total_reserved_mb; // Memoria total reservada en MB
    unsigned long total_used_mb;     // Memoria total utilizada en MB (basada en procesos)
};

// Definición del syscall
SYSCALL_DEFINE1(memory_usage_general_201902278, struct total_memory_stats __user *, stats) {
    struct total_memory_stats kmem_stats = {0, 0};
    struct sysinfo si;
    struct task_struct *task;
    struct mm_struct *mm;

    if (!stats)
        return -EINVAL;

    // Obtener información del sistema
    si_meminfo(&si);

    // Memoria total y libre en páginas
    unsigned long total_ram = si.totalram;
    unsigned long free_ram = si.freeram;

    // Cálculo de memoria total reservada (convertir páginas a MB)
    kmem_stats.total_reserved_mb = (total_ram * PAGE_SIZE) / (1024 * 1024);

    // Calcular memoria utilizada basada en procesos
    for_each_process(task) {
        mm = get_task_mm(task);
        if (mm) {
            // Memoria RSS (Resident Set Size) y Swap de cada proceso
            kmem_stats.total_used_mb += ((get_mm_rss(mm) + get_mm_counter(mm, MM_SWAPENTS)) * PAGE_SIZE) / (1024 * 1024);
            mmput(mm);
        }
    }

    // Copiar resultados al espacio de usuario
    if (copy_to_user(stats, &kmem_stats, sizeof(kmem_stats)))
        return -EFAULT;

    return 0;
}