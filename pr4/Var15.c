#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#define ALLOCATIONS 1000
#define BLOCK_SIZE (1024 * 1024) 

void print_usage(const char *stage) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("[%s] Пік використання памʼяті: %.2f МіБ\n", stage, usage.ru_maxrss / 1024.0);
}

int main() {
    printf("=== malloc тест для порівняння glibc vs musl ===\n");

    print_usage("До malloc");

    void *blocks[ALLOCATIONS];
    for (int i = 0; i < ALLOCATIONS; i++) {
        blocks[i] = malloc(BLOCK_SIZE);
        if (!blocks[i]) {
            fprintf(stderr, "malloc не вдалося на кроці %d\n", i);
            break;
        }
        memset(blocks[i], 0, BLOCK_SIZE);
    }

    print_usage("Після malloc");

    for (int i = 0; i < ALLOCATIONS; i++) {
        free(blocks[i]);
    }

    print_usage("Після free");

    return 0;
}
