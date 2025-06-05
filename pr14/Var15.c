#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define N 4

typedef struct {
    pid_t sender;
    time_t timestamp;
} Event;

volatile sig_atomic_t event_count = 0;
Event events[N];

void handler(int sig, siginfo_t *si, void *context) {
    if (event_count < N) {
        events[event_count].sender = si->si_pid;
        events[event_count].timestamp = time(NULL);
        event_count++;
    }
}

int compare_events(const void *a, const void *b) {
    Event *ea = (Event *)a;
    Event *eb = (Event *)b;
    if (ea->timestamp < eb->timestamp) return -1;
    if (ea->timestamp > eb->timestamp) return 1;
    return 0;
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);

    pid_t pids[N];
    int delays[N] = {2, 5, 1, 3};

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            sleep(delays[i]);
            kill(getppid(), SIGUSR1);
            exit(0);
        } else if (pid > 0) {
            pids[i] = pid;
        } else {
            perror("fork");
            exit(1);
        }
    }

    while (event_count < N) {
        pause();
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }

    qsort(events, N, sizeof(Event), compare_events);

    printf("Timeline of received signals:\n");
    time_t t0 = events[0].timestamp;
    for (int i = 0; i < N; i++) {
        int offset = (int)(events[i].timestamp - t0);
        printf("PID %d |", events[i].sender);
        for (int j = 0; j < offset; j++) printf(" ");
        printf("* (%lds)\n", (long)(events[i].timestamp - t0));
    }

    return 0;
}
