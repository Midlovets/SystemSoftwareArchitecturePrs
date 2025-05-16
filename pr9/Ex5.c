#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define TEMP_FILE "tempfile.txt"

int main() {
    FILE *fp;

    printf("Creating temporary file as a regular user...\n");

    fp = fopen(TEMP_FILE, "w");
    if (!fp) {
        perror("Failed to create temporary file");
        return EXIT_FAILURE;
    }
    fprintf(fp, "Temporary file for testing.\n");
    fclose(fp);

    printf("Temporary file created: %s\n", TEMP_FILE);

    printf("Changing owner to root and setting permissions to 600...\n");

    if (system("sudo chown root:root tempfile.txt") != 0) {
        perror("Error executing chown");
    }
    if (system("sudo chmod 600 tempfile.txt") != 0) {
        perror("Error executing chmod");
    }

    printf("Checking current user's access permissions to the file...\n");

    if (access(TEMP_FILE, R_OK) == 0) {
        printf("User can read the file.\n");
    } else {
        printf("User cannot read the file.\n");
    }

    if (access(TEMP_FILE, W_OK) == 0) {
        printf("User can write to the file.\n");
    } else {
        printf("User cannot write to the file.\n");
    }

    return 0;
}
