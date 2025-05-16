#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
    struct timeval start, end;
    long elapsed;

    gettimeofday(&start, NULL);

    for (volatile int i = 0; i < 1000000; i++);

    gettimeofday(&end, NULL);

    elapsed = (end.tv_sec - start.tv_sec) * 1000;
    elapsed += (end.tv_usec - start.tv_usec) / 1000;

    printf("Execution time: %ld ms\n", elapsed);

    return 0;
}
