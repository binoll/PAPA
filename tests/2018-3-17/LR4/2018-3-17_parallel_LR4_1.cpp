#include <iostream>
#include <omp.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>

using namespace std;

void print_matrix(int* A[], int N)
{
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            cout << A[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}


int main(int argc, char **argv)
{
    int N = 10, P = 8, opt;
    double start, stop;
    int i, j, k;
    while ((opt = getopt(argc, argv, "p:n: :")) != -1)
    {
        switch (opt)
        {
            case 'n':
                N = atoi(optarg);
                break;
            case 'p':
                P = atoi(optarg);
            case ' ':
                break;
            default:
                fprintf(stderr, ": %s [-n process_id]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    int **A = new int*[N];
    int **B = new int*[N];
    int **C = new int*[N];
    int **D = new int*[N];
    for(int i = 0; i < N; i++)
    {
        A[i] = new int[N];
        B[i] = new int[N];
        C[i] = new int[N];
        D[i] = new int[N];
    }

    for(int j = 0, i = 0; i < N; i++)
        for(j = 0; j < N; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }

    omp_set_num_threads(P);
    start = omp_get_wtime();
    #pragma omp parallel shared(A,B,D) private(i,j,k)
    {
        #pragma omp for schedule(dynamic)
        for ( i = 0; i < N; i++)
            for ( j = 0; j < N; j++)
                C[i][j] = A[i][j] * B[i][j];

    }
    stop = omp_get_wtime();
    cout << "Time (element by element): " << stop - start << endl;

    start = omp_get_wtime();
    omp_set_num_threads(P);
    #pragma omp parallel shared(A,B,D) private(i,j,k)
    {
        #pragma omp for schedule(dynamic) 
        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++)
                for (k = 0; k < N; k++)
                    D[i][j] += A[i][k] * B[k][j];
    }
    stop = omp_get_wtime();

    cout << "Time (column by column): " << stop - start << endl;
    cout << "Matrix size: " << N << endl;
    cout << "Proccess count: " << P << endl;

    return 0;
}
