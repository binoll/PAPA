/*
–еализовать при помощи отправки сообщений типа "точка-точка"
следующие схемы коммуникации процессов: ѕересылка данных
от каждого процесса каждому
 */

#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
#include <unistd.h>
#include <stdlib.h>
#define SIZE_ARRAY 1

int main(int argc, char** argv) {



    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int tag = 0;
    MPI_Request request;
    MPI_Status status;
    int buf;
    int resualt = 0;
    
    int i = 0;
    while (i < size) {
        MPI_Isend(&rank, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request);
        printf("%d send rank to %d\n", rank, i);
        i++;
    }

    i = 0;
    while (i < size) {
        MPI_Irecv(&buf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        printf("%d recv buf %d from %d\n", rank, buf, i);
        i++;
    }
    




    MPI_Finalize();
    return 0;
}