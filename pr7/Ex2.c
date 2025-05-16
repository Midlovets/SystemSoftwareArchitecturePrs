#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libprocstat.h>
#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>

// Список стандартних шеллів
const char *standard_shells[] = {
    "/bin/sh",
    "/bin/csh",
    "/bin/tcsh",
    NULL
};

// Перевірка, чи є шлях стандартним шеллом
int is_standard_shell(const char *path) {
    for (int i = 0; standard_shells[i]; i++) {
        if (strcmp(path, standard_shells[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Отримання повного шляху до виконуваного файлу процесу
char *get_process_path(struct kinfo_proc *proc) {
    char *path = malloc(PATH_MAX);
    if (!path) {
        perror("Не вдалося виділити пам'ять для шляху");
        return NULL;
    }

    // Формуємо шлях до /proc/<pid>/file
    char proc_path[PATH_MAX];
    snprintf(proc_path, PATH_MAX, "/proc/%d/file", proc->ki_pid);

    // Отримуємо шлях через readlink
    ssize_t len = readlink(proc_path, path, PATH_MAX - 1);
    if (len == -1) {
        free(path);
        return NULL; // Не вдалося отримати шлях
    }
    path[len] = '\0';
    return path;
}

int main() {
    // Ініціалізація libprocstat
    struct procstat *prstat = procstat_open_sysctl();
    if (!prstat) {
        fprintf(stderr, "Помилка ініціалізації procstat: %s\n", strerror(errno));
        return 1;
    }

    // Отримуємо список усіх процесів
    unsigned int count;
    struct kinfo_proc *procs = procstat_getprocs(prstat, KERN_PROC_ALL, 0, &count);
    if (!procs) {
        fprintf(stderr, "Помилка отримання списку процесів: %s\n", strerror(errno));
        procstat_close(prstat);
        return 1;
    }

    // Виводимо заголовок
    printf("%-8s %-16s %-16s %s\n", "PID", "USER", "PPID", "COMMAND");

    // Проходимо по всіх процесах
    for (unsigned int i = 0; i < count; i++) {
        struct kinfo_proc *proc = &procs[i];

        // Пропускаємо системні процеси (PID 0, ядро тощо)
        if (proc->ki_pid == 0) {
            continue;
        }

        // Отримуємо батьківський процес (PPID)
        pid_t ppid = proc->ki_ppid;
        if (ppid == 0) {
            continue; // Пропускаємо процеси без батьків (наприклад, init)
        }

        // Отримуємо шлях до виконуваного файлу батьківського процесу
        struct kinfo_proc *parent_proc = NULL;
        for (unsigned int j = 0; j < count; j++) {
            if (procs[j].ki_pid == ppid) {
                parent_proc = &procs[j];
                break;
            }
        }

        if (!parent_proc) {
            continue; // Батьківський процес не знайдено
        }

        // Отримуємо шлях до виконуваного файлу батьківського процесу
        char *parent_path = get_process_path(parent_proc);
        if (!parent_path) {
            continue; // Не вдалося отримати шлях
        }

        // Перевіряємо, чи батьківський процес є нестандартним шеллом
        if (!is_standard_shell(parent_path)) {
            // Отримуємо ім'я користувача
            struct passwd *pw = getpwuid(proc->ki_uid);
            const char *username = pw ? pw->pw_name : "unknown";

            // Отримуємо командний рядок процесу
            char **cmd = procstat_getargv(prstat, proc, 0);
            const char *cmdline = cmd && cmd[0] ? cmd[0] : proc->ki_comm;

            // Виводимо інформацію
            printf("%-8d %-16s %-16d %s\n",
                   proc->ki_pid,
                   username,
                   ppid,
                   cmdline);

            procstat_freeargv(prstat);
        }

        free(parent_path);
    }

    // Звільняємо ресурси
    procstat_freeprocs(prstat, procs);
    procstat_close(prstat);
    return 0;
}