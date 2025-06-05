#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 4

int main() {
    pid_t pids[N];
    int delays[N] = {2, 5, 1, 3};

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            printf("Child %d started, will exit after %d seconds (PID=%d)\n", i, delays[i], getpid());
            sleep(delays[i]);
            printf("Child %d exiting (PID=%d)\n", i, getpid());
            exit(0);
        } else {
            pids[i] = pid;
        }
    }

    int status;
    pid_t finished_pid;
    for (int i = 0; i < N; i++) {
        finished_pid = wait(&status);
        printf("Parent: child with PID %d finished\n", finished_pid);
    }

    printf("Parent: all children finished\n");
    return 0;
}
