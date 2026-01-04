/*
 * matrix.c
 *
 * Parallel matrix add, subtract, and product using pthreads.
 *
 * Usage: ./matrix <N>
 * Example: ./matrix 20
 *
 * This program divides work by rows across up to 10 threads.
 */
 // Generated using ChatGPT AI

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int MAX; // matrix dimension (assigned from argv)
int *matA;
int *matB;
int *matSumResult;
int *matDiffResult;
int *matProductResult;

#define IDX(r, c) ((r) * MAX + (c))

typedef struct {
    int start_row;
    int end_row; // exclusive
} ThreadArgs;

void fillMatrix(int *matrix) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matrix[IDX(i, j)] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int *matrix) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            printf("%5d", matrix[IDX(i, j)]);
        }
        printf("\n");
    }
    printf("\n");
}

/* Thread worker for sum */
void *computeSum(void *args) {
    ThreadArgs *targs = (ThreadArgs *)args;
    int sr = targs->start_row;
    int er = targs->end_row;
    for (int r = sr; r < er; r++) {
        for (int c = 0; c < MAX; c++) {
            matSumResult[IDX(r, c)] = matA[IDX(r, c)] + matB[IDX(r, c)];
        }
    }
    free(targs);
    return NULL;
}

/* Thread worker for diff */
void *computeDiff(void *args) {
    ThreadArgs *targs = (ThreadArgs *)args;
    int sr = targs->start_row;
    int er = targs->end_row;
    for (int r = sr; r < er; r++) {
        for (int c = 0; c < MAX; c++) {
            matDiffResult[IDX(r, c)] = matA[IDX(r, c)] - matB[IDX(r, c)];
        }
    }
    free(targs);
    return NULL;
}

/* Thread worker for product (row block of result) */
void *computeProduct(void *args) {
    ThreadArgs *targs = (ThreadArgs *)args;
    int sr = targs->start_row;
    int er = targs->end_row;
    for (int r = sr; r < er; r++) {
        for (int c = 0; c < MAX; c++) {
            long sum = 0;
            for (int k = 0; k < MAX; k++) {
                sum += (long)matA[IDX(r, k)] * (long)matB[IDX(k, c)];
            }
            matProductResult[IDX(r, c)] = (int)sum;
        }
    }
    free(targs);
    return NULL;
}

int main(int argc, char *argv[]) {
    srand((unsigned)time(NULL)); // keep as requested in template

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <matrix-size>\n", argv[0]);
        return 1;
    }

    MAX = atoi(argv[1]);
    if (MAX <= 0) {
        fprintf(stderr, "Invalid matrix size: %s\n", argv[1]);
        return 1;
    }

    /* Allocate flattened matrices */
    size_t total = (size_t)MAX * (size_t)MAX;
    matA = malloc(total * sizeof(int));
    matB = malloc(total * sizeof(int));
    matSumResult = malloc(total * sizeof(int));
    matDiffResult = malloc(total * sizeof(int));
    matProductResult = malloc(total * sizeof(int));

    if (!matA || !matB || !matSumResult || !matDiffResult || !matProductResult) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    /* 1. Fill the matrices (matA and matB) */
    fillMatrix(matA);
    fillMatrix(matB);

    /* 2. Print the initial matrices. */
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    /* Determine number of threads: use 10 or less if MAX < 10 */
    int num_threads = 10;
    if (MAX < num_threads) num_threads = MAX;

    /* Arrays of pthread_t for each operation */
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    if (!threads) {
        fprintf(stderr, "Failed to allocate thread array\n");
        return 1;
    }

    /* Helper to spawn and join threads for an operation */
    // Sum
    for (int i = 0; i < num_threads; i++) {
        ThreadArgs *targs = malloc(sizeof(ThreadArgs));
        int rows_per_thread = MAX / num_threads;
        int extra = MAX % num_threads;
        int start = i * rows_per_thread + (i < extra ? i : extra);
        int end = start + rows_per_thread + (i < extra ? 1 : 0);
        targs->start_row = start;
        targs->end_row = end;
        if (pthread_create(&threads[i], NULL, computeSum, (void *)targs) != 0) {
            fprintf(stderr, "Failed to create sum thread %d\n", i);
            return 1;
        }
    }
    for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);

    // Diff
    for (int i = 0; i < num_threads; i++) {
        ThreadArgs *targs = malloc(sizeof(ThreadArgs));
        int rows_per_thread = MAX / num_threads;
        int extra = MAX % num_threads;
        int start = i * rows_per_thread + (i < extra ? i : extra);
        int end = start + rows_per_thread + (i < extra ? 1 : 0);
        targs->start_row = start;
        targs->end_row = end;
        if (pthread_create(&threads[i], NULL, computeDiff, (void *)targs) != 0) {
            fprintf(stderr, "Failed to create diff thread %d\n", i);
            return 1;
        }
    }
    for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);

    // Product
    for (int i = 0; i < num_threads; i++) {
        ThreadArgs *targs = malloc(sizeof(ThreadArgs));
        int rows_per_thread = MAX / num_threads;
        int extra = MAX % num_threads;
        int start = i * rows_per_thread + (i < extra ? i : extra);
        int end = start + rows_per_thread + (i < extra ? 1 : 0);
        targs->start_row = start;
        targs->end_row = end;
        if (pthread_create(&threads[i], NULL, computeProduct, (void *)targs) != 0) {
            fprintf(stderr, "Failed to create product thread %d\n", i);
            return 1;
        }
    }
    for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);

    /* 6. Print the results */
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    /* Cleanup */
    free(matA);
    free(matB);
    free(matSumResult);
    free(matDiffResult);
    free(matProductResult);
    free(threads);

    return 0;
}
