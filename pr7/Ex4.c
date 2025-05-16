#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Executing command 'whoami':\n");
    if (system("whoami") != 0) {
        perror("Error executing 'whoami'");
        return 1; 
    }

    printf("\nExecuting command 'id':\n");
    if (system("id") != 0) {
        perror("Error executing 'id'");
        return 1; 
    }

    printf("\nProgram completed successfully.\n");

    return 0;
}
