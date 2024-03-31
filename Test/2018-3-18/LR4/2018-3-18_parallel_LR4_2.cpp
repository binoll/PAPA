#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
using namespace std;

int main(int argc, char* argv[])
{
    int opt, size, num, proof_print = 0;
    while ((opt = getopt(argc, argv, "n:s:p")) != -1) {
        switch (opt) {
            case 'n':
                sscanf(optarg, "%d", &num);
                break;
            case 's':
                sscanf(optarg, "%d", &size);
                break;
            case 'p':
                proof_print = 1;
                break;
        }
    }
    
    double start, end;
    int i, j, k;
    int sum = 0;

    int **A, **B;
    A = (int**)malloc(size * sizeof(int*));
    B = (int**)malloc(size * sizeof(int*));

    omp_set_num_threads(num);

#pragma omp parallel for schedule(dynamic, 1) private(i, j)
    for (i = 0; i < size; i++)
    {
        A[i] = (int*)malloc(size * sizeof(int));
        B[i] = (int*)malloc(size * sizeof(int));
       
        for (j = 0; j < size; j++)
        {
            B[i][j] = rand() % 10;
        }
    }

    start = omp_get_wtime();

#pragma omp parallel for schedule(dynamic) private(i,j)
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j)
            A[i][j] = B[j][i];
    end = omp_get_wtime();

    cout << (end - start) << " sec" << endl;
    if (proof_print) {
        cout << endl << "Matrix A:" << endl;
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
                cout << A[i][j] << " ";
            cout << endl;
        }
        cout << endl << "Matrix T-A:" << endl;
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
                cout << B[i][j] << " ";
            cout << endl;
        }
    }
    cout << endl << endl;
    free(A);
    free(B);
}