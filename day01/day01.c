#include <stdio.h>
#include <stdlib.h>

// Comparison function for qsort
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int main(int argc, char **argv) {
    FILE* file;
    int left[1000];
    int right[1000];
    int n = 0;
    int diff = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Could not read file %s", argv[1]);
    } 

    while (fscanf(file, "%d %d", &left[n],&right[n]) == 2) n++;

    fclose(file);

    qsort(left, n, sizeof(int), compare);
    qsort(right, n, sizeof(int), compare);

    for(int j = 0; j < n; j++){
        diff += abs(left[j] - right[j]);
    } 

    printf("part 1: %d\n", diff);

    return 0;
} 