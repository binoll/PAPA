#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include "mpi.h"

using namespace std;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int count_val, rank, size, opt;

    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            sscanf(optarg, "%d", &count_val);
            break;

        default:
            fprintf(stderr, "Usage: %s\n",
                argv[0]);
            return 1;
        }
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int* sendbuf;
    sendbuf = new int[count_val];
    int* recvbuf;
    recvbuf = new int[count_val];
    int* count;
    count = new int[size];
    for (int i = 0; i < count_val; i++) sendbuf[i] = rank+12;

    ostringstream stream1;
    for (int i = 0; i < size; i++) stream1 << sendbuf[i] << " ";
    printf("rank=%d, sendbuf=%s\n", rank, stream1.str().c_str());

    for (int i = 0; i < size; i++) count[i] = 1;

    if (count_val > size)
    {
        int scat = count_val - size;
        count[0] = 1 + scat;
    }
    MPI_Reduce_scatter(sendbuf, recvbuf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    ostringstream stream2;
    for (int i = 0; i < count[rank]; i++) stream2 << recvbuf[i] << " ";
    printf("rank=%d, recieve=%s\n", rank, stream2.str().c_str());

    MPI_Finalize();
}