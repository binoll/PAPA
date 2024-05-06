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

    int *A = new int[size];
    int *B = new int[size];
    int *C = new int[size];

    int i;
    double start, finish;
	
    for(i = 0; i < size; ++i)
    {
        A[i] = i + 1;
        B[i] = i + 1;
        C[i] = 0;
    }
		
    start = omp_get_wtime();
    for(i = 0; i < size; i++)
    {	
        C[i] = A[i] * B[i];
    }
    finish = omp_get_wtime();
    printf("Time consistent program %lf \n", finish - start);

    for(i = 0; i < size; i++)
    {
        A[i] = i + 1;
        B[i] = i + 1;
        C[i] = 0;
    }
	
    start = omp_get_wtime();
    #pragma omp parallel for private(i) num_threads(num)
    for(i = 0; i < size; ++i)
    {
        C[i] = A[i] * B[i];
    }
    finish = omp_get_wtime();
    printf("Time parallel program %lf \n", finish - start);

    delete [] A;
    delete [] B;
    delete [] C;
}
