/*
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched/signal.h>

// Estructura para retornar estadísticas
struct memstats {
    unsigned long reserved_memory;  // Memoria reservada (en KB)
    unsigned long used_memory;      // Memoria utilizada (en KB)
};

// Syscall: Recolectar estadísticas generales de memoria
SYSCALL_DEFINE1(memory_usage_general_201902278, struct memstats __user *, stats) {
    struct memstats kstats;
    struct task_struct *task;

    unsigned long used = 0;

    // Obtener información global del sistema
    struct sysinfo si;
    si_meminfo(&si);

    // Memoria total reservada (en KB): Total RAM + Swap
    unsigned long reserved = si.totalram + si.totalswap;

    // Iterar sobre todos los procesos para calcular la memoria utilizada
    for_each_process(task) {
        struct mm_struct *mm = task->mm;
        if (mm) {
            down_read(&mm->mmap_lock);
            used += get_mm_rss(mm); // Páginas utilizadas
            up_read(&mm->mmap_lock);
        }
    }

    // Convertir los valores a KB
    kstats.reserved_memory = reserved * (PAGE_SIZE / 1024);
    kstats.used_memory = used * (PAGE_SIZE / 1024);

    // Copiar los resultados al espacio de usuario
    if (copy_to_user(stats, &kstats, sizeof(struct memstats)))
        return -EFAULT;

    return 0;
}
*/
//la syscall usada es memory_usage_general_201902278
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>

struct total_memory_stats {
    unsigned long total_reserved_kb;
    unsigned long total_commited_kb;
};

SYSCALL_DEFINE1(memory_usage_general_201902278, struct total_memory_stats __user *, stats) {
    struct task_struct *task;
    struct mm_struct *mm;
    struct total_memory_stats total_stats = {0, 0};

    if (!stats)
        return -EINVAL;

    // Recorrer todos los procesos activos
    for_each_process(task) {
        mm = get_task_mm(task);

        if (mm) {
            total_stats.total_reserved_kb += get_mm_rss(mm) * PAGE_SIZE / 1024;
            total_stats.total_commited_kb += get_mm_counter(mm, MM_FILEPAGES) * PAGE_SIZE / 1024;
            mmput(mm);
        }
    }

    if (copy_to_user(stats, &total_stats, sizeof(total_stats)))
        return -EFAULT;

    return 0;
}
