#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/mman.h>

SYSCALL_DEFINE1(tamaloc_201902278, size_t, size)
{
    unsigned long addr;
    size_t aligned_size;
    if (size == 0)
    {
        return -EINVAL; // Invalido
    }
    aligned_size = PAGE_ALIGN(size);
    if (aligned_size == 0)
    {
        return -ENOMEM; // No hay memoria
    }

	addr = vm_mmap(NULL, 0, aligned_size,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, 0);

    if (IS_ERR_VALUE(addr))
    {
        printk(KERN_ERR "ERROR AL MAPEAR MEMORIA VIRTUAL CON TAMALOC :( ya no sale)");
        return addr;
    }
    return addr;
}
