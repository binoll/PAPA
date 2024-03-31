#include <iostream>
#include <unistd.h>
#include "mpi.h"

void Scatter(const int count, MPI_Comm comm) {
    int rank, size, tag = 99;

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    char* sendbuf;
    sendbuf = new char[count*size];

    for (int i = 0; i < count; i++) {
        sendbuf[i] = 'q';
    }

    char* recvbuf;
    recvbuf = new char[count];

    MPI_Request requests;
    MPI_Status status;

    if (rank == 0) {
        
        for (int i = 0; i < size; i++) {
            
//            printf("Send messege to rank%d: %s\n", i, sendbuf[0]);
            MPI_Isend(sendbuf, count, MPI_CHAR, i, tag, comm, &requests);
        }
        MPI_Irecv(recvbuf, count, MPI_CHAR, 0, tag, comm, &requests);
        MPI_Wait(&requests, &status);
//        printf("Messege from rank 0 to rank %d: %s\n", rank, recvbuf);
    }
    else {
        MPI_Irecv(recvbuf, count, MPI_CHAR, 0, tag, comm, &requests);
        MPI_Wait(&requests, &status);
        
//        printf("Messege from rank 0 to rank %d: %s\n", rank, recvbuf);
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
    Scatter(count, MPI_COMM_WORLD);
    end = MPI_Wtime();
    printf("rank = %d scatter time: %f\n", rank, (end - start));

    char* sendbuf;
    char* recvbuf;

    sendbuf = new char[count];
    for (int i = 0; i < count; i++) {
        sendbuf[i] = 'q';
    }
    recvbuf = new char[count];
    start = MPI_Wtime();
    MPI_Scatter(sendbuf, count, MPI_CHAR, recvbuf, count, MPI_CHAR, 0, MPI_COMM_WORLD);
    //MPI_Gather(sendbuf, strlen(sendbuf), MPI_CHAR, recvbuf, count, MPI_CHAR, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();
    printf("rank = %d MPI_Scatter time: %f\n", rank, (end - start));
    //printf("%s\n", recvbuf);

    MPI_Finalize();
}
