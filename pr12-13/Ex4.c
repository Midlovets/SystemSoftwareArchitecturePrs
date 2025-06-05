#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main() {
    // Уникаємо створення зомбі за допомогою SA_NOCLDWAIT
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;           // ігнорувати SIGCHLD
    sa.sa_flags = SA_NOCLDWAIT;        // не створювати зомбі
    sigaction(SIGCHLD, &sa, NULL);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Дочірній процес
        printf("Child process (PID %d) running...\n", getpid());
        sleep(1);
        printf("Child process exiting...\n");
        _exit(0);
    } else {
        // Батьківський процес
        printf("Parent process (PID %d) continues without wait()\n", getpid());
        sleep(3);  // Чекаємо довше за дитину
        printf("Parent done.\n");
    }

    return 0;
}