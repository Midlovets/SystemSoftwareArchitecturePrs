#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (fcntl(pipefd[1], F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    char buffer[128 * 1024]; // 128 KB
    memset(buffer, 'A', sizeof(buffer));

    ssize_t count = write(pipefd[1], buffer, sizeof(buffer));
    if (count == -1) {
        perror("write");
    } else {
        printf("Requested to write %lu bytes, actually wrote %zd bytes\n", sizeof(buffer), count);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}