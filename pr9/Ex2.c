#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    if (setuid(0) != 0) {
        perror("setuid");
        return 1;
    }

    execl("/bin/cat", "cat", "/etc/shadow", (char *)NULL);

    perror("execl");
    return 1;
}
