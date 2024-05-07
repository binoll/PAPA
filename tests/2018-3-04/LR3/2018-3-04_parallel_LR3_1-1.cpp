#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include "mpi.h"

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int rank, size;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int* sendbuf;
    sendbuf = new int[size];

    int* recvbuf;
    recvbuf = new int[size];

    for (int i = 0; i < size; i++) sendbuf[i] = rank+12;

    std::ostringstream stream;
    for (int i = 0; i < size; i++) stream << sendbuf[i] << " ";
    printf("rank=%d, sendbuf=%s\n", rank, stream.str().c_str());

    MPI_Reduce(sendbuf, recvbuf, size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        std::ostringstream stream;
        for (int i = 0; i < size; i++) stream << recvbuf[i] << " ";
        printf("rank=%d, recieve=%s\n", rank, stream.str().c_str());
    }
    MPI_Finalize();
}
