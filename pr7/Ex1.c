#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define MIN_UID 1000

int main() {
    FILE *fp;
    char buffer[BUFFER_SIZE];

    fp = popen("getent passwd", "r");
    if (fp == NULL) {
        perror("Failed to execute getent passwd");
        exit(EXIT_FAILURE);
    }

    printf("Regular users on the system:\n");

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        char *username, *token;
        int uid;

        char line_copy[BUFFER_SIZE];
        strncpy(line_copy, buffer, sizeof(line_copy));

        username = strtok(line_copy, ":");
        strtok(NULL, ":");
        token = strtok(NULL, ":");

        if (token != NULL) {
            uid = atoi(token);
            if (uid >= MIN_UID && uid != getuid()) {
                printf("User: %s (UID: %d)\n", username, uid);
            }
        }
    }

    pclose(fp);
    return 0;
}
