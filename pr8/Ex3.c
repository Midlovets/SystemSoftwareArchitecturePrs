#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

double measure_sort_time(int *array, size_t size, int runs) {
    int *copy = malloc(size * sizeof(int));
    double total_time = 0.0;

    for (int i = 0; i < runs; i++) {
        memcpy(copy, array, size * sizeof(int));
        clock_t start = clock();
        qsort(copy, size, sizeof(int), compare_ints);
        clock_t end = clock();
        total_time += (double)(end - start) / CLOCKS_PER_SEC;
    }

    free(copy);
    return total_time / runs;
}

void generate_sorted(int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        array[i] = i;
    }
}

void generate_reversed(int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        array[i] = size - i;
    }
}

void generate_random(int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        array[i] = rand();
    }
}

void generate_constant(int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        array[i] = 42;
    }
}

void generate_almost_sorted(int *array, size_t size) {
    generate_sorted(array, size);
    for (size_t i = 0; i < size / 10; i++) {
        size_t idx1 = rand() % size;
        size_t idx2 = rand() % size;
        int temp = array[idx1];
        array[idx1] = array[idx2];
        array[idx2] = temp;
    }
}

void generate_killer(int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        array[i] = i;
    }
    for (size_t i = 1; i < size; i += 2) {
        array[i] = i - 1;
        array[i - 1] = i;
    }
}

int is_sorted(int *array, size_t size) {
    for (size_t i = 1; i < size; i++) {
        if (array[i - 1] > array[i]) return 0;
    }
    return 1;
}

void run_test(int *array, size_t size, const char *test_name) {
    int *copy = malloc(size * sizeof(int));
    memcpy(copy, array, size * sizeof(int));
    qsort(copy, size, sizeof(int), compare_ints);
    if (is_sorted(copy, size)) {
        printf("Test \"%s\": success\n", test_name);
    } else {
        printf("Test \"%s\": failed\n", test_name);
    }
    free(copy);
}

int main() {
    srand(time(NULL));
    const size_t size = 100000; 
    const int runs = 5; 
    int *array = malloc(size * sizeof(int));

    printf("Running correctness tests for qsort:\n");
    generate_sorted(array, size);
    run_test(array, size, "Sorted Array");
    
    generate_reversed(array, size);
    run_test(array, size, "Reverse Sorted Array");
    
    generate_random(array, size);
    run_test(array, size, "Random Array");
    
    generate_constant(array, size);
    run_test(array, size, "Constant Array");
    
    generate_almost_sorted(array, size);
    run_test(array, size, "Almost Sorted Array");
    
    generate_killer(array, size);
    run_test(array, size, "Killer Sequence");

    printf("\nPerformance analysis of qsort (average of %d runs):\n", runs);
    
    generate_sorted(array, size);
    printf("Already sorted array: %f seconds\n", measure_sort_time(array, size, runs));
    
    generate_reversed(array, size);
    printf("Reverse sorted array: %f seconds\n", measure_sort_time(array, size, runs));
    
    generate_random(array, size);
    printf("Random array: %f seconds\n", measure_sort_time(array, size, runs));
    
    generate_constant(array, size);
    printf("Constant value array: %f seconds\n", measure_sort_time(array, size, runs));
    
    generate_almost_sorted(array, size);
    printf("Almost sorted array: %f seconds\n", measure_sort_time(array, size, runs));
    
    generate_killer(array, size);
    printf("Killer sequence array: %f seconds\n", measure_sort_time(array, size, runs));

    free(array);
    return 0;
}