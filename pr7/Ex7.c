#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

void ask_and_grant_read(const char *filename) {
    char answer[10];
    printf("Grant read permission to others for \"%s\"? (y/n/q): ", filename);
    if (!fgets(answer, sizeof(answer), stdin)) {
        perror("fgets");
        return;
    }

    if (answer[0] == 'y' || answer[0] == 'Y') {
        struct stat st;
        if (stat(filename, &st) == 0) {
            mode_t new_mode = st.st_mode | S_IROTH;  
            if (chmod(filename, new_mode) == 0) {
                printf("✓ Read permission granted to others for: %s\n", filename);
            } else {
                perror("chmod");
            }
        } else {
            perror("stat");
        }
    } else if (answer[0] == 'q' || answer[0] == 'Q') {
        printf("Exiting program.\n");
        exit(0);
    }
}

int main() {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        const char *filename = entry->d_name;
        size_t len = strlen(filename);
        if (len > 2 && strcmp(filename + len - 2, ".c") == 0) {
            ask_and_grant_read(filename);
        }
    }

    closedir(dir);
    return 0;
}
