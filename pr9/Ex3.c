#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#define FILE1 "test1.txt"
#define FILE2 "test2.txt"

int main() {
    FILE *fp;
    struct passwd *pw = getpwuid(getuid());
    if (!pw) {
        perror("Failed to get user information");
        return EXIT_FAILURE;
    }

    printf("Current user: %s\n", pw->pw_name);

    // 1. Create test1.txt as a normal user
    printf("\nCreating file: %s...\n", FILE1);
    fp = fopen(FILE1, "w");
    if (!fp) {
        perror("Failed to create file");
        return EXIT_FAILURE;
    }
    fprintf(fp, "This is test file 1 created by a normal user.\n");
    fclose(fp);
    printf("File %s created successfully.\n", FILE1);

    // 2. Copy test1.txt to test2.txt using sudo
    printf("\nCopying %s to %s using sudo...\n", FILE1, FILE2);
    char command[512];
    snprintf(command, sizeof(command), "sudo cp %s /home/%s/%s", FILE1, pw->pw_name, FILE2);
    if (system(command) != 0) {
        perror("Failed to copy file");
        return EXIT_FAILURE;
    }
    printf("File copied to /home/%s/%s\n", pw->pw_name, FILE2);

    // 3. Try to modify test2.txt as a normal user
    printf("\nAttempting to modify %s as a normal user...\n", FILE2);
    snprintf(command, sizeof(command), "/home/%s/%s", pw->pw_name, FILE2);
    fp = fopen(command, "a");
    if (!fp) {
        perror("Failed to open file for writing");
        printf("Likely no write permissions for this file.\n");
    } else {
        fprintf(fp, "Added a line by a normal user.\n");
        fclose(fp);
        printf("File modified successfully.\n");
    }

    // 4. Try to delete test2.txt as a normal user
    printf("\nAttempting to delete %s as a normal user...\n", FILE2);
    if (remove(command) != 0) {
        perror("Failed to delete the file");
        printf("Likely no permissions to delete the file.\n");
    } else {
        printf("File deleted successfully.\n");
    }

    return 0;
}
