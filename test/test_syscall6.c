#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

// Syscall definida en syscall_64.tbl
#define NUMERO_SYSCALL 552

// Estructura estadisticas_memoria 
struct estadisticas_memoria {
    unsigned long memoria_reservada_mb; // Memoria total reservada en MB
    unsigned long memoria_utilizada_mb; // Memoria total utilizada en MB
    unsigned long memoria_libre_mb;     // Memoria libre en MB
};

int main() {
    struct estadisticas_memoria estadisticas;

    if (syscall(NUMERO_SYSCALL, &estadisticas) == 0) {
        // Imprimir tabla con colores
        printf("\033[1;34m+---------------------+----------------+\033[0m\n");
        printf("\033[1;34m| Categoría           | MB             |\033[0m\n");
        printf("\033[1;34m+---------------------+----------------+\033[0m\n");
        printf("\033[1;32m| Memoria reservada   | %14lu |\033[0m\n", estadisticas.memoria_reservada_mb);
        printf("\033[1;32m| Memoria utilizada   | %14lu |\033[0m\n", estadisticas.memoria_utilizada_mb);
        printf("\033[1;32m| Memoria libre       | %14lu |\033[0m\n", estadisticas.memoria_libre_mb);
        printf("\033[1;34m+---------------------+----------------+\033[0m\n");
    } else {
        perror("\033[1;31mError al ejecutar la syscall\033[0m");
    }

    return 0;
}
