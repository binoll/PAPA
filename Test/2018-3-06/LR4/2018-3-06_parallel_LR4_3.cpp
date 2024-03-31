#define MSMPI_NO_DEPRECATE_20
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include "mpi.h"
#include <time.h>
#include <map>
#include <valarray>
#include <omp.h>
using namespace std;

void par(double* A, double* B, double* C, int N, int num) {
    int i;
    omp_set_num_threads(num);
    double start, end;
    start = omp_get_wtime();
#pragma omp parallel for schedule(static, N/num)
    for (i = 0; i < N; ++i)
        C[i] = A[i] + B[i];
    end = omp_get_wtime();
    cout << "+ = " << (end - start) * 1000 << " ms" << endl;

    start = omp_get_wtime();
#pragma omp parallel for schedule(static, N/num)
    for (i = 0; i < N; ++i)
        C[i] = A[i] * B[i];
    end = omp_get_wtime();
    cout << "* = " << (end - start) * 1000 << " ms" << endl;

#pragma omp parallel for schedule(static, N/num)
    for (i = 0; i < N; ++i)
        C[i] = A[i] / (B[i]);
    end = omp_get_wtime();
    cout << "/ = " << (end - start) * 1000 << " ms" << endl;
}

void pass(double* A, double* B, double* C, int N) {
    int i;
    double start, end;
    start = omp_get_wtime();
    for (i = 0; i < N; ++i)
        C[i] = A[i] + B[i];
    end = omp_get_wtime();
    cout << "+ = " << (end - start) * 1000 << " ms" << endl;

    start = omp_get_wtime();
    for (i = 0; i < N; ++i)
        C[i] = A[i] * B[i];
    end = omp_get_wtime();
    cout << "* = " << (end - start) * 1000 << " ms" << endl;

    for (i = 0; i < N; ++i)
        C[i] = A[i] / (B[i]);
    end = omp_get_wtime();
    cout << "/ = " << (end - start) * 1000 << " ms" << endl;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int opt, N, num_thr;
    while ((opt = getopt(argc, argv, "n:t:")) != -1) {
        switch (opt) {
        case 'n':
            N = atoi(optarg);
            break;
        case 't':
            num_thr = atoi(optarg);
            break;
        default:
            fprintf(stderr, "������ ����������!\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    double* A = new double[N];
    double* B = new double[N];
    double* C = new double[N];

    for (int i = 0; i < N; ++i) {
        A[i] = (i + 1) / 3;
        B[i] = (i + 1) / 3;
    }
    // ������������ ���������
    int i, j;
    double start, end;
    cout << "\n-------------------------------------\n";
    cout << "N=" << N << "\tthread=" << num_thr << endl;
    cout << "||" << endl;
    par(A, B, C, N, num_thr);
    cout << "-------------------------------------\n";
    cout << "---" << endl;
    pass(A, B, C, N);
    cout << "-------------------------------------\n";
    delete[] A, B, C;
}
