#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    int size, num_threads;
    char opt;

    while((opt = getopt(argc,argv, "s:t:")) != -1)
    {
        switch(opt)
        {
            case 's':
            size = atoi(optarg);
            break;
            case 't':
            num_threads = atoi(optarg);
            break;
            default:
            fprintf(stderr,"Error in arguments.\n");
            exit(1);
        }
    }


    int **a = new int *[size];
    int **b = new int *[size];
    int **c = new int *[size];
    int **d = new int *[size];
    for (int i = 0; i < size; ++i)
    {
        a[i] = new int [size];
        b[i] = new int [size];
        c[i] = new int [size];
        d[i] = new int [size];
    }

    int i, j, k, sum;
    double start, finish;

    for(i=0; i<size; ++i)
    {
        for(j=0; j<size; ++j)
        {
            a[i][j]=j;
            b[i][j]=j;
        }
    }

    cout << "matrix a" << endl;
    for (i=0; i<size; ++i)
    {
        for(j=0; j<size; ++j)
            cout << a[i][j] << " ";
        cout << endl;
    }
    cout << endl;

    cout << "matrix b" << endl;
    for (i=0; i<size; ++i)
    {
        for(j=0; j<size; ++j)
            cout << b[i][j] << " ";
        cout << endl;
    }
    cout << endl;


    start = omp_get_wtime();
    #pragma omp parallel for num_threads(num_threads) shared(a, b, d) private(i, j)
    for (i = 0; i < size; ++i)
    {
        for (j = 0; j < size; ++j)
           d[i][j] = a[i][j] * b[i][j];
    }

  

    finish = omp_get_wtime();
    cout << "Elapsed time for elements is " << finish - start << endl;

    cout << "matrix d" << endl;
    for (i=0; i<size; ++i)
    {
        for(j=0; j<size; ++j)
            cout << d[i][j] << " ";
        cout << endl;
    }
    cout << endl;


    start = omp_get_wtime();
    #pragma omp parallel for shared(a,b,c) private(i,j,k) num_threads(num_threads)
    for(i=0; i<size; ++i)
        for(j=0; j<size; ++j)
            for(k=0; k<size; ++k) 
                c[i][j]+=a[i][k]*b[k][j];

    finish = omp_get_wtime();

    cout << "Elapsed time for columns is " << finish - start << endl;

    cout << "matrix c" << endl;
    for (i=0; i<size; ++i)
    {
        for(j=0; j<size; ++j)
            cout << c[i][j] << " ";
        cout << endl;
    }
    cout << endl;

    for (int i = 0; i < size; ++i)
    {
        delete [] a[i];
        delete [] b[i];
        delete [] c[i];
        delete [] d[i];
    }
    delete [] a;
    delete [] b;
    delete [] c;
    delete [] d;
}
