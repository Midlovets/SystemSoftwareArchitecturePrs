#include <stdio.h>
#include <stdlib.h>

int main() {
    void *ptr = NULL;
    int i = 0;
    
    while (i < 5) { 
        if (!ptr)
            ptr = malloc(16); 

        printf("Iteration %d, ptr: %p\n", i, ptr);

        free(ptr); 

        i++;
    }

    return 0;
}