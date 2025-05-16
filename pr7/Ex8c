#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void ask_and_delete(const char *filename, int *delete_all) {
    char answer[10];

    if (*delete_all) {
        if (remove(filename) == 0) {
            printf("✓ Deleted: %s\n", filename);
        } else {
            perror("remove");
        }
        return;
    }

    printf("Delete \"%s\"? (y/n/a/q): ", filename);
    if (fgets(answer, sizeof(answer), stdin) == NULL) {
        perror("fgets");
        exit(1);
    }

    if (answer[0] == 'y' || answer[0] == 'Y') {
        if (remove(filename) == 0) {
            printf("✓ Deleted: %s\n", filename);
        } else {
            perror("remove");
        }
    } else if (answer[0] == 'a' || answer[0] == 'A') {
        if (remove(filename) == 0) {
            printf("✓ Deleted: %s\n", filename);
        } else {
            perror("remove");
        }
        *delete_all = 1;  
    } else if (answer[0] == 'q' || answer[0] == 'Q') {
        printf("Exiting program.\n");
        exit(0);
    }
}

int main(void) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    int delete_all = 0;

    dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &st) == 0 && S_ISREG(st.st_mode)) {
            ask_and_delete(entry->d_name, &delete_all);
        }
    }

    closedir(dir);
    return 0;
}
