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
    int** A = new int* [N];
    int** B = new int* [N];
    for (int i = 0; i < N; ++i) {
        A[i] = new int[N];
        B[i] = new int[N];
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = i + j + 1;
        }
    }
    //size_ = omp_get_num_threads();
    omp_set_num_threads(num_thr);
    // ������������ ���������
    int i, j;
    double start, end;
    //print_matr(N, A);
    start = MPI_Wtime();
    // �����������
#pragma omp parallel for shared(A, B) private(i, j)
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            B[i][j] = A[j][i];
        }
    }
    end = MPI_Wtime();
    cout << "\n-------------------------------------\n";
    cout << "N=" << N << "\tthread=" << num_thr << endl;
    cout << "time (transponir)=" << end - start << endl;
    cout << "-------------------------------------\n";
    for (int i = 0; i < N; i++) {
        delete[] A[i];
        delete[] B[i];
    }
    delete[] A;
    delete[] B;
}