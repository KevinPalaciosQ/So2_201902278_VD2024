#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/cred.h>
#include <linux/errno.h>
#include <linux/uaccess.h> // Para copy_to_user
#include <linux/sched/task.h> // Para obtener estadísticas de procesos

// Estructura para almacenar las estadísticas de I/O
struct io_statistics {
    unsigned long total_read_bytes;
    unsigned long total_write_bytes;
    unsigned long total_read_operations;
    unsigned long total_write_operations;
    unsigned long total_io_wait_time;
};

SYSCALL_DEFINE2(get_io_throttle_201902278, pid_t, pid, struct io_statistics __user *, stats)
{
    struct task_struct *task;
    struct io_statistics kernel_stats;

    // Inicializar la estructura de estadísticas
    memset(&kernel_stats, 0, sizeof(kernel_stats));

    // Obtener la estructura task_struct del proceso
    task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task)
        return -ESRCH;

    // Obtener estadísticas de I/O del proceso desde task_io_accounting
    kernel_stats.total_read_bytes = READ_ONCE(task->ioac.read_bytes);
    kernel_stats.total_write_bytes = READ_ONCE(task->ioac.write_bytes);
    kernel_stats.total_read_operations = READ_ONCE(task->ioac.rchar);
    kernel_stats.total_write_operations = READ_ONCE(task->ioac.wchar);
    kernel_stats.total_io_wait_time = READ_ONCE(task->se.sum_exec_runtime);

    // Copiar los datos al espacio de usuario
    if (copy_to_user(stats, &kernel_stats, sizeof(kernel_stats)))
        return -EFAULT;

    return 0;
}
