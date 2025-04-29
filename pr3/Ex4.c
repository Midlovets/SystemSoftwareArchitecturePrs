#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

void set_cpu_limit() {
    struct rlimit limit;
    limit.rlim_cur = 2;  
    limit.rlim_max = 2;
    setrlimit(RLIMIT_CPU, &limit);
}

void generate_lottery_numbers(int count, int max_value) {
    int numbers[count];
    for (int i = 0; i < count; i++) {
        int num;
        do {
            num = (rand() % max_value) + 1;
        } while (numbers[num]); 

        numbers[num] = 1;
        printf("%d ", num);
    }
    printf("\n");
}

int main() {
    set_cpu_limit();
    srand(time(NULL));

    printf("Lottery 7/49: ");
    generate_lottery_numbers(7, 49);

    printf("Lottery 6/36: ");
    generate_lottery_numbers(6, 36);

    return 0;
}