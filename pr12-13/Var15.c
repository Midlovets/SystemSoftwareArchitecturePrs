#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#define MAX_EVENTS 50

typedef struct {
    int signo;
    time_t received;
    pid_t sender;
} Event;

Event events[MAX_EVENTS];
int event_count = 0;

void handle_signal(int signo, siginfo_t *info, void *context) {
    if (event_count >= MAX_EVENTS) return;
    events[event_count].signo = signo;
    events[event_count].received = time(NULL);
    events[event_count].sender = info->si_pid;
    event_count++;
    printf("Received signal %d from PID %d at %ld\n", signo, info->si_pid, events[event_count-1].received);
}

int cmp_events(const void *a, const void *b) {
    Event *ea = (Event *)a, *eb = (Event *)b;
    if (ea->received < eb->received) return -1;
    if (ea->received > eb->received) return 1;
    return 0;
}

void print_timeline() {
    printf("\n--- Timeline ---\n");
    for (int i = 0; i < event_count; i++) {
        struct tm *tm_info = localtime(&events[i].received);
        char buf[32];
        strftime(buf, 32, "%H:%M:%S", tm_info);
        printf("[%s] PID:%d SIGNAL:%s\n", buf, events[i].sender,
            events[i].signo == SIGUSR1 ? "USR1" : (events[i].signo == SIGUSR2 ? "USR2" : "OTHER"));
    }
    printf("\nASCII timeline:\n");
    for (int i = 0; i < event_count; i++) {
        int pos = i * 4;
        for (int j = 0; j < pos; j++) printf(" ");
        printf("*");
        printf("(%s)\n", events[i].signo == SIGUSR1 ? "USR1" : "USR2");
    }
}

volatile sig_atomic_t stop = 0;

void handle_sigint(int signo) {
    stop = 1;
}

int main() {
    struct sigaction sa;
    sa.sa_sigaction = handle_signal;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    struct sigaction sa_int;
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGINT, &sa_int, NULL);

    printf("Controller PID: %d\n", getpid());
    printf("Send SIGUSR1 or SIGUSR2 from other processes to this PID.\n");
    printf("Press Ctrl+C to stop and see the timeline.\n");

    while (!stop) {
        pause();
    }

    qsort(events, event_count, sizeof(Event), cmp_events);
    print_timeline();
    return 0;
}
