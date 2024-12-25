#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

struct mem_snapshot {
    unsigned long long total_memory;
    unsigned long long free_memory;
    unsigned long long active_memory;
    unsigned long long cache_size;
};

int main() {
    struct mem_snapshot snapshot;
    if (syscall(548, &snapshot) == 0) {
        printf("Total memory: %llu KB\n", snapshot.total_memory);
        printf("Free memory: %llu KB\n", snapshot.free_memory);
        printf("Active memory: %llu KB\n", snapshot.active_memory);
        printf("Cache size: %llu KB\n", snapshot.cache_size);
    } else {
        perror("syscall");
    }
    return 0;
}
