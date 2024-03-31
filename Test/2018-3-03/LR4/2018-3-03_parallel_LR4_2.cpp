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
    for (int i = 0; i < size; ++i)
    {
        a[i] = new int [size];
        b[i] = new int [size];
        c[i] = new int [size];
    }

    int i, j, k, num, dd;

    double start, finish;
    for(i=0; i<size; i++)
        for(j=0; j<size; j++)
            a[i][j]=(i*10)+j;
 

    cout << "The matrix before transposition" << endl;
    for(i=0; i<size; ++i)
    {
        for(j=0; j<size; j++)
            cout << a[i][j] << " ";
	cout << endl;
    }

    start = omp_get_wtime();
    #pragma omp parallel for shared(a, b) private(i, j) num_threads(num_threads)
    for(i=0; i<size; ++i)
        #pragma omp parallel for shared(a, b) private( j) num_threads(num_threads)
        for(j=0; j<size; ++j)	
            b[i][j]=a[j][i];

    finish = omp_get_wtime();

    cout << "The matrix after transposition" << endl;
    for(i=0; i<size; ++i)
    {
        for(j=0; j<size; j++)
            cout << b[i][j] << " ";
	cout << endl;
    }

    cout << "Elapsed time for matrix transposition " << finish-start << endl;
    for (int i = 0; i < size; ++i)
    {
        delete [] a[i];
        delete [] b[i];
        delete [] c[i];
    }
    delete [] a;
    delete [] b;
    delete [] c;
}
