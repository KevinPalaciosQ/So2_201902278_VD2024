#include <linux/kernel.h>//macros, funciones y definiciones
#include <linux/syscalls.h>//declaración e implementación a las syscalls
#include <linux/sched.h>//utilizada para planificación de procesos
#include <linux/pid.h>//funciones macros para PID
#include <linux/cred.h>//proporciona acceso a credenciales
#include <linux/errno.h>//define los códigos de error relacionados al kernel

// Estructura para almacenar las estadísticas de I/O
struct io_stats {
    unsigned long lectura_bytes;
    unsigned long escribir_bytes;
    unsigned long leer_operations;
    unsigned long escribir_operations;
    unsigned long io_wait_time;
};

SYSCALL_DEFINE2(get_io_throttle_201902278, pid_t, pid, struct io_stats __user *, stats)
{
    struct task_struct *task;
    struct io_stats stats_kernel;

    // Obtener la estructura task_struct del proceso
    task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task)
        return -ESRCH;

    // Copiar estadísticas de I/O del proceso
    stats_kernel.lectura_bytes = READ_ONCE(task->ioac.lectura_bytes);
    stats_kernel.escribir_bytes = READ_ONCE(task->ioac.escribir_bytes);
    stats_kernel.io_wait_time = READ_ONCE(task->se.sum_exec_runtime);

    // Copiar los datos al espacio de usuario
    if (copy_to_user(stats, &stats_kernel, sizeof(stats_kernel)))
        return -EFAULT;

    return 0;
}