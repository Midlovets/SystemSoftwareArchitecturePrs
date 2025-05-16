#include <stdio.h> 
#include <stdlib.h>

#define MAX_LINE 1024
#define LINES_PER_PAGE 20

void show_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror(filename);
        return;
    }

    char line[MAX_LINE];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        fputs(line, stdout);
        count++;

        if (count >= LINES_PER_PAGE) {
            printf("---- Press Enter to continue ----");
            while (getchar() != '\n'); 
            count = 0;
        }
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2 ...]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        printf("\n\n--- File: %s ---\n", argv[i]);
        show_file(argv[i]);
    }

    return 0;
}
