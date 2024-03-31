#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include "mpi.h"

#define RECV_BUFF_SIZE 128

using namespace std;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    int position = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    void* recvbuf[RECV_BUFF_SIZE];

    char* sendbuf;
    sendbuf = new char[size];

    int* sendbuf2;
    sendbuf2 = new int[size];

    if (rank == 0)
    {
        for (int i = 0; i < size; i++)
        {
            sendbuf[i] = '!';
            sendbuf2[i] = i*12;
        }
        
        MPI_Pack(sendbuf, size, MPI_CHAR, recvbuf, RECV_BUFF_SIZE, &position, MPI_COMM_WORLD);
        MPI_Pack(sendbuf2, size, MPI_INT, recvbuf, RECV_BUFF_SIZE, &position, MPI_COMM_WORLD);

        ostringstream stream1;
        for (int i = 0; i < size; i++) stream1 << sendbuf[i] << " ";
        printf("rank=%d, sendbuf_char=%s\n", rank, stream1.str().c_str());

        ostringstream stream2;
        for (int i = 0; i < size; i++) stream2 << sendbuf2[i] << " ";
        printf("rank=%d, sendbuf_int=%s\n", rank, stream2.str().c_str());
    }
    MPI_Bcast(recvbuf, RECV_BUFF_SIZE, MPI_PACKED, 0, MPI_COMM_WORLD);

    MPI_Unpack(recvbuf, RECV_BUFF_SIZE, &position, sendbuf, size, MPI_CHAR, MPI_COMM_WORLD);
    MPI_Unpack(recvbuf, RECV_BUFF_SIZE, &position, sendbuf2, size, MPI_INT, MPI_COMM_WORLD);

    if (rank != 0)
    {
        ostringstream stream3;
        for (int i = 0; i < size; i++) stream3 << sendbuf[i] << " ";
        printf("rank=%d, recieve_char=%s\n", rank, stream3.str().c_str());

        ostringstream stream4;
        for (int i = 0; i < size; i++) stream4 << sendbuf2[i] << " ";
        printf("rank=%d, recieve_int=%s\n", rank, stream4.str().c_str());
    }
    MPI_Finalize();
}