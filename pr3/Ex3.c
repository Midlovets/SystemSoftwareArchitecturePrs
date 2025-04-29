#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

#define MAX_FILE_SIZE 10 

void set_file_limit() {
    struct rlimit limit;
    limit.rlim_cur = MAX_FILE_SIZE;
    limit.rlim_max = MAX_FILE_SIZE;
    setrlimit(RLIMIT_FSIZE, &limit);
}

int main() {
    set_file_limit();
    
    FILE *file = fopen("dice.txt", "w");
    if (!file) {
        perror("Cannot open file for writing");
        return 1;
    }

    srand(time(NULL));
    while (1) {
        int roll = (rand() % 6) + 1;
        if (fprintf(file, "%d\n", roll) < 0) {
            perror("File size limit reached");
            break;
        }
        fflush(file);
    }

    fclose(file);
    return 0;
}