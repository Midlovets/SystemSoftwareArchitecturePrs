#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

// Розширений обробник сигналу
void sigint_handler(int sig, siginfo_t *info, void *context) {
    (void)context; // не використовується
    printf("\n[!] Caught SIGINT (signal %d)\n", sig);
    printf("[*] Signal sent by PID: %d\n", info->si_pid);
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_sigaction = sigint_handler;
    sa.sa_flags = SA_SIGINFO | SA_RESTART; // розширений обробник + повтор системних викликів

    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    char buffer[100];
    printf("Enter text (press Ctrl+C while typing):\n");

    // Читання з stdin, яке може бути перерване сигналом
    ssize_t len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    if (len >= 0) {
        buffer[len] = '\0';
        printf("You entered: %s", buffer);
    } else {
        perror("read");
    }

    return 0;
}