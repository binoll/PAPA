#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include "mpi.h"

using namespace std;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int* sendbuf;
    sendbuf = new int[size];

    int* recvbuf;
    recvbuf = new int[size];

    for (int i = 0; i < size; i++)
    {
        sendbuf[i] = rank;
    }

    ostringstream stream1;
    for (int i = 0; i < size; i++) {
        stream1 << sendbuf[i] << " ";
    }
    printf("rank=%d, sendbuf=%s\n", rank, stream1.str().c_str());

    MPI_Scan(sendbuf, recvbuf, size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    ostringstream stream2;
    for (int i = 0; i < size; i++) {
        stream2 << recvbuf[i] << " ";
    }
    printf("rank=%d, recieve=%s\n", rank, stream2.str().c_str());

    MPI_Finalize();

}