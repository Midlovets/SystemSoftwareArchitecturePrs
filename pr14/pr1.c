#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t ready = 0;

void timer_handler(int signo) {
    ready = 1;
}

int main() {
    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    srand(time(NULL));
    int wait_sec = 2 + rand() % 5; // 2-6 секунд

    timer_t timerid;
    struct sigevent sev = {0};
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR1;
    timer_create(CLOCK_REALTIME, &sev, &timerid);

    struct itimerspec its = {0};
    its.it_value.tv_sec = wait_sec;
    timer_settime(timerid, 0, &its, NULL);

    printf("Гра на реакцію!\nЗачекайте сигналу...\n");

    while (!ready) pause();

    struct timespec start, end;
    printf("Натисніть Enter!\n");
    clock_gettime(CLOCK_MONOTONIC, &start);
    getchar();
    clock_gettime(CLOCK_MONOTONIC, &end);

    double reaction = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Час реакції: %.3f секунд\n", reaction);

    timer_delete(timerid);
    return 0;
}
