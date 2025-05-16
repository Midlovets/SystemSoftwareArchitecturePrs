#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_ENTRIES 1024

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int main() {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return 1;
    }
    struct dirent *entry;
    char *dirs[MAX_ENTRIES];
    int count = 0;
    struct stat st;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (stat(entry->d_name, &st) == 0 && S_ISDIR(st.st_mode)) {
            dirs[count] = strdup(entry->d_name);
            count++;
            if (count >= MAX_ENTRIES) break;
        }
    }

    closedir(dir);

    qsort(dirs, count, sizeof(char *), compare);

    for (int i = 0; i < count; i++) {
        printf("%s/\n", dirs[i]);
        free(dirs[i]);
    }

    return 0;
}