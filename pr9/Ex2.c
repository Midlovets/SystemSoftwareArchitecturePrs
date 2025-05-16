#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Команда для виконання від імені адміністратора
    const char *cmd = "sudo cat /etc/shadow";

    printf("Виконую: %s\n\n", cmd);
    int result = system(cmd);

    if (result != 0) {
        fprintf(stderr, "Не вдалося виконати команду. Перевірте, чи маєте доступ через sudo без пароля.\n");
        return 1;
    }

    return 0;
}
