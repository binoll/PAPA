#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include "mpi.h"

using namespace std;

void Alltoall(int count, MPI_Comm comm) {
    int rank, size, tag=99;
    int* sendbuf;
    //    err = getSizeData(argc, argv, &count);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    sendbuf = new int[count*size];
//    for (int i = 0; i < count; i++) {
//        sendbuf[i] = 'q';
//    }
    for (int i = 0; i < count; ++i) {
        sendbuf[i] = (i + 1) * 10;
    }
    int* recvbuf;

    recvbuf = new int[count*size];



    std::ostringstream stream;
    for (int i = 0; i < size; ++i) {
        stream << sendbuf[i] << ", ";
    }
//    printf("RANK=%d; sendbuf=%s\n", rank, stream.str().c_str());

    MPI_Request* requests = new MPI_Request[size * 2];
    MPI_Status* status = new MPI_Status[size * 2];

    int j = 0;
    for (int i = 0; i < size; ++i) {
        MPI_Isend(sendbuf + i, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[j++]);
        MPI_Irecv(recvbuf + i, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[j++]);
    }
    MPI_Waitall(j, requests, status);

    ostringstream stream2;
    for (int i = 0; i < size; ++i) {
        stream2 << recvbuf[i] << ", ";
    }
//    printf("RANK=%d; recvbuf=%s\n", rank, stream2.str().c_str());
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
    Alltoall(count, MPI_COMM_WORLD);
    end = MPI_Wtime();
    printf("rank = %d alltoall time: %f\n", rank, (end - start));

    int* sendbuf;
    int* recvbuf;

    sendbuf = new int[count*size];
//    for (int i = 0; i < count; i++) {
//        sendbuf[i] = 'q';
//    }
    for (int i = 0; i < count; ++i) {
        sendbuf[i] = (i + 1) * 10;
    }
    recvbuf= new int[count*size];
    start = MPI_Wtime();
//    for (int i = 0; i < RREPEAD; ++i) {
    MPI_Alltoall(sendbuf, count, MPI_INT, recvbuf, count, MPI_INT, MPI_COMM_WORLD);
    //printf("%s\n", recvbuf);
//    }
    end = MPI_Wtime();
    printf("rank = %d MPI_Alltoall time: %f\n", rank, (end - start));
//    printf("%s, %d\n", recvbuf, rank);

    MPI_Finalize();
}