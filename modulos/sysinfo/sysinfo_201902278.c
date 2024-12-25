#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sched/signal.h> // Necesario para iterar procesos
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/statfs.h>
#include <linux/file.h>
#include <linux/jiffies.h>

#define PROC_NAME "sysinfo_json"
#define PARTITION_PATH "/boot/efi"

// Helper para obtener estadísticas de almacenamiento
static int get_storage_stats(const char *path, struct kstatfs *stat) {
    struct file *file;
    int ret;

    // Abrir el archivo para obtener la ruta al sistema de archivos
    file = filp_open(path, O_PATH, 0);
    if (IS_ERR(file)) {
        return PTR_ERR(file);
    }

    // Obtener estadísticas del sistema de archivos
    ret = vfs_statfs(&file->f_path, stat);

    // Cerrar el archivo
    filp_close(file, NULL);

    return ret;
}

// Helper para obtener estadísticas de uso de CPU desde /proc/stat
static void get_cpu_usage(unsigned long *user, unsigned long *system, unsigned long *idle) {
    unsigned long tmp_user, tmp_nice, tmp_system, tmp_idle;
    char buf[128];
    struct file *file;
    ssize_t count;

    // Abrir el archivo /proc/stat
    file = filp_open("/proc/stat", O_RDONLY, 0);
    if (IS_ERR(file)) {
        *user = *system = *idle = 0;
        return;
    }

    // Leer el contenido del archivo
    count = kernel_read(file, buf, sizeof(buf) - 1, 0);
    if (count < 0) {
        *user = *system = *idle = 0;
        filp_close(file, NULL);
        return;
    }

    buf[count] = '\0';
    filp_close(file, NULL);

    // Parsear la línea de CPU
    sscanf(buf, "cpu %lu %lu %lu %lu", &tmp_user, &tmp_nice, &tmp_system, &tmp_idle);

    *user = tmp_user + tmp_nice;
    *system = tmp_system;
    *idle = tmp_idle;
}

// Mostrar estadísticas en formato JSON
static int system_stats_show(struct seq_file *m, void *v) {
    struct sysinfo si;
    struct kstatfs stat;
    unsigned long total_memory, free_memory, total_storage, free_storage;
    unsigned long cpu_user, cpu_system, cpu_idle;
    unsigned long total_cpu;
    int total_threads = 0;
    struct task_struct *task;

    // Obtener estadísticas de memoria
    si_meminfo(&si);
    total_memory = si.totalram * si.mem_unit;
    free_memory = si.freeram * si.mem_unit;

    // Obtener estadísticas de almacenamiento
    if (get_storage_stats(PARTITION_PATH, &stat) != 0) {
        seq_printf(m, "{\"error\": \"Error retrieving storage stats\"}\n");
        return -EINVAL;
    }
    total_storage = stat.f_blocks * stat.f_bsize;
    free_storage = stat.f_bfree * stat.f_bsize;

    // Contar el número total de hilos
    for_each_process(task) {
        total_threads += get_nr_threads(task);
    }

    // Obtener estadísticas de uso de CPU
    get_cpu_usage(&cpu_user, &cpu_system, &cpu_idle);
    total_cpu = cpu_user + cpu_system + cpu_idle;

    // Generar salida en formato JSON
    seq_printf(m, "{\n");
    seq_printf(m, "  \"cpu_stats\": {\n");
    seq_printf(m, "    \"total_threads\": %d,\n", total_threads);
    seq_printf(m, "    \"user_time\": %lu,\n", cpu_user * 100 / total_cpu);   // % de uso en usuario
    seq_printf(m, "    \"system_time\": %lu,\n", cpu_system * 100 / total_cpu); // % de uso en sistema
    seq_printf(m, "    \"idle_time\": %lu\n", cpu_idle * 100 / total_cpu);    // % de tiempo inactivo
    seq_printf(m, "  },\n");

    seq_printf(m, "  \"memory_stats\": {\n");
    seq_printf(m, "    \"total_memory\": %lu,\n", total_memory);
    seq_printf(m, "    \"free_memory\": %lu\n", free_memory);
    seq_printf(m, "  },\n");

    seq_printf(m, "  \"storage_stats\": {\n");
    seq_printf(m, "    \"total_storage\": %lu,\n", total_storage);
    seq_printf(m, "    \"free_storage\": %lu\n", free_storage);
    seq_printf(m, "  }\n");
    seq_printf(m, "}\n");

    return 0;
}

static int system_stats_open(struct inode *inode, struct file *file) {
    return single_open(file, system_stats_show, NULL);
}

static const struct proc_ops system_stats_fops = {
    .proc_open = system_stats_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init system_stats_init(void) {
    if (!proc_create(PROC_NAME, 0444, NULL, &system_stats_fops)) {
        return -ENOMEM;
    }

    printk(KERN_INFO "System stats JSON module loaded\n");
    return 0;
}

static void __exit system_stats_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "System stats JSON module unloaded\n");
}

module_init(system_stats_init);
module_exit(system_stats_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Estuardo Palacios Q");
MODULE_DESCRIPTION("Module for system stats in JSON format");
