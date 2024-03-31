#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"


void print_matrix(int n, int m, int matrix[][m])
{
    int i, j;

    printf("%*s", 3, "M");
    for (j = 0; j < m; j++) {
        printf("%*d", 3, j);
    }
    printf("\n");


    for (i = 0; i < n; i++) {
        printf("%*d", 3, i);
        for (j = 0; j < m; j++) {
            printf("%*d", 3, matrix[i][j]);
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
            fprintf(stderr, "Invalid parameters. Value: 0%o\n", opt);
            exit(EXIT_FAILURE);
        }
    }
    if (argc != 7) {
        fprintf(stderr, "Invalid parameters. Set dims of matrixs: A[n x k], B[k x m] by -n <n> -k <k> -m <m>\n");
        exit(EXIT_FAILURE);
    }
    if (n > N) {
        printf("Invalid count of proccess: n <= N\n");
        exit(EXIT_FAILURE);
    }

    // По local_N строк будет обрабатывать каждый процесс
    local_N = N / n;

    int A[N][K];
    int B[K][M];
    int C[N][M];

    // A делится на local_N подматриц, обрабатываемых в разных потоках
    int local_A[local_N][K];
    int local_C[local_N][M];

    // 0 ранг заполняет матрицы для умножения
    if (rank == 0) {

        for (i = 0; i < N; i++) {
            for (j = 0; j < K; j++) {
                A[i][j] = i * K;
            }
        }

        for (i = 0; i < K; i++) {
            for (j = 0; j < M; j++) {
                B[i][j] = i * M;
            }
        }
    }

    // Передаем часть матрицы A во все процессы
    MPI_Scatter(A, local_N * K, MPI_INT, local_A, local_N * K, MPI_INT, 0, MPI_COMM_WORLD);

    // Передаем матрицу B во все процессы
    MPI_Bcast(B, K * M, MPI_INT, 0, MPI_COMM_WORLD);

    // Перемножение матриц
    for (i = 0; i < local_N; i++) {
        for (j = 0; j < M; j++) {
            sum = 0;
            for (k = 0; k < K; k++) {
                sum += local_A[i][k] * B[k][j];
            }
            local_C[i][j] = sum;
        }
    }

    // Собираем все данные в ранг 0 
    MPI_Gather(local_C, local_N * M, MPI_INT, C, local_N * M, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int rest = N % n;

        // То, что не распределилось по процессам (не разделилось нацело), дообрабатывается в процессе 0
        if (rest != 0 && N > n) {
            for (i = N - rest - 1; i < N; i++) {
                for (j = 0; j < M; j++) {
                    sum = 0;
                    for (k = 0; k < K; k++) {
                        sum += A[i][k] * B[k][j];
                    }
                    C[i][j] = sum;
                }
            }
        }

        printf("Matrix A\n"); print_matrix(N, K, A);
        printf("Matrix B\n"); print_matrix(K, M, B);
        printf("Matrix C\n"); print_matrix(N, M, C);
    }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}