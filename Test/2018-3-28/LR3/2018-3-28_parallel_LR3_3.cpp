#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

    int rank;
    char recvbuf[50];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        int int_msg = 6;
        char char_msg = 'G';
        int position = 0;
        MPI_Pack(&int_msg, 1, MPI_INT, recvbuf, 50, &position, MPI_COMM_WORLD);
        MPI_Pack(&char_msg, 1, MPI_CHAR, recvbuf, 50, &position, MPI_COMM_WORLD);
        printf("Pack in rank %d. int_msg: %d, char_msg: %c\n", rank, int_msg, char_msg);
    }

    MPI_Bcast(recvbuf, 50, MPI_PACKED, 0, MPI_COMM_WORLD);

    int position = 0;
    int int_recv;
    char char_recv;
    MPI_Unpack(recvbuf, 50, &position, &int_recv, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(recvbuf, 50, &position, &char_recv, 1, MPI_CHAR, MPI_COMM_WORLD);
    printf("Unpack in rank %d. int_recv: %d, char_recv: %c\n", rank, int_recv, char_recv);

    MPI_Finalize();

    return 0;
}