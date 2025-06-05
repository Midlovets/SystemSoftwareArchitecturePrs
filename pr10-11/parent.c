#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Дочірній процес: викликаємо child
        execl("./child", "child", NULL);
        perror("execl");
        return 1;
    } else {
        // Батьківський процес: чекаємо завершення дочірнього
        wait(NULL);
        printf("Дочірній процес завершився.\n");
    }
    return 0;
}
