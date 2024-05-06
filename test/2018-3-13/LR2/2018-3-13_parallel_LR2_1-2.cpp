#include <iostream>
#include <unistd.h>
#include "mpi.h"

void Bcast(const int count, MPI_Comm comm) {
    int rank, size, tag = 99;
    char* sendbuf;

    sendbuf = new char[count];

    for (int i = 0; i < count; i++) {
        sendbuf[i] = 'q';
    }

    char* recvbuf;
    recvbuf = new char[count];

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    MPI_Request requests;
    MPI_Status status;



    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Isend(sendbuf, count, MPI_CHAR, i, tag, comm, &requests);
        }
        
    }
    else {
        MPI_Irecv(recvbuf, count, MPI_CHAR, 0, tag, comm, &requests);
        MPI_Wait(&requests, &status);

    }
}


int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int  count, rank;
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

    start = MPI_Wtime();
    Bcast(count, MPI_COMM_WORLD);
    end = MPI_Wtime();
    printf("rank=%d bcast time: %f\n", rank, (end - start));

    char* sendbuf;

    sendbuf = new char[count];
    start = MPI_Wtime();
    for (int i = 0; i < count; i++) {
        sendbuf[i] = 'q';
    }
    //MPI_Bcast(sendbuf, strlen(sendbuf), MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(sendbuf, count, MPI_CHAR, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();
    printf("rank=%d MPI_Bcast time: %f\n", rank, (end - start));
    //printf("%.*s, %d\n", count, sendbuf, rank);

    MPI_Finalize();
}
