#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_permissions(mode_t mode) {
    printf( (S_ISDIR(mode)) ? "d" : "-");
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}

void check_file(const char *path, const char *file) {
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, file);

    struct stat st;
    if (stat(fullpath, &st) == -1) {
        perror("stat error");
        return;
    }

    print_permissions(st.st_mode);

    printf(" %2lu", st.st_nlink);

    struct passwd *pw = getpwuid(st.st_uid);
    struct group  *gr = getgrgid(st.st_gid);
    printf(" %-8s %-8s", pw ? pw->pw_name : "unknown", gr ? gr->gr_name : "unknown");

    printf(" %8ld", st.st_size);

    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
    printf(" %s", timebuf);

    printf(" %s\n", file);

    printf("    [Access check for user]:\n");
    printf("      Read:      %s\n", (access(fullpath, R_OK) == 0) ? "ALLOWED" : "DENIED");
    printf("      Write:     %s\n", (access(fullpath, W_OK) == 0) ? "ALLOWED" : "DENIED");
    printf("      Execute:   %s\n\n", (access(fullpath, X_OK) == 0) ? "ALLOWED" : "DENIED");
}

void check_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;

    printf("\n>>> Scanning directory: %s\n", path);

    dir = opendir(path);
    if (!dir) {
        perror("Failed to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        check_file(path, entry->d_name);
    }

    closedir(dir);
}

int main() {
    check_directory(getenv("HOME"));   // Home directory
    check_directory("/usr/bin");       // System binaries
    check_directory("/etc");           // Configuration files

    return 0;
}
