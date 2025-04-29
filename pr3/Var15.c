#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>

#define BUFFER_SIZE (1024 * 1024)  

int main() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_FSIZE, &rl) == 0) {
        printf("Ліміт розміру файлу: ");
        if (rl.rlim_cur == RLIM_INFINITY) {
            printf("немає обмеження\n");
        } else {
            printf("%lu байт (%.2f МіБ)\n", rl.rlim_cur, rl.rlim_cur / 1024.0 / 1024.0);
        }
    } else {
        perror("Не вдалося отримати обмеження RLIMIT_FSIZE");
    }

    FILE *file = fopen("test_output.bin", "wb");
    if (!file) {
        perror("Не вдалося відкрити файл");
        return 1;
    }

    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Помилка виділення памʼяті");
        fclose(file);
        return 2;
    }

    memset(buffer, 'A', BUFFER_SIZE);  

    size_t total_written = 0;
    while (1) {
        size_t written = fwrite(buffer, 1, BUFFER_SIZE, file);
        if (written < BUFFER_SIZE) {
            if (ferror(file)) {
                perror("Помилка запису (можливо, досягнуто ліміту ulimit -f)");
            } else {
                printf("Досягнуто кінця файлу або інша причина зупинки\n");
            }
            break;
        }
        total_written += written;
        printf("Записано загалом: %lu байт (%.2f МіБ)\n", total_written, total_written / 1024.0 / 1024.0);
    }

    free(buffer);
    fclose(file);

    printf("\n Підсумок: загалом записано: %.2f МіБ\n", total_written / 1024.0 / 1024.0);
    return 0;
}
