
/*
    3. Реализовать алгоритм перемножения матриц с использованием функций
        коллективного взаимодействия.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

#define ROOT_RANK 0

void print_matrix(int n, int m, int matrix[][m])
{
    int i, j;

    printf(" X   ");
    for(j = 0; j < m; j++) {
        printf("   [%2d] ", j);
    }
    printf("\n");


    for(i = 0; i < n; i++) {
        printf("[%2d] ", i);
        for(j = 0; j < m; j++) {
            printf("%7d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char* argv[])
{
    int rank, n, opt, local_N, sum;
    int N, K, M;
    int i, k, j;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while ((opt = getopt(argc, argv, "n:k:m:")) != -1) {
        switch (opt) {
        case 'n':
            N = atoi(optarg);
            break;
        case 'k':
            K = atoi(optarg);
            break;
        case 'm':
            M = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Invalid parameters. Getopt return value: 0%o\n", opt);
            exit(EXIT_FAILURE);
        }
    }
    if (argc != 7) {
        fprintf(stderr, "Invalid parameters. Set dims of matrixs: A[n x k], B[k x m] by -n <n> -k <k> -m <m>\n");
        exit(EXIT_FAILURE);
    }
    if (n > N) {
        printf("Invalid count of proccess: np <= n\n");
        exit(EXIT_FAILURE);
    }

    // По local_N строк будет обрабатывать каждый процесс
    local_N = N / n;

    int Matrix_A[N][K];
    int Matrix_B[K][M];
    int Matrix_C[N][M];

    // Matrix_A делится на local_N подматриц, обрабатываемых в разных потоках
    int local_Matrix_A[local_N][K];
    int local_Matrix_C[local_N][M];

    if (rank == ROOT_RANK) {

        for(i = 0; i < N; i++) {
            for(j = 0; j < K; j++) {
                Matrix_A[i][j] = i * K + j;
            }
        }

        for(i = 0; i < K; i++) {
            for(j = 0; j < M; j++) {
                Matrix_B[i][j] = i * M + j;
            }
        }
    }

    MPI_Scatter(Matrix_A, local_N * K,  MPI_INT, local_Matrix_A, local_N * K, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

    MPI_Bcast(Matrix_B, K * M, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

    for(i = 0; i < local_N; i++) {
        for(j = 0; j < M; j++) {
            sum = 0;
            for (k = 0; k < K; k++) {
                sum += local_Matrix_A[i][k] * Matrix_B[k][j];
            }
            local_Matrix_C[i][j] = sum;  
        }
    }

    MPI_Gather(local_Matrix_C, local_N * M, MPI_INT, Matrix_C, local_N * M, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

    if (rank == ROOT_RANK) {
        int rest = N % n;

        // То, что не распределилось по процессам (не разделилось нацело), дообрабатывается в процессе ROOT_RANK
        if (rest != 0 && N > n) {
            for(i = N - rest - 1; i < N; i++) {
                for(j = 0; j < M; j++) {
                    sum = 0;
                    for (k = 0; k < K; k++) {
                        sum += Matrix_A[i][k] * Matrix_B[k][j];
                    }
                    Matrix_C[i][j] = sum;  
                }
            }
        }

        printf("Matrix A\n"); print_matrix(N, K, Matrix_A);
        printf("Matrix B\n"); print_matrix(K, M, Matrix_B);
        printf("Matrix C\n"); print_matrix(N, M, Matrix_C);
    }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}