#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libutil.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    setproctitle("-[kernel]"); 

    const char *homedir = getenv("HOME");
    if (!homedir) {
        fprintf(stderr, "Error: Failed to get home directory\n");
        return 1;
    }

    char config_path[256];
    const char *shell = "/bin/sh"; 
    const char *config_file = ".shrc";
    snprintf(config_path, sizeof(config_path), "%s/%s", homedir, config_file);

    if (access("/usr/local/bin/bash", X_OK) == 0) {
        shell = "/usr/local/bin/bash";
        config_file = ".bashrc";
        snprintf(config_path, sizeof(config_path), "%s/%s", homedir, config_file);
    } else if (access("/bin/bash", X_OK) == 0) {
        shell = "/bin/bash";
        config_file = ".bashrc";
        snprintf(config_path, sizeof(config_path), "%s/%s", homedir, config_file);
    }

    struct stat st;
    if (stat(config_path, &st) != 0) {
        fprintf(stderr, "Error: Failed to access %s: %s\n", config_path, strerror(errno));
        return 1;
    }

    const char *log_file = "/var/log/user_commands.log";
    if (access(log_file, W_OK) != 0) {
        fprintf(stderr, "Error: Cannot write to %s: %s\n", log_file, strerror(errno));
        fprintf(stderr, "Please create %s with appropriate permissions (e.g., sudo touch %s; sudo chown root %s; sudo chmod 600 %s)\n",
                log_file, log_file, log_file, log_file);
        return 1;
    }

    printf("Launching %s with command logging to %s\n", shell, log_file);
    char *shell_argv[] = {(char *)shell, "--rcfile", config_path, NULL};
    execvp(shell, shell_argv);

    fprintf(stderr, "Error: Failed to launch %s: %s\n", shell, strerror(errno));
    return 1;
}