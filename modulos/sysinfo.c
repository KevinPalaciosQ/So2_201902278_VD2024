#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h> 
#include <linux/seq_file.h> 
#include <linux/mm.h> 
#include <linux/sched.h> 
#include <linux/timer.h> 
#include <linux/jiffies.h> 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Palacios-201902278");
MODULE_DESCRIPTION("Modulo para leer Estadísticas del CPU, memoria y almacenamiento");
MODULE_VERSION("1.0");
#define PROC_NAME "sysinfo_201902278"