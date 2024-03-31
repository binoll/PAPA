#include <iostream>
#include <unistd.h>
#include "mpi.h"

void Gather(int count, MPI_Comm comm) {
    int rank, size, tag=99;
    char* sendbuf;
    //    err = getSizeData(argc, argv, &count);
    sendbuf = new char[count];
    for (int i = 0; i < count; i++) {
        sendbuf[i] = 'q';
    }
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    char* recvbuf;
    recvbuf = new char[count*size];

    MPI_Request requests;
    MPI_Status status;

    if (rank == 0) {
        MPI_Isend(sendbuf, count, MPI_CHAR, 0, tag, comm, &requests);
        for (int i = 0; i < size; i++) {
            MPI_Irecv(recvbuf + i * count, count, MPI_CHAR, i, tag, comm, &requests);
            MPI_Wait(&requests, &status);
//            printf("recvbuf my:%s\n", recvbuf);
        }

    }
    else {
        MPI_Isend(sendbuf, count, MPI_CHAR, 0, tag, comm, &requests);
        //printf("sendbuf:%s\n", sendbuf);
    }

}


int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int count, rank, size;
    double start, end;

    int opt;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
            case 's':
                sscanf(optarg, "%d", &count);
                break;

            default:
                fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
                        argv[0]);
                return 1;
        }
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    start = MPI_Wtime();
//    for (int i = 0; i < RREPEAD; ++i) {
    Gather(count, MPI_COMM_WORLD);
//    }
    end = MPI_Wtime();
    printf("rank = %d gather time: %f\n", rank, (end - start));

    char* sendbuf;
    char* recvbuf;

    sendbuf = new char[count];
    for (int i = 0; i < count; i++) {
        sendbuf[i] = 'q';
    }
    recvbuf= new char[count * size];
    start = MPI_Wtime();
//    for (int i = 0; i < RREPEAD; ++i) {

    MPI_Gather(sendbuf, count, MPI_CHAR, recvbuf, count, MPI_CHAR, 0, MPI_COMM_WORLD);
        //printf("%s\n", recvbuf);
//    }
    //MPI_Gather(sendbuf, strlen(sendbuf), MPI_CHAR, recvbuf, count, MPI_CHAR, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();
    printf("rank = %d MPI_Gather time: %f\n", rank, (end - start));
//    printf("%s, %d\n", recvbuf, rank);

    MPI_Finalize();
}
