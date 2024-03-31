#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    int size, num_threads;
    char opt;

    while ((opt = getopt(argc, argv, "s:t:")) != -1)
    {
        switch (opt)
        {
        case 's':
            size = atoi(optarg);
            break;
        case 't':
            num_threads = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Error in arguments.\n");
            exit(1);
        }
    }

    int* a = new int[size];
    int* b = new int[size];
    int* c = new int[size];

    int i;
    double start, finish;

    for (i = 0; i < size; ++i)
    {
        a[i] = i + 1;
        b[i] = i + 1;
        c[i] = 0;
    }

    start = omp_get_wtime();
    for (i = 0; i < size; i++)
        c[i] = a[i] + b[i];
    finish = omp_get_wtime();
    cout << "Execution time sum_vector: " << finish - start << endl;


    delete[] a;
    delete[] b;
    delete[] c;
}
