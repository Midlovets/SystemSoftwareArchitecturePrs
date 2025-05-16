#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Retrieving contents of /etc/shadow using sudo...\n");

    int status = system("sudo cat /etc/shadow");

    if (status == -1) {
        perror("Error executing command");
        return EXIT_FAILURE;
    } else {
        printf("\nCommand executed with exit code: %d\n", WEXITSTATUS(status));
    }

    return EXIT_SUCCESS;
}