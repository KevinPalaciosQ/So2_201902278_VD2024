#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/uaccess.h>
#include <linux/list.h>

// Estructura de estadísticas de memoria
struct memory_stats {
    pid_t process_id;                   // PID del proceso
    unsigned long virtual_mem_kb;       // Memoria virtual reservada en KB
    unsigned long virtual_mem_mb;       // Memoria virtual reservada en MB
    unsigned long physical_mem_kb;      // Memoria física comprometida en KB
    unsigned long physical_mem_mb;      // Memoria física comprometida en MB
    unsigned long commit_percentage;    // Porcentaje de memoria comprometida
    int oom_adjust_score;               // OOM Score
};

SYSCALL_DEFINE3(memory_usage_table_201902278, pid_t, pid, struct memory_stats __user *, stats, int, max_processes) {
    struct task_struct *task;
    struct mm_struct *mm;
    struct memory_stats local_stats;
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
            local_stats.process_id = task->pid;  // Asignar el PID del proceso
            local_stats.virtual_mem_kb = mm->total_vm * (PAGE_SIZE / 1024);
            local_stats.virtual_mem_mb = local_stats.virtual_mem_kb / 1024; // Convertir a MB
            local_stats.physical_mem_kb = (get_mm_counter(mm, MM_ANONPAGES) + get_mm_counter(mm, MM_FILEPAGES)) * (PAGE_SIZE / 1024);
            local_stats.physical_mem_mb = local_stats.physical_mem_kb / 1024; // Convertir a MB

            if (local_stats.virtual_mem_kb > 0) {
                local_stats.commit_percentage = (local_stats.physical_mem_kb * 100) / local_stats.virtual_mem_kb;
            } else {
                local_stats.commit_percentage = 0;
            }

            // Obtener el OOM score
            local_stats.oom_adjust_score = task->signal->oom_score_adj;

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

    // Si PID no es 0, buscar especificamente
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
    local_stats.process_id = task->pid;  // Asignar el PID del proceso
    local_stats.virtual_mem_kb = mm->total_vm * (PAGE_SIZE / 1024);
    local_stats.virtual_mem_mb = local_stats.virtual_mem_kb / 1024; // Convertir a MB
    local_stats.physical_mem_kb = (get_mm_counter(mm, MM_ANONPAGES) + get_mm_counter(mm, MM_FILEPAGES)) * (PAGE_SIZE / 1024);
    local_stats.physical_mem_mb = local_stats.physical_mem_kb / 1024; // Convertir a MB

    if (local_stats.virtual_mem_kb > 0) {
        local_stats.commit_percentage = (local_stats.physical_mem_kb * 100) / local_stats.virtual_mem_kb;
    } else {
        local_stats.commit_percentage = 0;
    }

    local_stats.oom_adjust_score = task->signal->oom_score_adj;

    mmput(mm);
    rcu_read_unlock();

    if (copy_to_user(stats, &local_stats, sizeof(local_stats)))
        return -EFAULT;

    return 1; // Retornar 1 proceso copiado
}
