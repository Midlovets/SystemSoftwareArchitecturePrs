#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#define FILENAME "testfile.txt"
#define COPY_FILENAME "copied_testfile.txt"

int main() {
    FILE *fp;
    struct passwd *pw = getpwuid(getuid());
    if (!pw) {
        perror("Failed to get user information");
        return EXIT_FAILURE;
    }

    printf("Current user: %s\n", pw->pw_name);

    // 1. Create a file as a normal user
    printf("\n[+] Creating file %s...\n", FILENAME);
    fp = fopen(FILENAME, "w");
    if (!fp) {
        perror("Failed to create file");
        return EXIT_FAILURE;
    }
    fprintf(fp, "This is a test file created by a normal user.\n");
    fclose(fp);

    printf("[+] File created.\n");

    // 2. Copy file as root (sudo)
    printf("\n[+] Copying file to home directory using sudo...\n");

    char command[512];
    snprintf(command, sizeof(command), "sudo cp %s /home/%s/%s", FILENAME, pw->pw_name, COPY_FILENAME);
    if (system(command) != 0) {
        perror("Failed to copy file");
        return EXIT_FAILURE;
    }

    printf("[+] File copied to /home/%s/%s\n", pw->pw_name, COPY_FILENAME);

    // 3. Try to modify the file as a normal user
    printf("\n[+] Trying to modify the file as a normal user...\n");
    snprintf(command, sizeof(command), "/home/%s/%s", pw->pw_name, COPY_FILENAME);
    fp = fopen(command, "a");
    if (!fp) {
        perror("Failed to open file for writing");
        printf("[-] Probably no write permissions on the file.\n");
    } else {
        fprintf(fp, "Added a line by a normal user.\n");
        fclose(fp);
        printf("[+] Write successful!\n");
    }

    // 4. Try to delete the file as a normal user
    printf("\n[+] Trying to delete the file as a normal user...\n");
    if (remove(command) != 0) {
        perror("Failed to delete the file");
        printf("[-] Probably no permissions to delete the file.\n");
    } else {
        printf("[+] File deleted successfully!\n");
    }

    return 0;
}
