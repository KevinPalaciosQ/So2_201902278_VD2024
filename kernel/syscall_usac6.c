#include <linux/kernel.h>    // Funciones básicas y macros del kernel
#include <linux/syscalls.h>  // Declaraciones para definir syscalls
#include <linux/mm.h>        // Definiciones relacionadas con la memoria, como PAGE_SIZE
#include <linux/mm_types.h>  // Tipos de datos para estructuras de memoria
#include <linux/uaccess.h>   // Funciones para transferir datos entre kernel y espacio de usuario


struct estadisticas_memoria {
    unsigned long memoria_reservada_mb; // Memoria total reservada en MB
    unsigned long memoria_utilizada_mb; // Memoria total utilizada en MB (basada en procesos)
    unsigned long memoria_libre_mb;     // Memoria libre en MB
};

// Definición del syscall
SYSCALL_DEFINE1(memory_usage_general_201902278, struct estadisticas_memoria __user *, estadisticas) {
    struct estadisticas_memoria kstats_memoria = {0, 0, 0};
    struct sysinfo si;
    struct task_struct *tarea;
    struct mm_struct *mm;

    if (!estadisticas)
        return -EINVAL;

    // Obtener información del sistema
    si_meminfo(&si);

    // Memoria total y libre en páginas
    unsigned long memoria_total = si.totalram;
    unsigned long memoria_libre = si.freeram;

    // Cálculo de memoria total reservada (convertir páginas a MB)
    kstats_memoria.memoria_reservada_mb = (memoria_total * PAGE_SIZE) / (1024 * 1024);

    // Cálculo de memoria libre (convertir páginas a MB)
    kstats_memoria.memoria_libre_mb = (memoria_libre * PAGE_SIZE) / (1024 * 1024);

    // Calcular memoria utilizada basada en procesos
    for_each_process(tarea) {
        mm = get_task_mm(tarea);
        if (mm) {
            // Memoria RSS (Resident Set Size) y Swap de cada proceso
            kstats_memoria.memoria_utilizada_mb += ((get_mm_rss(mm) + get_mm_counter(mm, MM_SWAPENTS)) * PAGE_SIZE) / (1024 * 1024);
            mmput(mm);
        }
    }

    // Copiar resultados al espacio de usuario
    if (copy_to_user(estadisticas, &kstats_memoria, sizeof(kstats_memoria)))
        return -EFAULT;

    return 0;
}
