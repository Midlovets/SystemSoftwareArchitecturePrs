#include <stdio.h>
#include <stdlib.h>

int main() {
    int *arr = (int*)malloc(5 * sizeof(int));
    arr[5] = 10; 
    free(arr);
    return 0;
}
