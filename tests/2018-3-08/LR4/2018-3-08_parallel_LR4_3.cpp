/*
 *   3. Разработать последовательную и параллельную реализацию программы (с использованием OpenMP),
 *      демонстрирующую выполнение арифметических операций над векторами. Произвести
 *      экспериментальноесравнение времени выполнения двух версий программ
 *      в зависимости от размера матрицы и количества потоков.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <omp.h>

void print_vector(int n, int *vector)
{
    int i;
    for(i = 0; i < n; i++) {
        printf("%3d ", vector[i]);
    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    int num_threads, size, i;
    int opt, pr = 1;
    int *Vector_A, *Vector_B, *Vector_C;

    while ((opt = getopt(argc, argv, "s:n:")) != -1) {
        switch(opt) {
            case 's':
                size = atoi(optarg);
                if (size <= 0) {
                    fprintf(stderr, "Invalid parameters. Size of vector must be > 0 \n");
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

    Vector_A = (int*)malloc(size*sizeof(int));
    Vector_B = (int*)malloc(size*sizeof(int));
    Vector_C = (int*)malloc(size*sizeof(int));

    // // Filling matrix with random data [0,10]
    #pragma omp parallel for schedule(dynamic, 1) private(i)
    for (i = 0; i < size; i++) {
        Vector_A[i] = rand() % 10;
        Vector_B[i] = rand() % 10;
        Vector_C[i] = 0;
    }

    double start, end;
    start = omp_get_wtime();
    // ************************ OpenMP_Func **************************** //
    // schedule(dynamic, 1) - dynamic separation with n=1
    #pragma omp parallel for schedule(dynamic, 1) private(i)
    for (i = 0; i < size; i++) {
        Vector_C[i] = Vector_A[i] + Vector_B[i];
    }
    end = omp_get_wtime();
    printf("# Time elapsed with openmp -  %f seconds\n", end - start);

    // Output only when size < 10
    if (pr == 1) {
        printf("# Vector A \n");
        print_vector(size, Vector_A);
        printf("# Vector B \n");
        print_vector(size, Vector_B);
        printf("# Vector C = A + B \n");
        print_vector(size, Vector_C);
    }

    // Сhecking the correctness of addition
    for (i = 0; i < size; i++) {
        if (Vector_C[i] != Vector_A[i] + Vector_B[i]) {
            printf("Error addition on position (%d)\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Free vectors memory
    free(Vector_A);
    free(Vector_B);
    free(Vector_C);

    exit(EXIT_SUCCESS);
}
