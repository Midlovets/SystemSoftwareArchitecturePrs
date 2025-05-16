#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_GROUPS 100

// Функція для виведення списку членів групи
void print_group_members(gid_t gid) {
    struct group *grp = getgrgid(gid);
    if (!grp) {
        perror("getgrgid");
        return;
    }
    printf("  Members of group '%s' (GID: %d): ", grp->gr_name, (int)gid);
    char **members = grp->gr_mem;
    if (!members || !members[0]) {
        printf("No members\n");
        return;
    }
    for (int i = 0; members[i] != NULL; i++) {
        printf("%s ", members[i]);
    }
    printf("\n");
}

// Перевірка, чи користувач входить у групу
int is_user_in_group(const char *user, gid_t gid) {
    struct group *grp = getgrgid(gid);
    if (!grp)
        return 0;
    char **members = grp->gr_mem;
    if (!members)
        return 0;
    for (int i = 0; members[i] != NULL; i++) {
        if (strcmp(members[i], user) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return 1;
    }

    const char *username = argv[1];
    struct passwd *pwd = getpwnam(username);
    if (!pwd) {
        fprintf(stderr, "User '%s' not found\n", username);
        return 1;
    }

    gid_t groups[MAX_GROUPS];
    int ngroups = MAX_GROUPS;

    // Отримати всі групи користувача
    if (getgrouplist(username, pwd->pw_gid, groups, &ngroups) == -1) {
        fprintf(stderr, "Too many groups for user '%s'\n", username);
        return 1;
    }

    printf("Groups for user '%s':\n", username);
    for (int i = 0; i < ngroups; i++) {
        struct group *grp = getgrgid(groups[i]);
        if (grp) {
            printf(" - %s (GID: %d)\n", grp->gr_name, (int)groups[i]);
        }
    }

    printf("\nGroup membership details and intersections:\n");
    // Вивести членів кожної групи
    for (int i = 0; i < ngroups; i++) {
        print_group_members(groups[i]);
    }

    // Перевірка перетину груп: спільні користувачі
    printf("\nGroup intersections (groups sharing members):\n");
    for (int i = 0; i < ngroups; i++) {
        for (int j = i + 1; j < ngroups; j++) {
            struct group *g1 = getgrgid(groups[i]);
            struct group *g2 = getgrgid(groups[j]);
            if (!g1 || !g2) continue;

            int found_common = 0;
            for (char **m1 = g1->gr_mem; m1 && *m1; m1++) {
                for (char **m2 = g2->gr_mem; m2 && *m2; m2++) {
                    if (strcmp(*m1, *m2) == 0) {
                        found_common = 1;
                        break;
                    }
                }
                if (found_common) break;
            }
            if (found_common) {
                printf(" - Groups '%s' and '%s' share common members\n", g1->gr_name, g2->gr_name);
            }
        }
    }

    return 0;
}
