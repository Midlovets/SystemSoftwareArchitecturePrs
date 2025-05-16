#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

char* read_file(const char* filename, size_t* size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening %s: %s\n", filename, strerror(errno));
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(*size);
    if (!buffer) {
        fclose(file);
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    if (fread(buffer, 1, *size, file) != *size) {
        free(buffer);
        fclose(file);
        fprintf(stderr, "Error reading %s\n", filename);
        return NULL;
    }

    fclose(file);
    return buffer;
}

int write_file(const char* filename, const char* buffer, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error opening %s: %s\n", filename, strerror(errno));
        return -1;
    }

    if (fwrite(buffer, 1, size, file) != size) {
        fclose(file);
        fprintf(stderr, "Error writing to %s\n", filename);
        return -1;
    }

    fclose(file);
    return 0;
}

int swap_files(const char* file1, const char* file2) {
    size_t size1, size2;
    char* buffer1 = read_file(file1, &size1);
    if (!buffer1) return -1;

    char* buffer2 = read_file(file2, &size2);
    if (!buffer2) {
        free(buffer1);
        return -1;
    }

    int fd1 = open(file1, O_WRONLY);
    if (fd1 == -1) {
        fprintf(stderr, "Error opening %s for locking: %s\n", file1, strerror(errno));
        free(buffer1);
        free(buffer2);
        return -1;
    }

    int fd2 = open(file2, O_WRONLY);
    if (fd2 == -1) {
        fprintf(stderr, "Error opening %s for locking: %s\n", file2, strerror(errno));
        close(fd1);
        free(buffer1);
        free(buffer2);
        return -1;
    }

    if (flock(fd1, LOCK_EX) == -1 || flock(fd2, LOCK_EX) == -1) {
        fprintf(stderr, "Error locking files: %s\n", strerror(errno));
        close(fd1);
        close(fd2);
        free(buffer1);
        free(buffer2);
        return -1;
    }

    if (write_file(file1, buffer2, size2) != 0 || write_file(file2, buffer1, size1) != 0) {
        flock(fd1, LOCK_UN);
        flock(fd2, LOCK_UN);
        close(fd1);
        close(fd2);
        free(buffer1);
        free(buffer2);
        return -1;
    }

    flock(fd1, LOCK_UN);
    flock(fd2, LOCK_UN);
    close(fd1);
    close(fd2);
    free(buffer1);
    free(buffer2);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* file1 = argv[1];
    const char* file2 = argv[2];

    if (access(file1, F_OK) != 0) {
        fprintf(stderr, "Error: File %s does not exist\n", file1);
        exit(EXIT_FAILURE);
    }
    if (access(file2, F_OK) != 0) {
        fprintf(stderr, "Error: File %s does not exist\n", file2);
        exit(EXIT_FAILURE);
    }

    if (swap_files(file1, file2) != 0) {
        fprintf(stderr, "Error swapping files: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Files %s and %s swapped successfully\n", file1, file2);
    return EXIT_SUCCESS;
}