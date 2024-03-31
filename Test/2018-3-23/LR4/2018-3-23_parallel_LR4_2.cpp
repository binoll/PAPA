#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

int main(int argc, char **argv)
{
    int size, num;
    char opt;

    while((opt = getopt(argc,argv, "s:t:")) != -1)
    {
        switch(opt)
        {
            case 's':
              size = atoi(optarg);
              break;
            case 't':
              num = atoi(optarg);
              break;
            default:
              fprintf(stderr,"Error in arguments.\n");
              exit(1);
        }
    }
	
    int **A = new int *[size];
    int **B = new int *[size];
    int **C = new int *[size];
    for (int i = 0; i < size; ++i)
    {
        A[i] = new int [size];
        B[i] = new int [size];
        C[i] = new int [size];
    }

    int i, j, k, dd;

    double start, finish;
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++)
        {
            A[i][j] = (i * 10) + j;
        }
    }
    /*
    printf("A\n");
    for (i = 0; i < size; ++i)
    {
        for(j = 0; j < size; ++j)
        {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    */
	
    start = omp_get_wtime();
    #pragma omp parallel for schedule(static) shared(A, B) private(i, j) num_threads(num)
    for(i = 0; i < size; ++i)
    {
        #pragma omp parallel for schedule(static) shared(A, B) private(j) num_threads(num)
        for(j = 0; j < size; ++j)	
        {
            B[i][j] = A[j][i];
        }
    }

    finish = omp_get_wtime();
	
    /*
    printf("A after transposition\n");
    for (i = 0; i < size; ++i)
    {
        for(j = 0; j < size; ++j)
        {
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    */
    printf("Time transponition of matrices %lf \n", finish - start);
    for (int i = 0; i < size; ++i)
    {
        delete [] A[i];
        delete [] B[i];
        delete [] C[i];
    }
    delete [] A;
    delete [] B;
    delete [] C;
}
