/*
����������� OpenMP-���������, ����������� ���������������� ����������
������� �� ������������ ���������� �������.
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
    int size, numthreads;
    char opt;

    while ((opt = getopt(argc, argv, "s:t:")) != -1)
    {
        switch (opt)
        {
        case 's':
            size = atoi(optarg);
            break;
        case 't':
            numthreads = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Error in arguments.\n");
            exit(1);
        }
    }

    int A[size][size];
    int B[size][size];

    int i, j, k, num, dd;

    double start_time, end_time;
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            A[i][j] = (i * 10) + j;
#pragma omp num_threads(numberthreads)

    std::cout << "MATRIX" << std::endl;
    for (i = 0; i < size; ++i)
    {
        for (j = 0; j < size; j++)
            std::cout << A[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    start_time = omp_get_wtime();
#pragma omp parallel for shared(A, B) private(i, j)
    for (i = 0; i < size; ++i)
#pragma omp parallel for shared(A, B) private(j)
        for (j = 0; j < size; ++j)
            B[i][j] = A[j][i];

    end_time = omp_get_wtime();

    std::cout << "MATRIX TRANSP" << std::endl;
    for (i = 0; i < size; ++i)
    {
        for (j = 0; j < size; j++)
            std::cout << B[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    printf("Time: %f \n", (end_time - start_time));

    return 0;
}
