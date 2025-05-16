#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

// Функція для виводу рядка прав доступу (тип + rwxrwxrwx)
void print_permissions(mode_t mode) {
    char perms[11] = "----------";

    if (S_ISDIR(mode)) perms[0] = 'd';
    else if (S_ISLNK(mode)) perms[0] = 'l';
    else if (S_ISCHR(mode)) perms[0] = 'c';
    else if (S_ISBLK(mode)) perms[0] = 'b';
    else if (S_ISFIFO(mode)) perms[0] = 'p';
    else if (S_ISSOCK(mode)) perms[0] = 's';

    if (mode & S_IRUSR) perms[1] = 'r';
    if (mode & S_IWUSR) perms[2] = 'w';
    if (mode & S_IXUSR) perms[3] = 'x';

    if (mode & S_IRGRP) perms[4] = 'r';
    if (mode & S_IWGRP) perms[5] = 'w';
    if (mode & S_IXGRP) perms[6] = 'x';

    if (mode & S_IROTH) perms[7] = 'r';
    if (mode & S_IWOTH) perms[8] = 'w';
    if (mode & S_IXOTH) perms[9] = 'x';

    printf("%s ", perms);
}

int main() {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    struct stat st;
    char timebuf[64];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (lstat(entry->d_name, &st) == -1) {
            perror("lstat");
            continue;
        }

        print_permissions(st.st_mode);

        printf("%2lu ", (unsigned long)st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        printf("%-8s ", pw ? pw->pw_name : "unknown");

        struct group *gr = getgrgid(st.st_gid);
        printf("%-8s ", gr ? gr->gr_name : "unknown");

        printf("%8lld ", (long long)st.st_size);

        struct tm *tm = localtime(&st.st_mtime);
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);
        printf("%s ", timebuf);

        printf("%s", entry->d_name);

        if (S_ISLNK(st.st_mode)) {
            char link_target[1024];
            ssize_t len = readlink(entry->d_name, link_target, sizeof(link_target)-1);
            if (len != -1) {
                link_target[len] = '\0';
                printf(" -> %s", link_target);
            }
        }

        printf("\n");
    }

    closedir(dir);
    return 0;
}
