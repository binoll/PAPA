#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include "omp.h"

#define MAX_BUF_SIZE 1024

using namespace std;

void func_sum(int* a, int* b, int* res, int number)
{
    #pragma omp parallel for shared (a, b, res)
    
        for (int i = 0; i < number; i++)
        {
            res[i] = a[i] + b[i];
        }
    
}

void func_dell(int* a, int* b, int* res, int number)
{
    #pragma omp parallel for shared (a, b, res)

        for (int i = 0; i < number; i++)
        {
            res[i] = a[i] / b[i];
        }
    
}

void func_prond(int* a, int* b, int* res, int number)
{
    #pragma omp parallel for shared (a, b, res)

        for (int i = 0; i < number; i++)
        {
            res[i] = a[i] * b[i];
        }
    
}

int main(int argc, char* argv[])
{
    int opt;
    long number;
    int procs, err;

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

    int* a = new int [number];
    int* b = new int [number];
    int* res = new int [number];

    for (int i = 0; i < number; i++)
    {
        a[i] = 4;
        b[i] = 2;
        
    }

    omp_set_num_threads(procs);

    //cout << omp_get_num_threads() << endl;

    double start_parallel, end_parallel;
    double start_posled, end_posled;

    cout << endl;

    start_parallel = omp_get_wtime();

    func_sum(a, b, res, number);

    end_parallel = omp_get_wtime();

    /*cout << "rez -> ";

    for (int i = 0; i < number; i++)
    {
        cout << res[i] << " ";
    }
    cout << endl;*/
    
    cout << "time to work parallel sum -> " << end_parallel - start_parallel << endl << endl;


    //===================================================================================

    start_parallel = omp_get_wtime();

    func_dell(a, b, res, number);

    end_parallel = omp_get_wtime();

    /*cout << "rez -> ";

    for (int i = 0; i < number; i++)
    {
        cout << res[i] << " ";
    }
    cout << endl;*/
    cout << "time to work parallel dell -> " << end_parallel - start_parallel << endl << endl;


    //===================================================================================

    start_parallel = omp_get_wtime();

    func_prond(a, b, res, number);

    end_parallel = omp_get_wtime();

    /*cout << "rez -> ";

    for (int i = 0; i < number; i++)
    {
        cout << res[i] << " ";
    }
    cout << endl;*/
    cout << "time to work parallel prond -> " << end_parallel - start_parallel << endl << endl;

    //===================================================================================
    //===================================================================================
    //===================================================================================

    start_posled = omp_get_wtime();

    for (int i = 0; i < number; i++)
    {
        res[i] = a[i] + b[i];
    }

    end_posled = omp_get_wtime();

    cout << endl;
    cout << endl;
    cout << endl;
    cout << "time to work posled sum -> " << end_posled - start_posled << endl << endl;

    //===================================================================================

    start_posled = omp_get_wtime();

    for (int i = 0; i < number; i++)
    {
        res[i] = a[i] / b[i];
    }

    end_posled = omp_get_wtime();

    cout << "time to work posled dell -> " << end_posled - start_posled << endl << endl;

    //===================================================================================

    start_posled = omp_get_wtime();

    for (int i = 0; i < number; i++)
    {
        res[i] = a[i] * b[i];
    }

    end_posled = omp_get_wtime();

    cout << "time to work posled sum -> " << end_posled - start_posled << endl << endl;

    return 0;
}
