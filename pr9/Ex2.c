#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    const char *cmd = "sudo /bin/cat /etc/master.passwd";

    printf("Perform: %s\n\n", cmd);
    int result = system(cmd);

    if (result != 0) {
        fprintf(stderr, "The team did not go into detail. Make sure you have access via sudo without a password.\n");
        return 1;
    }

    return 0;
}
