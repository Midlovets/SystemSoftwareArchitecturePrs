#include <stdio.h>
#include <stdlib.h>

int main() {
    size_t huge_size = (size_t)-1; 
    void *ptr = malloc(16); 

    if (!ptr) {
        printf("Initial malloc failed\n");
        return 1;
    }

    printf("Initial allocation successful: %p\n", ptr);

    void *new_ptr = realloc(ptr, huge_size);
    
    if (new_ptr == NULL) {
        printf("realloc failed, original pointer is still valid: %p\n", ptr);
        free(ptr); 
    } else {
        printf("Unexpected success: %p\n", new_ptr);
        free(new_ptr);
    }

    return 0;
}