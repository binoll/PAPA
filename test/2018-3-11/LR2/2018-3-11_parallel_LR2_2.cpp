%%writefile task3.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

void print_matrix(int n, int m, int matrix[])
{
    int i, j;
    for(i = 0; i < n; i++) {
        for(j = 0; j < m; j++) {
            printf("%3d ", matrix[i * m + j]);
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char* argv[])
{
    int rank, n, opt, blockRows, sum;
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
            fprintf(stderr, "Invalid parameters. Set dims of matrices: A[n x k], B[k x m] by -n <n> -k <k> -m <m>\n");
            exit(EXIT_FAILURE);
        }
    }
    if (argc != 7) {
        fprintf(stderr, "Invalid parameters. Set dims of matrices: A[n x k], B[k x m] by -n <n> -k <k> -m <m>\n");
        exit(EXIT_FAILURE);
    }
    if (n > N) {
        printf("Invalid count of process: n <= N\n");
        exit(EXIT_FAILURE);
    }


    blockRows = N / n;

    int Matrix_A[N * K];
    int Matrix_B[K * M];
    int Matrix_C[N * M];

    int local_Matrix_A[blockRows * K];
    int local_Matrix_C[blockRows * M];

    if (!rank) {
        for(i = 0; i < N; i++) {
            for(j = 0; j < K; j++) {
                Matrix_A[i * K + j] = i * K + j;
            }
        }

        for(i = 0; i < K; i++) {
            for(j = 0; j < M; j++) {
                Matrix_B[i * M + j] = i * M + j;
            }
        }
    }

    MPI_Scatter(Matrix_A, blockRows * K,  MPI_INT, local_Matrix_A, blockRows * K, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(Matrix_B, K * M, MPI_INT, 0, MPI_COMM_WORLD);

    for(i = 0; i < blockRows; i++) {
        for(j = 0; j < M; j++) {
            sum = 0;
            for (k = 0; k < K; k++) {
                sum += local_Matrix_A[i*K+k] * Matrix_B[k*M+j];
            }
            local_Matrix_C[i*M+j] = sum;
        }
    }

    MPI_Gather(local_Matrix_C, blockRows * M, MPI_INT, Matrix_C, blockRows * M, MPI_INT, 0, MPI_COMM_WORLD);

    if (!rank) {
        int remains = N % n;

        if (remains != 0 && N > n) {
            for(i = N - remains - 1; i < N; i++) {
                for(j = 0; j < M; j++) {
                    sum = 0;
                    for (k = 0; k < K; k++) {
                        sum += Matrix_A[i*K+k] * Matrix_B[k*M+j];
                    }
                    Matrix_C[i*M+j] = sum;
                }
            }
        }
        printf("Matrix A\n");
        print_matrix(N, K, Matrix_A);
        printf("Matrix B\n");
        print_matrix(K, M, Matrix_B);
        printf("Matrix C = [A x B]\n");
        print_matrix(N, M, Matrix_C);
    }

    MPI_Finalize();
    return 0
}