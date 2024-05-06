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

    int** matrix_transpose = new int* [matrix_size];
    for (int i = 0; i < matrix_size; i++) matrix_transpose[i] = new int[matrix_size];

    if (matrix_size < 10)
    {
        for (int i = 0; i < matrix_size; i++)
        {
            for (int j = 0; j < matrix_size; j++)
            {
                cout << matrix_a[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    int i, j;

    omp_set_num_threads(num_threads);

    double starttime_transpose = omp_get_wtime();

#pragma omp parallel private(i, j)
    {
#pragma omp for schedule(dynamic)
        for (i = 0; i < matrix_size; i++)
        {
            for (j = 0; j < matrix_size; j++) matrix_transpose[i][j] = matrix_a[j][i];
        }
    }

    cout << "Time for TRANSPOSE matrix is: " << omp_get_wtime() - starttime_transpose << endl;

    if (matrix_size < 10)
    {
        for (int i = 0; i < matrix_size; i++)
        {
            for (int j = 0; j < matrix_size; j++)
            {
                cout << matrix_transpose[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }


    return 0;
}