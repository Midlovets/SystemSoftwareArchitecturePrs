#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define SIGNAL_TO_USE SIGUSR1
#define SIGNALS_TO_SEND 1000
#define TIMER_INTERVAL_NS 1000000 // 1 мс

volatile sig_atomic_t handled = 0;

void handler(int signo) {
    handled++;
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGNAL_TO_USE, &sa, NULL);

    timer_t timerid;
    struct sigevent sev;
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGNAL_TO_USE;

    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create");
        return 1;
    }

    struct itimerspec its;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = TIMER_INTERVAL_NS;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = TIMER_INTERVAL_NS;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime");
        return 1;
    }

    // Ждем, пока не будет отправлено нужное количество сигналов
    while (handled < SIGNALS_TO_SEND) {
        // Ждать немного, чтобы не грузить процессор
        usleep(1000);
    }

    // Останавливаем таймер
    struct itimerspec stop = {0};
    timer_settime(timerid, 0, &stop, NULL);

    printf("Обработано сигналов: %d\n", handled);
    printf("Потеряно сигналов: %d\n", SIGNALS_TO_SEND - handled);

    timer_delete(timerid);
    return 0;
}
