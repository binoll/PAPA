#include <iostream>
#include <omp.h>
#include <cmath>


using namespace std;

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "rus");

    srand(time(NULL));

    int matrix_size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    int** matrix_a = new int* [matrix_size];
    for (int i = 0; i < matrix_size; i++) matrix_a[i] = new int[matrix_size];

    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++) matrix_a[i][j] = rand() % 20;
    }

    int** matrix_b = new int* [matrix_size];
    for (int i = 0; i < matrix_size; i++) matrix_b[i] = new int[matrix_size];

    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++) matrix_b[i][j] = rand() % 20;
    }

    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++)
        {
            cout << matrix_a[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++)
        {
            cout << matrix_b[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    int** matrix_piecemeal = new int* [matrix_size];
    for (int i = 0; i < matrix_size; i++) matrix_piecemeal[i] = new int[matrix_size];

    int** matrix_result = new int* [matrix_size];
    for (int i = 0; i < matrix_size; i++) matrix_result[i] = new int[matrix_size];

    int i, j, k;

    omp_set_num_threads(num_threads);

    double starttime_piecemeal = omp_get_wtime();

#pragma omp parallel private(i, j)
    {
#pragma omp for schedule(dynamic)
        for (i = 0; i < matrix_size; i++)
        {
            for (j = 0; j < matrix_size; j++) matrix_piecemeal[i][j] = matrix_a[i][j] * matrix_b[i][j];
        }
    }

    cout << "Time for PIECEMEAL multiplication is: " << omp_get_wtime() - starttime_piecemeal << endl;

    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++)
        {
            cout << matrix_piecemeal[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    double starttime_algebraic = omp_get_wtime();

#pragma omp parallel private(i, j, k)
    {
#pragma omp for schedule(dynamic)
        for (i = 0; i < matrix_size; i++)
        {
            for (j = 0; j < matrix_size; j++)
            {
                matrix_result[i][j] = 0;
                for (k = 0; k < matrix_size; k++) matrix_result[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }

    cout << "Time for ALGEBRAIC multiplication is: " << omp_get_wtime() - starttime_algebraic << endl;

    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++)
        {
            cout << matrix_result[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}
