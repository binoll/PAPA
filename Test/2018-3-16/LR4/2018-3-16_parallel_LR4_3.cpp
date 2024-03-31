#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <getopt.h>
#include <time.h>

void print_vector(int* Vector, int N)
{
    for (int i = 0; i < N; i++)
    {
        printf("|%2d| ", Vector[i]);
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

    int* Vector_A = (int*)malloc(N * sizeof(int));
    int* Vector_B = (int*)malloc(N * sizeof(int));
    int* Vector_C = (int*)malloc(N * sizeof(int));
    int* Vector_D = (int*)malloc(N * sizeof(int));
    int* Vector_E = (int*)malloc(N * sizeof(int));
    for (i = 0; i < N; i++) {
        Vector_A[i] = i * N + j + 1;
        Vector_B[i] = i * N + j + 1;
        Vector_C[i] = 0;
        Vector_D[i] = 0;
        Vector_E[i] = 0;
    }

    omp_set_num_threads(num_thread);

    start = omp_get_wtime();
    #pragma omp parallel shared(Vector_A, Vector_B, Vector_C, Vector_D, Vector_E) private(i)
    {
        for (i = 0; i < N; i++)
        {
            Vector_C[i] = Vector_A[i] + Vector_B[i];
        }
        for (i = 0; i < N; i++)
        {
            Vector_D[i] = Vector_A[i] - Vector_B[i];
        }
        for (i = 0; i < N; i++)
        {
            Vector_E[i] = Vector_A[i] * Vector_B[i];
        }
    }
    end = omp_get_wtime();

    printf("Vector_A:\n");
    print_vector(Vector_A, N);
    printf("Vector_B:\n");
    print_vector(Vector_B, N);
    printf("Vector_C +:\n");
    print_vector(Vector_C, N);
    printf("Vector_D -:\n");
    print_vector(Vector_D, N);
    printf("Vector_E *:\n");
    print_vector(Vector_E, N);
    printf("Time: %f\n", end - start);

    return 0;
}
