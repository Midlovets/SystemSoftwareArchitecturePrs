#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Отримання вмісту /etc/shadow через sudo...\n");

    int status = system("sudo cat /etc/shadow");

    if (status == -1) {
        perror("Помилка виконання команди");
        return EXIT_FAILURE;
    } else {
        printf("\nКоманда виконана з кодом: %d\n", WEXITSTATUS(status));
    }

    return EXIT_SUCCESS;
}