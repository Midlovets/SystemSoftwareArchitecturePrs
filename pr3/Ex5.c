#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>

#define BUFFER_SIZE 1024
#define MAX_FILE_SIZE 10  

void set_file_limit() {
    struct rlimit limit;
    limit.rlim_cur = MAX_FILE_SIZE;
    limit.rlim_max = MAX_FILE_SIZE;
    setrlimit(RLIMIT_FSIZE, &limit);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Program need two arguments\n");
        return 1;
    }

    set_file_limit();

    FILE *src = fopen(argv[1], "r");
    if (!src) {
        fprintf(stderr, "Cannot open file %s for reading\n", argv[1]);
        return 1;
    }

    FILE *dest = fopen(argv[2], "w");
    if (!dest) {
        fprintf(stderr, "Cannot open file %s for writing\n", argv[2]);
        fclose(src);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
        if (fwrite(buffer, 1, bytes, dest) < bytes) {
            perror("File size limit reached");
            break;
        }
    }

    fclose(src);
    fclose(dest);
    return 0;
}