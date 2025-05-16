#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#define FILENAME "file1.txt"
#define COPY_FILENAME "file2.txt"

int main() {
    FILE *fp;
    struct passwd *pw = getpwuid(getuid());
    if (!pw) {
        perror("Failed to get user info");
        return EXIT_FAILURE;
    }

    printf("Current user: %s\n", pw->pw_name);

    printf("\n[+] Creating file %s...\n", FILENAME);
    fp = fopen(FILENAME, "w");
    if (!fp) {
        perror("Failed to create file");
        return EXIT_FAILURE;
    }
    fprintf(fp, "This is a test file created by a regular user.\n");
    fclose(fp);

    printf("[+] File created.\n");

    printf("\n[+] Copying file to home directory using sudo...\n");

    char command[512];
    snprintf(command, sizeof(command), "sudo cp %s /home/%s/%s", FILENAME, pw->pw_name, COPY_FILENAME);
    if (system(command) != 0) {
        perror("Failed to copy file");
        return EXIT_FAILURE;
    }

    printf("[+] File copied to /home/%s/%s\n", pw->pw_name, COPY_FILENAME);

    printf("\n[+] Attempting to modify the copied file as regular user...\n");
    snprintf(command, sizeof(command), "/home/%s/%s", pw->pw_name, COPY_FILENAME);
    fp = fopen(command, "a");
    if (!fp) {
        perror("Failed to open file for writing");
        printf("[-] Likely lack of write permissions.\n");
    } else {
        fprintf(fp, "Appended line by regular user.\n");
        fclose(fp);
        printf("[+] Write successful!\n");
    }

    printf("\n[+] Attempting to delete the copied file as regular user...\n");
    if (remove(command) != 0) {
        perror("Failed to delete file");
        printf("[-] Likely lack of permission to delete the file.\n");
    } else {
        printf("[+] File deleted successfully!\n");
    }

    return 0;
}
