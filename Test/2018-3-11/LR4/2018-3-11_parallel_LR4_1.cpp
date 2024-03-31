%%writefile 1.cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <omp.h>

void print_matrix(int n, int m, int **matrix)
{
    int i, j;
    for(i = 0; i < n; i++) {
        for(j = 0; j < m; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    int num_threads, size, sum, i, j, k;
    int opt, pr = 1;
    int **Matrix_A, **Matrix_B, **Matrix_C, **Matrix_D;

    while ((opt = getopt(argc, argv, "s:n:")) != -1) {
        switch(opt) {
            case 's':
                size = atoi(optarg);
                if (size <= 0) {
                    fprintf(stderr, "Invalid parameters. Size of matrix must be > 0 \n");
                    exit(EXIT_FAILURE);
                }
                if (size > 10) {
                    pr = 0;
                }
                break;
            case 'n':
                num_threads = atoi(optarg);
                if (num_threads <= 0) {
                    fprintf(stderr, "Invalid parameters. Number of threads must be > 0 \n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Invalid parameters . Set option -s <size> -n <threads>\n");
                exit(EXIT_FAILURE);
        }
    }
    if (argc !=5) {
        fprintf(stderr, "Invalid parameters count. Set option -s <size> -n <threads>\n");
        exit(EXIT_FAILURE);
    }

    omp_set_num_threads(num_threads);

    Matrix_A = (int**)malloc(size*sizeof(int*));
    Matrix_B = (int**)malloc(size*sizeof(int*));
    Matrix_C = (int**)malloc(size*sizeof(int*));
    Matrix_D = (int**)malloc(size*sizeof(int*));
    #pragma omp parallel for schedule(dynamic, 1) private(i, j)
    for (i = 0; i < size; i++) {
        Matrix_A[i] = (int*)malloc(size * sizeof(int));
        Matrix_B[i] = (int*)malloc(size * sizeof(int));
        Matrix_C[i] = (int*)malloc(size * sizeof(int));
        Matrix_D[i] = (int*)malloc(size * sizeof(int));
        #pragma omp parallel for schedule(dynamic, 1) private(j)
        for (j = 0; j < size; j++) {
            Matrix_A[i][j] = rand() % 10;
            Matrix_B[i][j] = rand() % 10;
        }
    }

    double start_el, end_el;
    start_el = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic, 1) private(i, j)
    for (i = 0; i < size; i++) {
        #pragma omp parallel for schedule(dynamic, 1) private(j)
        for (j = 0; j < size; j++) {
            Matrix_C[i][j] = Matrix_A[i][j] * Matrix_B[i][j];
        }
    }
    end_el = omp_get_wtime();
    printf("#Element: Time elapsed with openmp -  %f seconds\n", end_el - start_el);

    double start, end;
    start = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic, 1) private(i, j, k, sum)
    for (i = 0; i < size; i++) {
        #pragma omp parallel for schedule(dynamic, 1) private(j, k, sum)
        for (j = 0; j < size; j++) {
            sum = 0;
            #pragma omp parallel for schedule(dynamic, 1) private(k) reduction(+:sum)
            for (k = 0; k < size; k++) {
                sum += Matrix_A[i][k] * Matrix_B[k][j];
            }
            Matrix_D[i][j] = sum;
        }
    }
    end = omp_get_wtime();
    printf("#Column: Time elapsed with openmp -  %f seconds\n", end - start);

    if (pr == 1) {
        printf(" Matrix A \n");
        print_matrix(size, size, Matrix_A);
        printf(" Matrix B \n");
        print_matrix(size, size, Matrix_B);
        printf(" Matrix C = AxB (elementwise: C[i][j] = A[i][j] * B[i][j]) \n");
        print_matrix(size, size, Matrix_C);
        printf(" Matrix D = AxB\n");
        print_matrix(size, size, Matrix_D);
    }

    #pragma omp parallel for schedule(dynamic, 1) private(i, j)
    for (i = 0; i < size; i++) {
        free((void *)Matrix_A[i]);
        free((void *)Matrix_B[i]);
        free((void *)Matrix_C[i]);
        free((void *)Matrix_D[i]);
    }
    free(Matrix_A);
    free(Matrix_B);
    free(Matrix_C);
    free(Matrix_D);

    exit(EXIT_SUCCESS);
}
