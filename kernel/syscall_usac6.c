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
