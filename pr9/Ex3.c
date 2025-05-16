#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

int main() {
    uid_t real_uid = getuid(); 
    struct passwd *pw = getpwuid(real_uid);
    if (!pw) {
        fprintf(stderr, "Failed to get user info: %s\n", strerror(errno));
        return 1;
    }
    const char *homedir = pw->pw_dir; 
    const char *src_file = "/tmp/testfile.txt"; 
    char dest_file[256];
    snprintf(dest_file, sizeof(dest_file), "%s/testfile_copy.txt", homedir); 


    printf("Creating file %s as regular user (UID: %d)\n", src_file, real_uid);
    int fd = open(src_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        fprintf(stderr, "Failed to create file %s: %s\n", src_file, strerror(errno));
        return 1;
    }
    const char *content = "This is a test file created by a regular user.\n";
    write(fd, content, strlen(content));
    close(fd);

    if (setuid(0) != 0) {
        fprintf(stderr, "Failed to switch to root: %s\n", strerror(errno));
        return 1;
    }
    printf("Switched to root (UID: %d). Copying file to %s\n", getuid(), dest_file);
    FILE *src = fopen(src_file, "r");
    if (!src) {
        fprintf(stderr, "Failed to open source file %s: %s\n", src_file, strerror(errno));
        return 1;
    }
    FILE *dest = fopen(dest_file, "w");
    if (!dest) {
        fprintf(stderr, "Failed to create file %s: %s\n", dest_file, strerror(errno));
        fclose(src);
        return 1;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }
    fclose(src);
    fclose(dest);

    if (setuid(real_uid) != 0) {
        fprintf(stderr, "Failed to switch back to UID %d: %s\n", real_uid, strerror(errno));
        return 1;
    }
    printf("Switched back to regular user (UID: %d)\n", getuid());

    printf("Attempting to modify file %s as regular user\n", dest_file);
    fd = open(dest_file, O_WRONLY | O_APPEND);
    if (fd < 0) {
        fprintf(stderr, "Failed to open file %s for modification: %s\n", dest_file, strerror(errno));
    } else {
        const char *new_content = "Appended text by regular user.\n";
        write(fd, new_content, strlen(new_content));
        close(fd);
        printf("File %s successfully modified\n", dest_file);
    }

    printf("Attempting to delete file %s using unlink\n", dest_file);
    if (unlink(dest_file) != 0) {
        fprintf(stderr, "Failed to delete file %s: %s\n", dest_file, strerror(errno));
    } else {
        printf("File %s successfully deleted\n", dest_file);
    }

    return 0;
}