#define MSMPI_NO_DEPRECATE_20
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "mpi.h"
#include <time.h>
#include <map>
#include <valarray>
#include <omp.h>
using namespace std;

void print_matr(int N, int** matr) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%d\t", matr[i][j]);
        }printf("\n");
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int opt, N, num_thr, f;
    while ((opt = getopt(argc, argv, "n:t:f:")) != -1) {
        switch (opt) {
        case 'n':
            N = atoi(optarg);
            break;
        case 't':
            num_thr = atoi(optarg);
            break;
        case 'f':
            f = atoi(optarg);
            break;
        default:
            fprintf(stderr, "������ ����������!\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    int** A = new int* [N];
    int** B = new int* [N];
    int** C = new int* [N];
    int** D = new int* [N];
    for (int i = 0; i < N; ++i) {
        A[i] = new int[N];
        B[i] = new int[N];
        C[i] = new int[N];
        D[i] = new int[N];
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = i + j + 1;
            B[i][j] = i + j + 2;
            C[i][j] = 0;
            D[i][j] = 0;
        }
    }

    omp_set_num_threads(num_thr);
    // ������������ ���������
    int i, j, k, size_;
    double start, end, start_2, end_2;
    start = omp_get_wtime();
    // �����������
#pragma omp parallel for shared(A, B, C) private(i, j, k)
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            for (k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    end = omp_get_wtime();
    /*print_matr(N, A);
    cout << "-------------------------------------\n";
    print_matr(N, B);
    cout << "-------------------------------------\n";*/
    start_2 = omp_get_wtime();
    //�����������
#pragma omp parallel for shared(A, B, D) private(i, j, k)
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            for (k = 0; k < N; ++k) {
                D[j][k] += A[j][i] * B[i][k];
            }
        }
    }
    end_2 = omp_get_wtime();
    if (f) {
        cout << "-------------------------------------\n";
        print_matr(N, D);
        cout << "-------------------------------------\n";
        print_matr(N, C);
    }
    cout << "\n-------------------------------------\n";
    cout << "N=" << N << "\tthread=" << num_thr << endl;
    cout << "elem = " << end - start << "   col = " << end_2 - start_2 << endl;
    cout << "-------------------------------------\n";
    for (int i = 0; i < N; i++) {
        delete[] A[i];
        delete[] B[i];
        delete[] C[i];
        delete[] D[i];
    }
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] D;
}
