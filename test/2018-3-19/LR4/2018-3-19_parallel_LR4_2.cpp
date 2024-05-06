/*
Разработать OpenMP - программу, выполняющую транспонирование 
квадратной матрицы на произвольном количестве потоков.Произвести
экспериментальное сравнение времени выполнения двух версий 
программ в зависимости от размера матрицы и количества потоков.
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <getopt.h>

void print_matrix(int** Matrix, int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("|%2d| ", Matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    int opt, N, num_thread, i, j, k;
    double start, end;
    while ((opt = getopt(argc, argv, "n:t:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            sscanf(optarg, "%d", &N);
            break;
        case 't':
            sscanf(optarg, "%d", &num_thread);
            break;
        default:
            fprintf(stderr, "Invalid parameter. Set option -n <size> -t <num_thread>\n", argv[0]);
            return 0;
        }
    }
    if (argc != 5)
    {
        fprintf(stderr, "Invalid parameter. Set option -n <size> -t <num_thread>\n");
        return 0;
    }

    int** Matrix_A = (int**)malloc(N * sizeof(int*));
    int** Matrix_B = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++)
    {
        Matrix_A[i] = (int*)malloc(N * sizeof(int));
        Matrix_B[i] = (int*)malloc(N * sizeof(int));
        for (int j = 0; j < N; j++)
        {
            Matrix_A[i][j] = i * N + j;
        }
    }

    omp_set_num_threads(num_thread);

    start = omp_get_wtime();
    #pragma omp parallel for shared(Matrix_A, Matrix_B) private(i, j, k)
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            Matrix_B[j][i] = Matrix_A[i][j];
        }
    }
    end = omp_get_wtime();

    printf("Matrix_A:\n");
    print_matrix(Matrix_A, N);
    printf("Matrix_A transp:\n");
    print_matrix(Matrix_B, N);
    printf("Time: %f\n\n", end - start);

    return 0;
}