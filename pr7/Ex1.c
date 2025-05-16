#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>

// Структура для збереження inodes для виключення циклів
typedef struct {
    ino_t *inodes;
    size_t count;
    size_t capacity;
} InodeList;

// Ініціалізація списку inodes
void init_inode_list(InodeList *list) {
    list->capacity = 16;
    list->inodes = malloc(list->capacity * sizeof(ino_t));
    if (!list->inodes) {
        perror("Не вдалося виділити пам'ять для списку inodes");
        exit(1);
    }
    list->count = 0;
}

// Додавання inode до списку
void add_inode(InodeList *list, ino_t inode) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->inodes = realloc(list->inodes, list->capacity * sizeof(ino_t));
        if (!list->inodes) {
            perror("Не вдалося перевиділити пам'ять для списку inodes");
            exit(1);
        }
    }
    list->inodes[list->count++] = inode;
}

// Перевірка, чи inode вже відвідувався
int is_inode_visited(InodeList *list, ino_t inode) {
    for (size_t i = 0; i < list->count; i++) {
        if (list->inodes[i] == inode) {
            return 1;
        }
    }
    return 0;
}

// Звільнення пам'яті списку inodes
void free_inode_list(InodeList *list) {
    free(list->inodes);
}

// Вивід відступу для ієрархії
void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

// Рекурсивний обхід директорій
void traverse_directory(const char *path, int depth, InodeList *visited) {
    DIR *dir = opendir(path);
    if (!dir) {
        print_indent(depth);
        fprintf(stderr, "Помилка відкриття %s: %s\n", path, strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Пропускаємо . і ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Формуємо повний шлях
        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", path, entry->d_name);

        struct stat statbuf;
        // Використовуємо lstat для збереження символічних посилань
        if (lstat(full_path, &statbuf) == -1) {
            print_indent(depth);
            fprintf(stderr, "Помилка отримання даних %s: %s\n", full_path, strerror(errno));
            continue;
        }

        // Виводимо ім'я з відступом
        print_indent(depth);
        printf("%s", entry->d_name);

        if (S_ISLNK(statbuf.st_mode)) {
            // Для символічних посилань виводимо -> і ціль
            char link_target[PATH_MAX];
            ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0';
                printf(" -> %s (символічне посилання)\n", link_target);
            } else {
                printf(" -> (не вдалося прочитати символічне посилання)\n");
            }
            continue; // Не слідкуємо за символічними посиланнями
        } else if (S_ISDIR(statbuf.st_mode)) {
            printf("/\n");
        } else {
            printf("\n");
        }

        // Для директорій перевіряємо цикли і обходимо рекурсивно
        if (S_ISDIR(statbuf.st_mode)) {
            if (is_inode_visited(visited, statbuf.st_ino)) {
                print_indent(depth + 1);
                printf("(циклічне посилання, пропущено)\n");
                continue;
            }
            add_inode(visited, statbuf.st_ino);
            traverse_directory(full_path, depth + 1, visited);
        }
    }
    closedir(dir);
}

int main() {
    // Отримуємо домашню директорію користувача
    struct passwd *pw = getpwuid(getuid());
    if (!pw) {
        fprintf(stderr, "Помилка отримання даних користувача: %s\n", strerror(errno));
        return 1;
    }

    const char *home_dir = pw->pw_dir;
    printf("Ієрархія директорій для користувача %s (%s):\n", pw->pw_name, home_dir);

    InodeList visited;
    init_inode_list(&visited);

    // Починаємо обхід
    traverse_directory(home_dir, 0, &visited);

    free_inode_list(&visited);
    return 0;
}