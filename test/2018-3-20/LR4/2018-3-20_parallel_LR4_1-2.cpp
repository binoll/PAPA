#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include "omp.h"

#define MAX_BUF_SIZE 1024

using namespace std;

int main(int argc, char* argv[])
{
    int opt;
    int number, procs, err;

    while ((opt = getopt(argc, argv, "n:p:")) != -1) {
        switch (opt) {
        case 'n':
            sscanf(optarg, "%d", &number);
            break;

        case 'p':
            sscanf(optarg, "%d", &procs);
            break;

        default:
            fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
                argv[0]);
            return 1;
        }
    }

    /*cout << number << endl;
    cout << procs << endl;*/

    int** a = new int* [number];
    int** b = new int* [number];
    int** res = new int* [number];

    for (int i = 0; i < number; i++)
    {
        a[i] = new int[number];
        b[i] = new int[number];
        res[i] = new int[number];
    }

    for (int i = 0; i < number; i++)
    {
        for (int j = 0; j < number; j++)
        {
            a[i][j] = 2;
            b[i][j] = 3;
        }
    }

    omp_set_num_threads(procs);

    //cout << omp_get_num_threads() << endl;

    double start_parallel, end_parallel;
    double start_posled, end_posled;

    int i, j;

    start_parallel = omp_get_wtime();

#pragma omp parallel shared (a, b, res) //private(i,j)
    {
        #pragma omp for //schedule(dynamic)

        for (int i = 0; i < number; i++)
        {
            for (int j = 0; j < number; j++)
            {
                res[i][j] = (a[i][j] * b[i][j]);
            }
        }
    }
    end_parallel = omp_get_wtime();

    /*for (int i = 0; i < number; i++)
    {
        for (int j = 0; j < number; j++)
        {
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }
    cout << endl;
    cout << endl;

    for (int i = 0; i < number; i++)
    {
        for (int j = 0; j < number; j++)
        {
            printf("%d ", b[i][j]);
        }
        printf("\n");
    }
    cout << endl;
    cout << endl;

    for (int i = 0; i < number; i++)
    {
        for (int j = 0; j < number; j++)
        {
            printf("%d ", res[i][j]);
        }
        printf("\n");
    }
    cout << endl;*/


    cout << endl;
    cout << "time to work parallel -> " << end_parallel - start_parallel << endl;



    start_posled = omp_get_wtime();

    for (int i = 0; i < number; i++)
    {
        for (int j = 0; j < number; j++)
        {
            res[i][j] = (a[i][j] * b[i][j]);
        }
    }

    /*for (int i = 0; i < number; i++)
    {
        for (int j = 0; j < number; j++)
        {
            printf("%d ", res[i][j]);
        }
        printf("\n");
    }*/

    end_posled = omp_get_wtime();

    cout << "time to work posled -> " << end_posled - start_posled << endl;

    return 0;
}
