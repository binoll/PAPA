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
    int **D = new int *[size];
    for (int i = 0; i < size; ++i)
    {
        A[i] = new int [size];
        B[i] = new int [size];
        C[i] = new int [size];
        D[i] = new int [size];
    }

    int i, j, k;
    double start, finish;

    for(i = 0; i < size; ++i)
    {
        for(j = 0; j < size; ++j)
        {
            A[i][j] = j;
            B[i][j] = j;
        }
    }
    
    printf("A \n");
    for (i = 0; i < size; ++i)
    {
        for(j = 0; j < size; ++j)
        {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    printf("B \n");
    for (i = 0; i < size; ++i)
    {
        for(j = 0; j < size; ++j)
        {
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    for (i = 0; i < size; ++i)
    {
        for(j = 0; j < size; ++j)
        {
            C[i][j] = 0;
        }
    }
    start = omp_get_wtime();
    #pragma omp parallel for shared(A,B,C) private(i,j,k) num_threads(num)
    for(i = 0; i < size; ++i)
	{
        #pragma omp parallel for shared(A,B,C) private(j,k) num_threads(num)
        for(j = 0; j < size; ++j)
		{			
            #pragma omp parallel for shared(A,B,C) private(k) num_threads(num)
            for(k = 0; k < size; ++k)
			{
                C[i][j] += A[i][k] * B[k][j];
			}
        }
	}
    
    printf("C \n");
    for (i = 0; i < size; ++i)
    {
        for(j = 0; j < size; ++j)
        {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    finish = omp_get_wtime();
    printf("Time of column multiplication of matrices %lf \n", finish - start);

    start = omp_get_wtime();
    #pragma omp parallel for shared(A,B,D) private(i,j) num_threads(num)
    for (i = 0; i < size; ++i)
	{
        #pragma omp parallel for shared(A,B,D) private(j) num_threads(num)
        for(j = 0; j < size; ++j)
		{
            D[i][j] = A[i][j] * B[i][j];
		}
	}
    /*
    printf("D \n");
    for (i = 0; i < size; ++i)
    {
        for(j = 0; j < size; ++j)
        {
            printf("%d ", D[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    */
    finish = omp_get_wtime();
    printf("Time of element multiplication of matrices %lf \n", finish - start);

    for (int i = 0; i < size; ++i)
    {
        delete [] A[i];
        delete [] B[i];
        delete [] C[i];
        delete [] D[i];
    }
    delete [] A;
    delete [] B;
    delete [] C;
    delete [] D;
}
