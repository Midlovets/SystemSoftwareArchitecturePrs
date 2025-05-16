#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define USER_FILE "user_file.txt"
#define COPY_FILE "/home/username/user_file_copy.txt"  // Змініть на реальний домашній каталог користувача

int main() {
    // 1. Створюємо файл від імені звичайного користувача
    FILE *f = fopen(USER_FILE, "w");
    if (!f) {
        perror("fopen");
        return 1;
    }
    fprintf(f, "Це оригінальний файл.\n");
    fclose(f);

    // 2. Піднімаємо права до root (припустимо, SUID встановлений)
    if (setuid(0) != 0) {
        perror("setuid root");
        return 1;
    }
    printf("Дія від імені root\n");

    // 3. Копіюємо файл у домашній каталог користувача
    FILE *src = fopen(USER_FILE, "r");
    if (!src) {
        perror("fopen src");
        return 1;
    }
    FILE *dst = fopen(COPY_FILE, "w");
    if (!dst) {
        perror("fopen dst");
        fclose(src);
        return 1;
    }

    char buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
        fwrite(buf, 1, n, dst);
    }
    fclose(src);
    fclose(dst);

    // 4. Повертаємося до звичайного користувача
    uid_t user_uid = getuid();
    if (setuid(user_uid) != 0) {
        perror("setuid user");
        return 1;
    }
    printf("Дія від імені користувача\n");

    // 5. Пробуємо змінити скопійований файл
    FILE *edit = fopen(COPY_FILE, "a");  // Відкриваємо в режимі дозапису
    if (!edit) {
        perror("fopen edit");
    } else {
        fprintf(edit, "Додано користувачем.\n");
        fclose(edit);
        printf("Файл змінено успішно.\n");
    }

    // 6. Пробуємо видалити файл
    if (remove(COPY_FILE) == 0) {
        printf("Файл видалено успішно.\n");
    } else {
        perror("remove");
    }

    return 0;
}
