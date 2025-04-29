#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000

int g = 0;             // глобальна змінна
const int c = 42;      // константа

void f() {}            // порожня функція (текстовий сегмент)

long long t_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

int main() {
    int i, s = 0;
    int *h = malloc(sizeof(int));
    *h = 0;
    int tmp = 0;

    long long t1, t2;
    long long ts, th, tg, tc, tf;

    // стек
    t1 = t_ns();
    for (i = 0; i < N; i++) s++;
    t2 = t_ns();
    ts = t2 - t1;

    // купа
    t1 = t_ns();
    for (i = 0; i < N; i++) (*h)++;
    t2 = t_ns();
    th = t2 - t1;

    // глобальна
    t1 = t_ns();
    for (i = 0; i < N; i++) g++;
    t2 = t_ns();
    tg = t2 - t1;

    // константа
    t1 = t_ns();
    for (i = 0; i < N; i++) tmp += c;
    t2 = t_ns();
    tc = t2 - t1;

    // функція
    t1 = t_ns();
    for (i = 0; i < N; i++) f();
    t2 = t_ns();
    tf = t2 - t1;

    free(h);

    printf("\n================ ДОСТУП ДО ПАМ’ЯТІ ================\n");
    printf("%-30s %12s\n", "Сегмент", "Час (нс)");
    printf("---------------------------------------------------\n");
    printf("%-30s %12lld\n", "Стек", ts);
    printf("%-30s %12lld\n", "Купа", th);
    printf("%-30s %12lld\n", "Глобальна", tg);
    printf("%-30s %12lld\n", "Константа", tc);
    printf("%-30s %12lld\n", "Функція", tf);
    printf("===================================================\n");

    return 0;
}
