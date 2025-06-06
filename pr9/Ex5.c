#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>

int main() {
    uid_t real_uid = getuid(); 
    struct passwd *pw = getpwuid(real_uid);
    if (!pw) {
        fprintf(stderr, "Failed to get user info: %s\n", strerror(errno));
        return 1;
    }
    const char *tmp_file = "/tmp/testfile_perms.txt"; 

    printf("Creating temporary file %s as regular user (UID: %d)\n", tmp_file, real_uid);
    int fd = open(tmp_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        fprintf(stderr, "Failed to create file %s: %s\n", tmp_file, strerror(errno));
        return 1;
    }
    const char *content = "This is a test file for permission checks.\n";
    write(fd, content, strlen(content));
    close(fd);

    if (setuid(0) != 0) {
        fprintf(stderr, "Failed to switch to root: %s\n", strerror(errno));
        return 1;
    }
    printf("Switched to root (UID: %d). Changing ownership and permissions for %s\n", getuid(), tmp_file);

    if (chown(tmp_file, 0, 0) != 0) {
        fprintf(stderr, "Failed to change owner of %s: %s\n", tmp_file, strerror(errno));
        return 1;
    }

    mode_t permissions[] = {0600, 0644, 0400, 0000}; 
    const char *perm_desc[] = {"rw------- (0600)", "rw-r--r-- (0644)", "r-------- (0400)", "--------- (0000)"};
    int num_perms = sizeof(permissions) / sizeof(permissions[0]);

    for (int i = 0; i < num_perms; i++) {
        if (chmod(tmp_file, permissions[i]) != 0) {
            fprintf(stderr, "Failed to change permissions for %s: %s\n", tmp_file, strerror(errno));
            return 1;
        }
        printf("Permissions changed to %s\n", perm_desc[i]);

        if (setuid(real_uid) != 0) {
            fprintf(stderr, "Failed to switch back to UID %d: %s\n", real_uid, strerror(errno));
            return 1;
        }
        printf("Switched back to regular user (UID: %d)\n", getuid());

        printf("Checking access permissions for %s:\n", tmp_file);
        if (access(tmp_file, R_OK) == 0) {
            printf(" - Read: allowed\n");
        } else {
            printf(" - Read: denied (%s)\n", strerror(errno));
        }
        if (access(tmp_file, W_OK) == 0) {
            printf(" - Write: allowed\n");
        } else {
            printf(" - Write: denied (%s)\n", strerror(errno));
        }

        if (setuid(0) != 0) {
            fprintf(stderr, "Failed to switch to root: %s\n", strerror(errno));
            return 1;
        }
    }

    printf("Deleting file %s\n", tmp_file);
    if (unlink(tmp_file) != 0) {
        fprintf(stderr, "Failed to delete file %s: %s\n", tmp_file, strerror(errno));
        return 1;
    }
    printf("File %s successfully deleted\n", tmp_file);

    if (setuid(real_uid) != 0) {
        fprintf(stderr, "Failed to switch back to UID %d: %s\n", real_uid, strerror(errno));
        return 1;
    }

    return 0;
}