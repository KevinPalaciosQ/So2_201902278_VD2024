#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/cred.h>
#include <linux/errno.h>
#include <linux/uaccess.h> // Para copy_to_user
#include <linux/sched/task.h> // Para obtener estadísticas de procesos

// Estructura para almacenar las estadísticas de I/O
struct io_stats {
    unsigned long read_bytes;
    unsigned long write_bytes;
    unsigned long read_operations;
    unsigned long write_operations;
    unsigned long io_wait_time;
};

SYSCALL_DEFINE2(get_io_throttle_201902278, pid_t, pid, struct io_stats __user *, stats)
{
    struct task_struct *task;
    struct io_stats stats_kernel;

    // Inicializar la estructura de estadísticas
    memset(&stats_kernel, 0, sizeof(stats_kernel));

    // Obtener la estructura task_struct del proceso
    task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task)
        return -ESRCH;

    // Obtener estadísticas de I/O del proceso desde task_io_accounting
    stats_kernel.read_bytes = READ_ONCE(task->ioac.read_bytes);
    stats_kernel.write_bytes = READ_ONCE(task->ioac.write_bytes);
    stats_kernel.read_operations = READ_ONCE(task->ioac.rchar);
    stats_kernel.write_operations = READ_ONCE(task->ioac.wchar);
    stats_kernel.io_wait_time = READ_ONCE(task->se.sum_exec_runtime);

    // Copiar los datos al espacio de usuario
    if (copy_to_user(stats, &stats_kernel, sizeof(stats_kernel)))
        return -EFAULT;

    return 0;
}
