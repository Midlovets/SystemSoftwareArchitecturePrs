#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>  // Додано для wait() і макросів

#define MAX 1024

int main() {
    char line[MAX];

    while (1) {
        printf("dumbsh> ");
        if (!fgets(line, MAX, stdin)) break;

        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0) break;

        pid_t pid = fork();
        if (pid == 0) {
            // Дочірній процес
            execlp(line, line, NULL);
            perror("exec failed");
            exit(1);
        } else if (pid > 0) {
            // Батьківський процес
            int status;
            wait(&status);

            if (WIFEXITED(status)) {
                printf("Exited with code %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Terminated by signal %d\n", WTERMSIG(status));
            }
        } else {
            perror("fork failed");
        }
    }

    return 0;
}