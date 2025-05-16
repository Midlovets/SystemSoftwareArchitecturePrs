#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd;
    unsigned char buffer[4];
    unsigned char data[] = {4, 5, 2, 2, 3, 3, 7, 9, 1, 5};

    fd = open("testfile.bin", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    write(fd, data, sizeof(data));

    lseek(fd, 3, SEEK_SET);

    read(fd, buffer, 4);

    printf("Read bytes: ");
    for (int i = 0; i < 4; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");

    close(fd);
    return 0;
}
