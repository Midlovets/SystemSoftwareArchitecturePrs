#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <limits.h>

// Структура для збереження значень та їхньої частоти
typedef struct {
    int val; 
    int cnt; 
} Elem;

// Функція порівняння для сортування за зростанням
int cmp_asc(const void *a, const void *b) {
    Elem *ea = (Elem *)a;
    Elem *eb = (Elem *)b;
    if (ea->val != eb->val) {
        return ea->val - eb->val;
    }
    return eb->cnt - ea->cnt;
}

// Функція порівняння для сортування за спаданням
int cmp_desc(const void *a, const void *b) {
    Elem *ea = (Elem *)a;
    Elem *eb = (Elem *)b;
    if (ea->val != eb->val) {
        return eb->val - ea->val;
    }
    return eb->cnt - ea->cnt;
}

// Реалізація швидкого сортування QuickSort з використанням OpenMP
void qsort_arr(int arr[], int l, int r, int asc) {
    if (l < r) {
        int p = arr[r]; 
        int i = l - 1;
        for (int j = l; j < r; j++) {
            if ((asc && arr[j] < p) || (!asc && arr[j] > p)) {
                i++;
                int t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
        }

        int t = arr[i + 1];
        arr[i + 1] = arr[r];
        arr[r] = t;
        int pi = i + 1;
        
        #pragma omp parallel sections
        {
            #pragma omp section
            qsort_arr(arr, l, pi - 1, asc);
            #pragma omp section
            qsort_arr(arr, pi + 1, r, asc);
        }
    }
}

// Злиття двох відсортованих частин масиву для MergeSort
void merge_arr(int arr[], int l, int m, int r, int asc) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));
    if (L == NULL || R == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    
    #pragma omp parallel for
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    #pragma omp parallel for
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];
    
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (asc ? L[i] <= R[j] : L[i] >= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    
    free(L);
    free(R);
}

// Реалізація сортування MergeSort з OpenMP
void msort_arr(int arr[], int l, int r, int asc) {
    if (l < r) {
        int m = l + (r - l) / 2;
        
        #pragma omp parallel sections
        {
            #pragma omp section
            msort_arr(arr, l, m, asc);
            #pragma omp section
            msort_arr(arr, m + 1, r, asc);
        }
        
        merge_arr(arr, l, m, r, asc);
    }
}

// Функція підрахунку кількості кожного елемента в масиві
void count(int arr[], int n, Elem elems[], int *sz) {
    int i, j;
    *sz = 0;
    
    #pragma omp parallel for private(j)
    for (i = 0; i < n; i++) {
        int f = 0;
        for (j = 0; j < *sz; j++) {
            if (elems[j].val == arr[i]) {
                #pragma omp atomic
                elems[j].cnt++;
                f = 1;
                break;
            }
        }
        if (!f) {
            #pragma omp critical
            {
                elems[*sz].val = arr[i];
                elems[*sz].cnt = 1;
                (*sz)++;
            }
        }
    }
}

// Функція вибору напрямку сортування
int choose_dir() {
    int ch;
    char temp;
    do {
        printf("\n1. Ascending\n2. Descending\nChoose option (1-2): ");
        if (scanf("%d%c", &ch, &temp) != 2 || temp != '\n' || (ch != 1 && ch != 2)) {
            printf("Error! Please enter 1 or 2.\n");
            while (getchar() != '\n'); 
        } else {
            break;
        }
    } while (1);
    return ch;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s -t [qsort|merge] numbers...\n", argv[0]);
        return 1;
    }

    char *sort_type = argv[2];
    if (strcmp(sort_type, "qsort") != 0 && strcmp(sort_type, "merge") != 0) {
        printf("Unknown sort type! Choose 'qsort' or 'merge'.\n");
        return 1;
    }

    int n = argc - 3;
    if (n <= 0) {
        printf("Error: Enter at least one number to sort!\n");
        return 1;
    }

    int arr[n];
    for (int i = 0; i < n; i++) {
        char *end;
        long num = strtol(argv[i + 3], &end, 10);
        if (*end != '\0' || num > INT_MAX || num < INT_MIN) {
            printf("Error: Invalid number '%s'!\n", argv[i + 3]);
            return 1;
        }
        arr[i] = (int)num;
    }

    int dir = choose_dir();
    int asc = (dir == 1);
    
    if (strcmp(sort_type, "qsort") == 0) {
        qsort_arr(arr, 0, n - 1, asc);
    } else {
        msort_arr(arr, 0, n - 1, asc);
    }

    Elem elems[n];
    int sz = 0;
    count(arr, n, elems, &sz);
    
    qsort(elems, sz, sizeof(Elem), asc ? cmp_asc : cmp_desc);

    printf("\nSorted array: ");
    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < elems[i].cnt; j++) {
            printf("%d ", elems[i].val);
        }
    }
    printf("\n");
    return 0;
}
