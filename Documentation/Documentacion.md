# _Manual Técnico ☝️🤓_

## 📚 Contenido

- 🎯 Objetivos
- 📍 Tecnologías
- 🛠️ Herramientas
- 💼 Paquetes
- 🛡️ Guía Instalación
- Informe Técnico ☢️
- Detalles de las syscalls y modulo de Kernel 🕵️
- Pruebas Realizadas 🩻
- 🤔 Reflexión Personal y autoevaluación
## 🎯 Objetivos

## General

Modificar y personalizar el kernel de linux para agregar nuevas funcionalidades y llamadas al sistema, adquiriendo experiencia en la programación de sistemas operativos y comprender los principios básicos de estos mismos.

## Específicos

- Configurar el entorno de desarollo.
- Compilación y personalización del kernel.
- Desarollo de nuevas llamadas al sistema.
- Creación de modulos de kernel.

## 📍 Tecnologías

Estas son las tecnologías, herramientas y paquetes utilizados en el proyecto:

- [**Kernel linux mint versión 6.8.0**](https://www.kernel.org/pub/linux/kernel/v6.x/linux-6.8.tar.xz)
- [**Gitkraken**](https://www.gitkraken.com/)
## 🛠️ Herramientas

- [**Oracle VM VirtualBox Administrator:**](https://www.virtualbox.org/wiki/Downloads)
- [**Visual Studio Code:**](https://code.visualstudio.com/download)

## 💼 Paquetes
- GCC
- MAKE
- libncurses-dev
- libncurses5-dev
## 🛡️ Guía Instalación
Ejecutar los siguientes comandos para instalar las herramientas y librerías.
```bash
sudo apt-get install build-essential libncurses5-dev fakeroot wget bzip2 openssl
sudo apt-get install build-essential libncurses-dev bison flex libssl-dev libelf-dev
```
## Copiar configuración del kernel en uso:

```bash
cp -v /boot/config-$(uname -r) .config
```
## Desactivación de Llaves de Confianza

Ejecuta los siguientes comandos para desactivar las llaves de confianza del sistema:

```bash
scripts/config --disable SYSTEM_TRUSTED_KEYS
scripts/config --disable SYSTEM_REVOCATION_KEYS
```
### Explicación

- **SYSTEM_TRUSTED_KEYS**: Llaves usadas para verificar módulos firmados.
- **SYSTEM_REVOCATION_KEYS**: Llaves revocadas del sistema.

Estos comandos ajustan la configuración para deshabilitar estas características.

## Cambio de la Variable EXTRAVERSION

Para cambiar el valor de **EXTRAVERSION** en el archivo de configuración del kernel, sigue estos pasos:

1. Abre el archivo `Makefile` en el directorio raíz del código fuente del kernel:

   ```bash
   nano Makefile
   ```

2. Busca la línea que contiene **EXTRAVERSION**:

   ```Makefile
   EXTRAVERSION = -49-usac1
   ```

3. Cambia el valor de **EXTRAVERSION** según tus necesidades. Por ejemplo:

   ```Makefile
   EXTRAVERSION = -custom1
   ```

4. Guarda los cambios y cierra el editor.

   - En `nano`, presiona `CTRL+O` para guardar y `CTRL+X` para salir.
5. Verifica los cambios ejecutando:

   ```bash
   uname -r
   ```
### Nota 📝
El valor de **EXTRAVERSION** se agrega al número de versión del kernel para identificar compilaciones personalizadas.

## Recompilación del Kernel

Para recompilar el kernel, sigue los pasos a continuación:

1. Cambia al usuario root:

   ```bash
   sudo -s
   ```

2. Ve al directorio del código fuente del kernel:

   ```bash
   cd Linux/6.8
   ```

3. Limpia los archivos de compilaciones anteriores:

   ```bash
   make clean
   ```

4. Carga la configuración actual del kernel:

   ```bash
   make oldconfig
   ```

5. Opcional: Ajusta la configuración del kernel usando una interfaz de menú:

   ```bash
   make menuconfig
   ```

6. Genera una configuración basada en los módulos cargados actualmente:

   ```bash
   make localmodconfig
   ```

7. Compila el kernel utilizando todos los núcleos menos uno para optimizar:

   ```bash
   make -j$(nproc --ignore=1)
   ```

## Crear un Enlace Simbólico

Ejecuta el siguiente comando para crear un enlace simbólico:

```bash
ln -s /home/kevin/Documents/So2_201902278_VD2024/arch/x86/entry/syscalls/syscall_64.tbl syscall_64.tbl
```


### Explicación del Comando

- **ln**: Comando para crear enlaces (duros o simbólicos).
- **-s**: Especifica que el enlace debe ser simbólico (soft link).
- `/home/kevin/Documents/So2_201902278_VD2024/arch/x86/entry/syscalls/syscall_64.tbl`: Ruta absoluta del archivo original.
- `syscall_64.tbl`: Nombre del enlace simbólico que se creará en el directorio actual.

### Resultado

Después de ejecutar el comando:
- Se crea un archivo simbólico llamado `syscall_64.tbl` en el directorio actual.
- Este enlace apunta al archivo original en `/home/kevin/Documents/So2_201902278_VD2024/arch/x86/entry/syscalls/syscall_64.tbl`.

Luego de crear los enlaces simbolicos comenzamos con la elaboración de los modulos de kernel, para eso debemos de modificar/crear los siguientes archivos 

# Modificaciones en módulo de Kernel 
obj-y += syscall_usac.o 
## Makefile
- se modificó <u>**obj-m += syscall_usac.o**</u> en un **Makefile** para la construcción de un kernel de linux.

se modificó **obj-m += syscall_usac.o**

## syscall_usac.c 📞
En el archivo de **syscall_usac.c** se implementan las 3 llamadas al sistema (syscalls), estas son: 
- capture_memory_snapshot_201902278
- track_syscall_usage_201902278.
- get_io_throttle_201902278.
## syscall_64.tbl
En la syscall.tbl se asigna el número de syscall a llamar las syscall del proyecto.
## Informe Técnico

 ## capture_memory_snapshot_201902278
   ```c
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/vmstat.h>

struct mem_snapshot {
    __u64 total_memory;
    __u64 free_memory;
    __u64 active_memory;
    __u64 swap_used;
    __u64 cache_size;
};


// Implementación de la primer Syscall
SYSCALL_DEFINE1(capture_memory_snapshot_201902278, struct mem_snapshot __user *, snapshot)
{
    struct mem_snapshot ksnapshot;
    struct sysinfo i;

    // Obtén información del sistema
    si_meminfo(&i);

    // Llena la estructura con datos de memoria
    ksnapshot.total_memory = i.totalram << (PAGE_SHIFT - 10);  // En KB
    ksnapshot.free_memory = i.freeram << (PAGE_SHIFT - 10);    // En KB
    ksnapshot.active_memory = global_node_page_state(NR_ACTIVE_ANON) << (PAGE_SHIFT - 10);
    ksnapshot.swap_used = (i.totalswap - i.freeswap) << (PAGE_SHIFT - 10);
    ksnapshot.cache_size = global_node_page_state(NR_FILE_PAGES) << (PAGE_SHIFT - 10);

    // Copia la información a espacio de usuario
    if (copy_to_user(snapshot, &ksnapshot, sizeof(struct mem_snapshot)))
        return -EFAULT;

    return 0;
}

   ```
## track_syscall_usage_201902278.

   ```
   uname -r
   ```
##  get_io_throttle_201902278.
   ```c
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/cred.h>
#include <linux/errno.h>

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

    // Dar replica de datos al usuario
    if (copy_to_user(stats, &stats_kernel, sizeof(stats_kernel)))
        return -EFAULT;

    return 0;
}
   ```
## Detalles de las syscalls y modulo de Kernel 🕵️
## Pruebas Realizadas 🩻/ Errores

## 🤔 Reflexión Personal y autoevaluación
En el desarollo de este proyecto, enfocado en la expansión del kernel de Linux, fue una experiencia desafiante y enriquecedora. A lo largo del proceso de la realización de este proyecto, no solo adquirí un conocimiento más profundo de cómo funciona internamente un sistema operativo, sino que también puse en práctica habilidades fundametales en programación de sistemas y resolución de problemas técnicos complejos.

### Logros y Aspectos Positivos:

- Consolidación de conocimientos técnicos.
- Enfoque estructurado y metodólogico.
- Generar un mayor interes en el uso de Linux.

### Lecciones Aprendidas:

- Importancia de la planificación.
- Adaptación a herramientas nuevas.
- Trabajo en entornos críticos.