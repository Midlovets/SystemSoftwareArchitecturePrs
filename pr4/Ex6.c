#include <stdio.h>
#include <stdlib.h>

int main() {
    void *ptr = NULL;

    ptr = realloc(NULL, 16);
    printf("realloc(NULL, 16): %p\n", ptr);

    if (ptr) free(ptr);

    ptr = malloc(16);
    printf("Allocated memory: %p\n", ptr);

    void *new_ptr = realloc(ptr, 0);
    printf("realloc(ptr, 0): %p\n", new_ptr);

    if (new_ptr) free(new_ptr);

    return 0;
}