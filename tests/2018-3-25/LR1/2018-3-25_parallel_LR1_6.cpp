/*
С использованием  неблокирующих операций осуществить транспонирование квадратной
 матрицы, распределенной между процессорами по строкам.
 */

#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
#include <unistd.h>
#include <stdlib.h>
#define SIZE_ARRAY 1

    int main(int argc, char** argv) {
    int size_mtr;
    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
        case 'n':
            size_mtr = atoi(optarg);
            break;
        default:
            return 1;
        }
    }


    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != size_mtr) {
        printf("Incorrect n\n");
        return 1;
    }
    int i = 0;
    int* s_mtr = NULL;
    s_mtr = (int*)malloc(size_mtr * sizeof(int));
    while (i < size_mtr) {
        s_mtr[i] = rand() % 100 + rank;
        i++;
    }

    i = 0;
    while (i < size_mtr) {
        printf("matrix[%d][%d] = %d \n", rank, i, s_mtr[i]);
        i++;
    }
    int* s_mtr_t = NULL;
    s_mtr_t = (int*)malloc(size_mtr * sizeof(int)); // s_mtr_t - в нём результирующая строка


    MPI_Request* request = NULL;
    request = (MPI_Request*)malloc(2 * size_mtr * sizeof(MPI_Request));

    MPI_Status* status = NULL;
    status = (MPI_Status*)malloc(2 * size_mtr * sizeof(MPI_Status));


    i = 0;
    while (i < size_mtr) {
        if (rank == i) {
            s_mtr_t[i] = s_mtr[i];
            i++;
            continue;
        }
        int send_buf;
        send_buf = s_mtr[i];
        int tag = 0;
        MPI_Isend(&send_buf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request[i]);
        i++;
    }
    i = 0;
    while (i < size_mtr) {
        if (rank == i) {
            i++;
            continue;
        }
        int recv_buf = 0;
        int tag = 0;
        MPI_Irecv(&recv_buf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request[i + size_mtr]);
        MPI_Wait(&request[i + size_mtr], &status[i + size_mtr]);
        s_mtr_t[i] = recv_buf;
        i++;
    }
    i = 0;
    while (i < size_mtr) {
        printf("matrix_t[%d][%d] = %d \n", rank, i, s_mtr_t[i]);
        i++;
    }


    free(s_mtr);

    MPI_Finalize();
    return 0;
}
