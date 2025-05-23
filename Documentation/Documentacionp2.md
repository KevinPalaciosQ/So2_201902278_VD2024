# _Manual Técnico ☝️🤓_

## 📚 Contenido

- 🎯 Objetivos
- 📍 Tecnologías
- 🛠️ Herramientas
- 💼 Paquetes
- 🛡️ Guía Instalación
- Informe Técnico ☢️
- Detalles de las syscalls l 🕵️
- Pruebas Realizadas 🩻
- 🤔 Reflexión Personal y autoevaluación
## 🎯 Objetivos

## General

Modificar el Kernel de linux en la versión 6.8.49 para poder desarollar e integrar un sistema eficiente para llevar a cabo la gestión de memoría en el kernel de Linux que permita la inicialización de memoria en cero mediante técnicas de asignación usando lazy-zeroing, optimizando el uso de recurso físicos y mejorando el manejo de las estádisticas en tiempo real

## Específicos

- Implementar una syscall para recolección de memoria.
- Diseñar un algoritmo de asignación basado en lazy-zeroing llamado tamaloc.

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
- tamaloc
- memory_usage_table.
- memory_usage_general.
## syscall_64.tbl
En la syscall.tbl se asigna el número de syscall a llamar las syscall del proyecto.
## Informe Técnico

 ## tamalloc
![tamaloc](../images/tamaloc.png)
En la parte de entrada de usuario recibe dos argumentos pid_pit y este es el identificador de procesos, usa el struct io_statistics __user *stats: que es un puntero para copiar los datos al usuario.
Tambien utiliza get_pid_task y find_vpid para obtener el descriptor del proceso general del pid  si el proceso no existe con -ESRCH retorna no encontrado.
## memory_usage_table.
![s1](../images/syscall2.png)
Esta syscall como la anterior recibe 3 parámetros, al momento de obtener pid === 0 recorre un for_each_process para mostrar una tabla, obtiene la memoria reservada, comprometida y el OOM Score, si el pid!=0 este solo muestra los datos del PID deseado.
##  memory_usage_general.
![s2](../images/syscall3.png)
Recibe solo un argumento, que es el puntero donde se copiaran las estadisticas, obtiene la información del sistema y calcula la memoria general, recorre todos los procesos con un for_each_process, obtiene la estructura de cada proceso y convierte los valores en mb
## Detalles de las syscalls 🕵️

- Tamaloc: esta syscall obtiene estadísticas de operaciones de entrada/salida de un proceso especifrico identificado por su PID y devuelve el espacio de usuario mediante la estructura de io_statistics. 

- memory_usage_table: la syscall proporciona estadísticas detalladas sobre la memoria reservada(virtual), la memoria comprometida (física) y el OOM Score para procesos individuales o todos los procesos en el sistema. Esto es útil para monitoreo de recursos, análisis de rendimiento y diagnóstico de problemas relacionados con la gestión de memoria.

- memory_usage_general: se encarga de recolectar y proveer estadísticas detalladas de la memoria en general del sistema.
## Pruebas Realizadas 🩻/ Errores

Durante las pruebas de syscalls todo transcurrió con normalidad. 

![e1](../images/e1.jpeg)
El primer Error Ourrió debido a una mala referencia de una syscall, no estaba definida en la tabla de syscalls.
![e2](../images/e2.jpeg)
El segundo error ocurrió al no estar trabajando en el Kernel del proyecto, debido a una actualización de sistema.
![e3](../images/e3.jpeg)
este error ocurrió debido a trabajar mal un mmap.
## 🤔 Reflexión Personal y autoevaluación
Durante el desarollo del Proyecto 2, uno de los mayores desafíos fue el calculo correcto del OOM Score de cada proceso. Debido a que este puntaje, utlizado por el kernel de linux para priorizar qué procesos debe eliminar en situaciones de memoria insuficiente, requiere un análisis detallados de múltiples factores, incluidos el consumo de meoria, las prioridades del proceso y las políticas del sistema.


Finalmente, la complejidad inherente de modificar el Kernel, junto con la limitación de tiempo debido a las festividades del mes de diciembre, dificultó la exploración de alternativas que pudieron haber simplificado el cálculo, y pese a encontrar alternativas estas no funcionaron obteniendo los datos esperados.
### Logros y Aspectos Positivos:

- Consolidación de conocimientos técnicos.
- Enfoque estructurado y metodólogico.
- Generar un mayor interes en el uso de Linux.

### Lecciones Aprendidas:

- Importancia de la planificación.
- Documentación y Depuración.
- Manejo de Complejidad.
- Comunicación entre Kernel y espacio de Usuario.