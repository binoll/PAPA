#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
using namespace std;

void lab4_1_1(int num, int size)
{
    double start, end;
    int i, j;
    int **matrix1, **matrix2, **matrix_result;

    matrix1 = (int**)malloc(size * sizeof(int*));
    matrix2 = (int**)malloc(size * sizeof(int*));
    matrix_result = (int**)malloc(size * sizeof(int*));

    omp_set_num_threads(num);
    #pragma omp parallel for schedule(dynamic, 1) private(i, j)
    for ( i = 0; i < size; i++)
    {
        matrix1[i] = (int*)malloc(size * sizeof(int));
        matrix2[i] = (int*)malloc(size * sizeof(int));
        matrix_result[i] = (int*)malloc(size * sizeof(int));
        for (j = 0; j < size; j++)
        {
            matrix1[i][j] = rand() % 10;
            matrix2[i][j] = rand()%5;
        }
    }


    start = omp_get_wtime();

#pragma omp parallel for schedule(dynamic, 1) private(i, j)
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix_result[i][j] = matrix1[i][j] * matrix2[i][j];

    end = omp_get_wtime();
    //printf("time: %f \n", (end - start));
    /*for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            cout << matrix1[i][j] << " ";
        cout << endl;
    }
    cout << endl << endl;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            cout << matrix2[i][j] << " ";
        cout << endl;
    }
    cout << endl << endl;
    cout <<"time: "<< (end - start) << " " << endl;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            cout << matrix_result[i][j] << " ";
        cout << endl;
    }*/
    cout << endl << endl;
}


int main(int argc, char* argv[])
{
    int opt, count, num;
    while ((opt = getopt(argc, argv, "n:s:")) != -1) {
        switch (opt) {
            case 'n':
                sscanf(optarg, "%d", &num);
                break;
            case 's':
                sscanf(optarg, "%d", &count);
                break;
        }
    }
    lab4_1_1(num,count);
}