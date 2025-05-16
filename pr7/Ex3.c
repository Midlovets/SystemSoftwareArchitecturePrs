#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Використання: %s <слово> <файл>\n", argv[0]);
        return 1;
    }

    const char *word = argv[1];
    const char *filename = argv[2];
    char line[MAX_LINE];

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Не вдалося відкрити файл");
        return 1;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, word)) {
            printf("%s", line);
        }
    }

    fclose(fp);
    return 0;
}