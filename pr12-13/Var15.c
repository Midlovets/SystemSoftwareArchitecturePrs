#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_EVENTS 20
#define MSG_LEN 128

typedef struct {
    char msg[MSG_LEN];
    time_t next_time;
    int interval;
    int active;
} Event;

void add_event(Event *events, int *count) {
    if (*count >= MAX_EVENTS) {
        printf("Maximum number of events reached.\n");
        return;
    }
    char buf[MSG_LEN];
    int delay, interval;
    printf("Enter reminder message: ");
    fgets(buf, MSG_LEN, stdin);
    buf[strcspn(buf, "\n")] = 0;
    printf("In how many seconds should the reminder trigger? ");
    scanf("%d", &delay);
    printf("Repeat interval (0 - one-time): ");
    scanf("%d", &interval);
    getchar();

    Event *e = &events[*count];
    strncpy(e->msg, buf, MSG_LEN);
    e->next_time = time(NULL) + delay;
    e->interval = interval;
    e->active = 1;
    (*count)++;
}

void print_menu() {
    printf("\n1. Add event\n2. Exit\nChoose an option: ");
}

int main() {
    Event events[MAX_EVENTS];
    int event_count = 0;
    int running = 1;

    while (running) {
        print_menu();
        int choice;
        scanf("%d", &choice);
        getchar();
        if (choice == 1) {
            add_event(events, &event_count);
        } else if (choice == 2) {
            running = 0;
        } else {
            printf("Invalid choice.\n");
        }

        time_t now = time(NULL);
        for (int i = 0; i < event_count; i++) {
            if (events[i].active && events[i].next_time <= now) {
                printf("\n*** REMINDER: %s ***\n", events[i].msg);
                if (events[i].interval > 0) {
                    events[i].next_time = now + events[i].interval;
                } else {
                    events[i].active = 0;
                }
            }
        }
        sleep(1);
    }
    printf("Done.\n");
    return 0;
}
