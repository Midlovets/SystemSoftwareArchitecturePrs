#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

int main() {
    uid_t real_uid = getuid(); // Отримати реальний UID (звичайний користувач)
    struct passwd *pw = getpwuid(real_uid);
    if (!pw) {
        fprintf(stderr, "Не вдалося отримати інформацію про користувача: %s\n", strerror(errno));
        return 1;
    }
    const char *homedir = pw->pw_dir; // Домашня директорія користувача
    const char *src_file = "/tmp/testfile.txt"; // Шлях до вихідного файлу
    char dest_file[256];
    snprintf(dest_file, sizeof(dest_file), "%s/testfile_copy.txt", homedir); // Шлях до копії файлу

    // Крок 1: Створення файлу від імені звичайного користувача
    printf("Створюємо файл %s від імені звичайного користувача (UID: %d)\n", src_file, real_uid);
    int fd = open(src_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        fprintf(stderr, "Не вдалося створити файл %s: %s\n", src_file, strerror(errno));
        return 1;
    }
    const char *content = "Це тестовий файл, створений звичайним користувачем.\n";
    write(fd, content, strlen(content));
    close(fd);

    // Крок 2: Копіювання файлу від імені root
    if (setuid(0) != 0) {
        fprintf(stderr, "Не вдалося переключитися на root: %s\n", strerror(errno));
        return 1;
    }
    printf("Переключено на root (UID: %d). Копіюємо файл у %s\n", getuid(), dest_file);
    FILE *src = fopen(src_file, "r");
    if (!src) {
        fprintf(stderr, "Не вдалося відкрити вихідний файл %s: %s\n", src_file, strerror(errno));
        return 1;
    }
    FILE *dest = fopen(dest_file, "w");
    if (!dest) {
        fprintf(stderr, "Не вдалося створити файл %s: %s\n", dest_file, strerror(errno));
        fclose(src);
        return 1;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }
    fclose(src);
    fclose(dest);

    // Повернення до звичайного користувача
    if (setuid(real_uid) != 0) {
        fprintf(stderr, "Не вдалося повернутися до UID %d: %s\n", real_uid, strerror(errno));
        return 1;
    }
    printf("Повернено до звичайного користувача (UID: %d)\n", getuid());

    // Крок 3: Спроба редагування файлу
    printf("Намагаємося редагувати файл %s від імені звичайного користувача\n", dest_file);
    fd = open(dest_file, O_WRONLY | O_APPEND);
    if (fd < 0) {
        fprintf(stderr, "Не вдалося відкрити файл %s для редагування: %s\n", dest_file, strerror(errno));
    } else {
        const char *new_content = "Додано текст від звичайного користувача.\n";
        write(fd, new_content, strlen(new_content));
        close(fd);
        printf("Файл %s успішно відредаговано\n", dest_file);
    }

    // Крок 4: Спроба видалення файлу
    printf("Намагаємося видалити файл %s за допомогою unlink\n", dest_file);
    if (unlink(dest_file) != 0) {
        fprintf(stderr, "Не вдалося видалити файл %s: %s\n", dest_file, strerror(errno));
    } else {
        printf("Файл %s успішно видалено\n", dest_file);
    }

    return 0;
}