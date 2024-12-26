#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/uaccess.h>
#include <linux/list.h>

// Estructura de estadísticas de memoria
struct process_memory_stats {
    pid_t pid;                         // PID del proceso
    unsigned long reserved_kb;         // Memoria reservada en KB
    unsigned long reserved_mb;         // Memoria reservada en MB
    unsigned long committed_kb;        // Memoria comprometida en KB
    unsigned long committed_mb;        // Memoria comprometida en MB
    unsigned long committed_percent;   // Porcentaje de memoria comprometida
    int oom_score;                     // OOM Score
};

SYSCALL_DEFINE3(memory_usage_table_201902278, pid_t, pid, struct process_memory_stats __user *, stats, int, max_processes) {
    struct task_struct *task;
    struct mm_struct *mm;
    struct process_memory_stats local_stats;
    int count = 0;

    // Validar el puntero de usuario
    if (!stats || max_processes <= 0)
        return -EINVAL;

    if (pid == 0) {
        // Recorrer todos los procesos
        for_each_process(task) {
            if (count >= max_processes)
                break;

            mm = get_task_mm(task);
            if (!mm)
                continue;

            // Calcular estadísticas de memoria
            local_stats.pid = task->pid;  // Asignar el PID del proceso
            local_stats.reserved_kb = mm->total_vm * (PAGE_SIZE / 1024);
            local_stats.reserved_mb = local_stats.reserved_kb / 1024; // Convertir a MB
            local_stats.committed_kb = (get_mm_counter(mm, MM_ANONPAGES) + get_mm_counter(mm, MM_FILEPAGES)) * (PAGE_SIZE / 1024);
            local_stats.committed_mb = local_stats.committed_kb / 1024; // Convertir a MB

            if (local_stats.reserved_kb > 0) {
                local_stats.committed_percent = (local_stats.committed_kb * 100) / local_stats.reserved_kb;
            } else {
                local_stats.committed_percent = 0;
            }

            // Obtener el OOM score
            local_stats.oom_score = task->signal->oom_score_adj;

            // Copiar los resultados de este proceso al buffer de usuario
            if (copy_to_user(&stats[count], &local_stats, sizeof(local_stats))) {
                mmput(mm);
                return -EFAULT;
            }

            mmput(mm);
            count++;
        }
        return count; // Retornar el número de procesos copiados
    }

    // Si PID no es 0, manejar como antes
    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        rcu_read_unlock();
        return -ESRCH;
    }

    mm = get_task_mm(task);
    if (!mm) {
        rcu_read_unlock();
        return -EINVAL;
    }

    // Calcular estadísticas de memoria
    local_stats.pid = task->pid;  // Asignar el PID del proceso
    local_stats.reserved_kb = mm->total_vm * (PAGE_SIZE / 1024);
    local_stats.reserved_mb = local_stats.reserved_kb / 1024; // Convertir a MB
    local_stats.committed_kb = (get_mm_counter(mm, MM_ANONPAGES) + get_mm_counter(mm, MM_FILEPAGES)) * (PAGE_SIZE / 1024);
    local_stats.committed_mb = local_stats.committed_kb / 1024; // Convertir a MB

    if (local_stats.reserved_kb > 0) {
        local_stats.committed_percent = (local_stats.committed_kb * 100) / local_stats.reserved_kb;
    } else {
        local_stats.committed_percent = 0;
    }

    local_stats.oom_score = task->signal->oom_score_adj;

    mmput(mm);
    rcu_read_unlock();

    if (copy_to_user(stats, &local_stats, sizeof(local_stats)))
        return -EFAULT;

    return 1; // Retornar 1 proceso copiado
}
