#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Check if the program is running as root
    if (geteuid() == 0) {
        printf("Program is running with administrator privileges.\n");
    } else {
        printf("Program is running as a normal user. Attempting to gain elevated access via sudo...\n");
    }

    // Execute the command using sudo
    int ret = system("sudo cat /etc/shadow");

    if (ret == -1) {
        perror("system");
        return 1;
    } else if (WEXITSTATUS(ret) != 0) {
        fprintf(stderr, "Command failed with exit code: %d\n", WEXITSTATUS(ret));
        return 1;
    }

    return 0;
}
