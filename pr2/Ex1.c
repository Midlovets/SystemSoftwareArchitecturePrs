#include <stdio.h>
#include <time.h>
#include <limits.h>

int main() {
    time_t max_time;

    if ((time_t)-1 > 0) {
        max_time = (time_t)~0;
    } else {
        max_time = (time_t)((1ULL << (sizeof(time_t) * 8 - 1)) - 1);
    }
    printf("Maximum value of time_t: %ld\n", (long)max_time);

    struct tm *tm_time = gmtime(&max_time);
    if (tm_time) {
        printf("Date and time: %s\n", ctime(&max_time));
    } else {
        perror("Time conversion error");
    }
    return 0;
}