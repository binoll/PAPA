/*
    1. Разработать OpenMP-программу, выполняющую поэлементное и постолбцовое вычисление
        произведения квадратных матриц в разделяемой памяти. Сравнить время выполнения двух
        версий программ в зависимости от размера матрицы и количества потоков.
*/

#include "lr4.h"

int main(int argc, char* argv[])
{
    int num, size, count, sum, i, j, k;
    int ***Matrix;
    struct timespec start, end;
    double time;

    getopt_ns(argc, argv, &num, &size);

    omp_set_num_threads(num);

    count = 3;
    Matrix = (int***)malloc(count * sizeof(int**));

    init_matrix(count, size, size, Matrix);

    clock_gettime(CLOCK_REALTIME, &start);
    // ********************************** OpenMP_Func **********************************

    #pragma omp parallel for schedule(dynamic, 1) private(i, j, k, sum)
    for (i = 0; i < size; i++) {
        #pragma omp parallel for schedule(dynamic, 1) private(j, k, sum)
        for (j = 0; j < size; j++) {
            sum = 0; 
            #pragma omp parallel for schedule(dynamic, 1) private(k) reduction(+:sum)
            for (k = 0; k < size; k++) {
                sum += Matrix[0][i][k] * Matrix[1][k][j];
            }
            Matrix[2][i][j] = sum;
        }
    }

    // ********************************** OpenMP_Func **********************************
    clock_gettime(CLOCK_REALTIME, &end);

    time = (double)(end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;

    if (MAX_RPINT > size) {
        for (i = 0; i < count; i++) {
            printf("Matrix %c\n", i + 65); print_matrix(size, size, Matrix[i]);
        }
    }

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            sum = 0;
            for (k = 0; k < size; k++) {
                sum += Matrix[0][i][k] * Matrix[1][k][j];
            }
            if (Matrix[2][i][j] != sum) {
                printf("Error on position (%d %d)\n", i, j);
                exit(EXIT_FAILURE);
            }
        }
    }
    printf("%0.9f -> Success\n", time);

    OpenMP_time_comparator(num, size, time);

    free_matrix(count, size, Matrix);
    
    exit(EXIT_SUCCESS);
}
