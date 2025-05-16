#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *rwho_fp = popen("rwho", "r");
    if (!rwho_fp) {
        perror("popen rwho");
        return 1;
    }

    FILE *more_fp = popen("more", "w");
    if (!more_fp) {
        perror("popen more");
        pclose(rwho_fp);
        return 1;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), rwho_fp)) {
        fputs(buffer, more_fp);
    }

    pclose(rwho_fp);
    pclose(more_fp);

    return 0;
}
