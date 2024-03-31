/*
����������� ���������������� � ������������ ���������� ��������� (� �������������� OpenMP),
��������������� ���������� �������������� �������� ��� ���������.
*/

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

    int A[size];
    int B[size];
    int C[size];

    int i;
    double start, finish;
	
    for(i = 0; i < size; ++i)
    {
        A[i] = i + 1;
        B[i] = 2;
        C[i] = 0;
    }
		
    start = omp_get_wtime();
    for(i = 0; i < size; i++)
    {	
        C[i] = A[i] * B[i];
    }
    finish = omp_get_wtime();

    printf("Vecror A: ");
    for (i = 0; i < size; i++)
    {
        printf("%d ", A[i]);
    }
    printf("\n");

    printf("Vecror B: ");
    for (i = 0; i < size; i++)
    {
        printf("%d ", B[i]);
    }
    printf("\n");
    
    printf("Vecror C: ");
    for (i = 0; i < size; i++)
    {
        printf("%d ", C[i]);
    }
    printf("\n");

    printf("Time sequence *: %lf \n", finish - start);

    for(i = 0; i < size; i++)
    {
        A[i] = i + 1;
        B[i] = 2;
        C[i] = 0;
    }
	
    start = omp_get_wtime();
    #pragma omp parallel for shared(A, B, C) private(i) num_threads(num)
    for(i = 0; i < size; ++i)
    {
        C[i] = A[i] * B[i];
    }
    finish = omp_get_wtime();
    printf("Time parallel *: %lf \n", finish - start);

    return 0;
}
