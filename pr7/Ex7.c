#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void ask_and_set_permission(const char *filename) {
    char answer[10];
    struct stat st;

    if (stat(filename, &st) != 0) {
        perror("stat");
        return;
    }

    if (!(st.st_mode & S_IXUSR)) {
        return;
    }

    printf("Grant read permission to others for \"%s\"? (y/n/q): ", filename);
    if (fgets(answer, sizeof(answer), stdin) == NULL) {
        perror("fgets");
        exit(1);
    }

    if (answer[0] == 'y' || answer[0] == 'Y') {
        mode_t new_mode = st.st_mode | S_IROTH;
        if (chmod(filename, new_mode) == 0) {
            printf("✓ Read permission granted to others for: %s\n", filename);
        } else {
            perror("chmod");
        }
    } else if (answer[0] == 'q' || answer[0] == 'Q') {
        printf("Exiting program.\n");
        exit(0);
    }
}

int main(void) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        struct stat st;

        if (stat(entry->d_name, &st) != 0)
            continue;

        if (S_ISREG(st.st_mode)) {
            ask_and_set_permission(entry->d_name);
        }
    }

    closedir(dir);
    return 0;
}
