// -------------- Segundo Syscall
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>

#define MAX_SYSCALLS 1024

struct syscall_usage {
    unsigned long count;
    struct timespec64 last_called;
};

static struct syscall_usage *syscall_counters;

static int init_syscall_counters(void) {
    syscall_counters = kzalloc(sizeof(struct syscall_usage) * MAX_SYSCALLS, GFP_KERNEL);
    
    if (!syscall_counters) {
        printk(KERN_ERR "track_syscall_usage: Failed to allocate memory for syscall counters\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "track_syscall_usage: Initialized syscall counters\n");

	return 0;

}

static DEFINE_SPINLOCK(syscall_lock);

// Identificadores de syscalls
#define SYS_OPEN 2
#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_FORK 56

early_initcall(init_syscall_counters);

void track_syscall(int syscall_id) {
    struct timespec64 now;
	


    // Verificamos que syscall_counters haya sido inicializado correctamente
    if (!syscall_counters || syscall_id >= MAX_SYSCALLS) {
        printk(KERN_ERR "track_syscall: Invalid syscall_id = %d\n", syscall_id);
        return;
    }

    spin_lock(&syscall_lock);

    syscall_counters[syscall_id].count++;
    ktime_get_real_ts64(&now);
    syscall_counters[syscall_id].last_called = now;

    spin_unlock(&syscall_lock);
}

EXPORT_SYMBOL(track_syscall);

// Función para rastrear las syscalls open, read, write y fork
void track_open_read_write_fork_syscall(int syscall_id) {
    if (syscall_id == SYS_OPEN || syscall_id == SYS_READ || syscall_id == SYS_WRITE || syscall_id == SYS_FORK) {
        track_syscall(syscall_id);
    }
}

SYSCALL_DEFINE1(track_syscall_usage_201902278, struct syscall_usage __user *, statistics) {
    int result;

    printk(KERN_INFO "track_syscall_usage: Entering syscall\n");

    if (!syscall_counters) {
        if (init_syscall_counters() != 0) {
            return -ENOMEM;
        }
    }

    spin_lock(&syscall_lock);

    result = copy_to_user(statistics, syscall_counters, sizeof(struct syscall_usage) * MAX_SYSCALLS);

    spin_unlock(&syscall_lock);

    if (result != 0) {
        printk(KERN_ERR "track_syscall_usage: Failed to copy data to user space (result = %d)\n", result);
        return -EFAULT;
    }

    printk(KERN_INFO "track_syscall_usage: Successfully copied data to user space\n");
    return 0;
}
