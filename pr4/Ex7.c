#include <stdio.h>
#include <stdlib.h>

struct sbar {
    int data;
};

int main() {
    struct sbar *ptr, *newptr;

    ptr = calloc(1000, sizeof(struct sbar));
    if (!ptr) {
        perror("calloc failed");
        return 1;
    }

    newptr = reallocarray(ptr, 500, sizeof(struct sbar));
    if (!newptr) {
        perror("reallocarray failed");
        free(ptr); 
        return 1;
    }

    printf("Memory reallocated successfully.\n");

    free(newptr);
    return 0;
}