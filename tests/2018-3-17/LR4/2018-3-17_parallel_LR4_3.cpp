#include <iostream>
#include <omp.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
    int N = 10, P = 8, opt;
    double start, stop;
    int i;
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

    int *A = new int[N];
    int *B = new int[N];
    int *C = new int[N];
    for(i = 0; i < N; i++)
    {
        A[i] = rand() % 10;
        B[i] = rand() % 10;
        C[i] = 0;
    }

    start = omp_get_wtime();
    omp_set_num_threads(P);

    #pragma omp parallel for private(i) schedule(dynamic)
    for(i = 0; i < N; i++)
        C[i] = A[i] + B[i] + A[i] * B[i];
    
    stop = omp_get_wtime();

    cout << "Time: " << stop - start << endl;
    cout << "Vector size: " << N << endl;
    cout << "Proccess count: " << P << endl;

    return 0;
}
