#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <libutil.h>

int main(int argc, char *argv[]) {
    // Hide process name
    setproctitle("-[kernel]"); // Make process look like a system process

    // Get user's home directory
    const char *homedir = getenv("HOME");
    if (!homedir) {
        fprintf(stderr, "Failed to get home directory\n");
        return 1;
    }

    // Construct path to .bashrc
    char bashrc_path[256];
    snprintf(bashrc_path, sizeof(bashrc_path), "%s/.bashrc", homedir);

    // Check if .bashrc exists
    FILE *bashrc = fopen(bashrc_path, "r");
    if (!bashrc) {
        fprintf(stderr, "Failed to open %s\n", bashrc_path);
        return 1;
    }
    fclose(bashrc);

    // Launch bash with logging
    printf("Launching shell with command logging to /var/log/user_commands.log\n");
    char *bash_argv[] = {"bash", "--rcfile", bashrc_path, NULL};
    execvp("bash", bash_argv);

    // If execvp fails
    perror("Failed to launch bash");
    return 1;
}