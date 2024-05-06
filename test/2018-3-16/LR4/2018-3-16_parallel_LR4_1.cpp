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
            printf("|%3d| ", Matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

int main(int argc, char* argv[])
{
    int opt, N, num_thread, i, j, k;
    double start_1, end_1, start_2, end_2;
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
    int** Matrix_C = (int**)malloc(N * sizeof(int*));
    int** Matrix_D = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++)
    {
        Matrix_A[i] = (int*)malloc(N * sizeof(int));
        Matrix_B[i] = (int*)malloc(N * sizeof(int));
        Matrix_C[i] = (int*)malloc(N * sizeof(int));
        Matrix_D[i] = (int*)malloc(N * sizeof(int));
        for (int j = 0; j < N; j++)
        {
            Matrix_A[i][j] = i * N + j;
            Matrix_B[i][j] = i * N + j;
        }
    }

    omp_set_num_threads(num_thread);

    start_1 = omp_get_wtime();
    #pragma omp parallel for shared(Matrix_A, Matrix_B, Matrix_C) private(i, j, k)
    for (i = 0; i < N; i++) 
    {
        for (j = 0; j < N; j++) 
        {
            Matrix_C[i][j] = 0;
            for (k = 0; k < N; k++)
            {
                Matrix_C[i][j] += Matrix_A[i][k] * Matrix_B[k][j];
            }
        }
    }
    end_1 = omp_get_wtime();


    start_2 = omp_get_wtime();
    #pragma omp parallel for shared(Matrix_A, Matrix_B, Matrix_D) private(i, j, k)
    for (i = 0; i < N; i++) 
    {
        for (j = 0; j < N; j++) 
        {
            Matrix_D[i][j] = Matrix_A[i][j] * Matrix_B[i][j];
        }
    }
    end_2 = omp_get_wtime();

    printf("Matrix_A:\n");
    print_matrix(Matrix_A, N);
    printf("Matrix_B:\n");
    print_matrix(Matrix_B, N);
    printf("Matrix_C col:\n");
    print_matrix(Matrix_C, N);
    printf("Time: %f\n\n", end_1 - start_1);
    printf("Matrix_D elem:\n");
    print_matrix(Matrix_D, N);
    printf("Time: %f\n\n", end_2 - start_2);

	return 0;
}
