#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <iostream>
#include <cstdlib>

using namespace std;

void defineMatrix(int count, int n, int m, int*** matrix)
{
    int i, j, k;
#pragma omp parallel for schedule(dynamic, 1) private(i, j, k)
    for ( i = 0; i < count; i++)
    {
        matrix[i] = (int**)malloc(n * sizeof(int*));
        #pragma omp parallel for schedule(dynamic, 1) private(j, k)
        for (j = 0; j < n; j++)
        {
            matrix[i][j] = (int*)malloc(n * sizeof(int));
            #pragma omp parallel for schedule(dynamic, 1) private(k)
            for (k = 0; k < m; k++)
                matrix[i][j][k] = rand() % 10;
        }
    }
}

void lab4_1_1(int num, int size)
{
    int start, end;
    int i, j;
    int*** Matrix;

    omp_set_num_threads(num);
    Matrix = (int***)malloc(3 * sizeof(int**));

    defineMatrix(3, size, size, Matrix);

    start = clock();

    #pragma omp parallel for schedule(dynamic, 1) private(i, j)
    for (int i = 0; i < size; i++)
    {
        #pragma omp parallel for schedule(dynamic, 1) private(j)
        for (int j = 0; j < size; j++)
            Matrix[2][i][j] = Matrix[0][i][j] * Matrix[1][i][j];
    }
    end = clock();

    /*cout << (end - start) << " ";*/
    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
                cout << Matrix[k][i][j] << " ";
            cout << endl;
        }
        cout << endl << endl;
    }
}

void lab4_1_2(int num, int size)
{
    int i, j, k;
    int sum = 0;
    double start, end;
    int*** Matrix;

    omp_set_num_threads(num);
    Matrix = (int***)malloc(3 * sizeof(int**));

    defineMatrix(3, size, size, Matrix);

    #pragma omp parallel for schedule(dynamic, 1) private(i, j, k, sum)
    for (int i = 0; i < size; i++) {
        #pragma omp parallel for schedule(dynamic, 1) private(j, k, sum)
        for (int j = 0; j < size; j++) {
            sum = 0;
            #pragma omp parallel for schedule(dynamic, 1) private(k) reduction(+:sum)
            for (int k = 0; k < size; k++)
                sum += Matrix[0][i][k] * Matrix[1][k][j];
            Matrix[2][i][j] = sum;
        }
    }

    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
                cout << Matrix[k][i][j] << " ";
            cout << endl;
        }
        cout << endl << endl;
    }
}

void lab4_2(int num, int size)
{
    int i, j;
    double start, end;
    int*** Matrix; Matrix = (int***)malloc(2 * sizeof(int**));

    omp_set_num_threads(num);

    defineMatrix(2, size, size, Matrix);

    #pragma omp parallel for schedule(dynamic, 1) private(i, j)
    for (int i = 0; i < size; i++) {
        #pragma omp parallel for schedule(dynamic, 1) private(j)
        for (int j = 0; j < size; j++)
            Matrix[1][j][i] = Matrix[0][i][j];
    }

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
                cout << Matrix[i][j][k] << " ";
            cout << endl;
        }
        cout << endl << endl;
    }
}

void lab4_3(int num, int size)
{
    int i, j;
    double start, end;
    int*** Matrix; Matrix = (int***)malloc(3 * sizeof(int**));

    omp_set_num_threads(num);

    defineMatrix(3, size, size, Matrix);

    #pragma omp parallel for schedule(dynamic, 1) private(i, j)
    for (int i = 0; i < 1; i++) {
        #pragma omp parallel for schedule(dynamic, 1) private(j)
        for (int j = 0; j < size; j++) {
            Matrix[2][i][j] = Matrix[0][i][j] + Matrix[1][i][j];
        }
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
                cout << Matrix[i][j][k] << " ";
            cout << endl;
        }
        cout << endl << endl;
    }
}


int main()
{
    int size, num;
    num = 4;
    lab4_1_1(num, 5);
    
    //lab4_1_1(num, 5);
    //lab4_1_2(num, 5);
    //lab4_2(num, 5);
    //lab4_3(num, 5);
}