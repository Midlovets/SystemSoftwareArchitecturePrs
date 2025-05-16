#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

long get_time_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000L + t.tv_usec / 1000;
}

int main() {
    long start_time = get_time_ms();

    long sum = 0;
    for (long i = 0; i < 100000000L; i++) {
        sum += i;
    }

    long end_time = get_time_ms();
    printf("Час виконання: %ld мс\n", end_time - start_time);
    return 0;
}