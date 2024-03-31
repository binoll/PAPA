#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include "mpi.h"

using namespace std;

void AllGather(const int count, MPI_Comm comm) {
    int rank, procs, tag = 99, size;
    char *sendbuf;
    //    err = getSizeData(argc, argv, &count);
    sendbuf = new char[count];
    for (int i = 0; i < count; i++) {
        sendbuf[i] = 'q';
    }

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    char *recvbuf;
    recvbuf = new char[count * size];

    MPI_Request *request;
    MPI_Status *status;

    request = new MPI_Request[size * 2];
    status = new MPI_Status[size * 2];

//    printf("RANK=%d; sebdbuf=%d\n", rank, sendbuf);

    int j = 0;
    for (int i = 0; i < size; ++i) {

        MPI_Isend(&sendbuf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request[j++]);
        MPI_Irecv(recvbuf + i, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request[j++]);
    }
    MPI_Waitall(j, request, status);

    ostringstream stream;
    for (int i = 0; i < size; ++i) {
        stream << recvbuf[i] << ", ";
    }
//    printf("RANK=%d; recvbuf=%s\n", rank, stream.str().c_str());
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
    AllGather(count, MPI_COMM_WORLD);
    end = MPI_Wtime();
    printf("rank = %d allgather time: %f\n", rank, (end - start));

    char* sendbuf;
    char* recvbuf;

    sendbuf = new char[count];
    for (int i = 0; i < count; i++) {
        sendbuf[i] = 'q';
    }
    recvbuf= new char[count * size];
    start = MPI_Wtime();
//    for (int i = 0; i < RREPEAD; ++i) {

    MPI_Allgather(sendbuf, count, MPI_CHAR, recvbuf, count, MPI_CHAR, MPI_COMM_WORLD);
    //printf("%s\n", recvbuf);
//    }
    //MPI_Gather(sendbuf, strlen(sendbuf), MPI_CHAR, recvbuf, count, MPI_CHAR, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();
    printf("rank = %d MPI_Allather time: %f\n", rank, (end - start));
//    printf("%s, %d\n", recvbuf, rank);

    MPI_Finalize();
}