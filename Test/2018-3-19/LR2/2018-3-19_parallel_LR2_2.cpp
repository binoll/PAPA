/*
Реализовать алгоритм перемножения матриц с использованием функций
коллективного взаимодействия.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

void print_matrix(int n, int m, int matrix[])
{
    int i, j;
    for(i = 0; i < n; i++) 
    {
        for(j = 0; j < m; j++) 
        {
            printf("%3d ", matrix[i * m + j]);
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char* argv[])
{
    int rank, n, opt, local_N, sum;
    int N, K, M;
    int i, j, k;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while ((opt = getopt(argc, argv, "n:k:m:")) != -1)
    {
        switch (opt) 
        {
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
    if (argc != 7) 
    {
        fprintf(stderr, "Invalid parameters. Set dims of matrices: A[n x k], B[k x m] by -n <n> -k <k> -m <m>\n");
        exit(EXIT_FAILURE);
    }
    if (n > N) 
    {
        printf("Invalid count of process: n <= N\n");
        exit(EXIT_FAILURE);
    }

    //Количество строк, выделенных для каждой матрицы
    local_N = N / n;

    int Matrix_A[N * K];
    int Matrix_B[K * M];
    int Matrix_C[N * M];

    //Матрица присваивается каждому процессу
    int local_Matrix_A[local_N * K];
    int local_Matrix_C[local_N * M];
    
    //Заполнение матриц
    if (rank == 0) 
    {
        for(i = 0; i < N; i++) 
        {
            for(j = 0; j < K; j++) 
            {
                Matrix_A[i * K + j] = i * K + j;
            }
        }

        for(i = 0; i < K; i++) 
        {
            for(j = 0; j < M; j++) 
            {
                Matrix_B[i * M + j] = i * M + j;
            }
        }

        //Распределение данных
        MPI_Scatter(Matrix_A, local_N * K,  MPI_INT, local_Matrix_A, local_N * K, MPI_INT, 0, MPI_COMM_WORLD);
        
        //Передача данных
        MPI_Bcast(Matrix_B, K * M, MPI_INT, 0, MPI_COMM_WORLD);
        
        //Расчет результатов
        for(i = 0; i < local_N; i++) 
        {
            for(j = 0; j < M; j++) 
            {
                sum = 0;
                for (k = 0; k < K; k++) 
                {
                    sum += local_Matrix_A[i * K + k] * Matrix_B[k * M + j];
                }
                local_Matrix_C[i * M + j] = sum;
            }
        }

        //Сбор результатов
        MPI_Gather(local_Matrix_C, local_N * M, MPI_INT, Matrix_C, local_N * M, MPI_INT, 0, MPI_COMM_WORLD);
    
        //Обработка оставшейся строки (обработка ситуации, которая не может быть делимой)
        int rest = N % n;

        if (rest != 0 && N > n) {
            for(i = N - rest - 1; i < N; i++) {
                for(j = 0; j < M; j++) {
                    sum = 0;
                    for (k = 0; k < K; k++) {
                        sum += Matrix_A[i * K + k] * Matrix_B[k * M + j];
                    }
                    Matrix_C[i * M + j] = sum;
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
    else
    {
		//Распределение данных
        MPI_Scatter(Matrix_A, local_N * K,  MPI_INT, local_Matrix_A, local_N * K, MPI_INT, 0, MPI_COMM_WORLD);
        
        //Передача данных
        MPI_Bcast(Matrix_B, K * M, MPI_INT, 0, MPI_COMM_WORLD);

		//Расчет результатов
        for(i = 0; i < local_N; i++) 
        {
            for(j = 0; j < M; j++) 
            {
                sum = 0;
                for (k = 0; k < K; k++) 
                {
                    sum += local_Matrix_A[i * K + k] * Matrix_B[k * M + j];
                }
                local_Matrix_C[i * M + j] = sum;
            }
        }

		//Сбор результатов
        MPI_Gather(local_Matrix_C, local_N * M, MPI_INT, Matrix_C, local_N * M, MPI_INT, 0, MPI_COMM_WORLD);
    }


    MPI_Finalize();
    return 0;
}