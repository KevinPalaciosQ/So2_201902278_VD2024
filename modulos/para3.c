#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <linux/fs.h>

#ifndef O_DIRECT
#define O_DIRECT 040000
#endif


#define BUFFER_SIZE 512
#define FILE_NAME "direct_io_test_file.txt"

// GLOBAL VARIABLE TO CONTROL THE PROGRAM
static int running = 1;

// SINGNAL HANDLER FUNCTION TO STOP THE PROGRAM
void handle_signal(int signal) {
    running = 0;
}

void perform_direct_io_operations() {
    // THE BUFFER NEEDS TO BE ALIGNED TO 512 BYTES FOR DIRECT I/O
    char *buffer;
    if (posix_memalign((void **)&buffer, 512, BUFFER_SIZE) != 0) {
        perror("Failed to allocate aligned memory");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 'A', BUFFER_SIZE);
    // CREATE AND OPEN A FILE WITH O_DIRECT FLAG TO ENABLE DIRECT I/O
    int fd = open(FILE_NAME, O_CREAT | O_RDWR | O_DIRECT, 0644);
    if (fd < 0) {
        perror("Failed to open file with O_DIRECT");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    while (running) {
        // MORE WRITES THAN READS TO CREATE A WRITE HEAVY WORKLOAD
        for (int i = 0; i < 5; i++) { // 5 escrituras
            if (write(fd, buffer, BUFFER_SIZE) < 0) {
                perror("Write error");
            }
            fsync(fd); // FORCE THE DATA TO BE WRITTEN TO DISK
        }
        // SYNC THE FILE TO MAKE SURE THE DATA IS WRITTEN TO DISK
        fsync(fd);
        // REED THE FILE TO CREATE A READ HEAVY WORKLOAD
        lseek(fd, 0, SEEK_SET);
        if (read(fd, buffer, BUFFER_SIZE) < 0) {
            perror("Read error");
        }
        // PAUSE
        usleep(10000); // 10 ms
    }
    close(fd);
    unlink(FILE_NAME); // DELETE THE FILE AFTER USE
    free(buffer);
}

int main() {
    // REGISTER THE SIGNAL HANDLER FUNCTION (ctrl+c)
    signal(SIGINT, handle_signal);
    printf("PID of this process: %d\n", getpid());
    printf("Generating direct I/O activity. Press Ctrl+C to stop.\n");
    perform_direct_io_operations();
    printf("Direct I/O activity stopped. Exiting.\n");
    return 0;
}