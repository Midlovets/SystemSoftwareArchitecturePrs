#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Attempting to read the contents of /etc/shadow with administrator privileges\n");
    
    if (system("which sudo > /dev/null 2>&1") == 0) {
        printf("Using sudo:\n");
        return system("sudo cat /etc/shadow");
    } 
    // Check if doas is available (an alternative to sudo on FreeBSD)
    else if (system("which doas > /dev/null 2>&1") == 0) {
        printf("Using doas:\n");
        return system("doas cat /etc/shadow");
    }
    // If neither sudo nor doas is available
    else {
        printf("Neither sudo nor doas found on this system.\n");
        printf("Cannot run the command with administrator privileges.\n");
        return 1;
    }
}