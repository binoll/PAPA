#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int opt;
    int myrank;
    char* small_buffer;
    char* big_buffer;
    int size;
    while ((opt = getopt(argc, argv, "b:")) != -1) {
        switch (opt) {
        case 'b':
            big_buffer = malloc(strlen(optarg));
            small_buffer = malloc(strlen(optarg));
            size = strlen(optarg);
            strcpy(big_buffer, optarg);

            break;

        default:
            fprintf(stderr, "Use flag -b to input message \n");
            exit(EXIT_FAILURE);
        }



    }
    MPI_Init(&argc, &argv);
    MPI_Status status;
    int tag = 0;
    int proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);
    int destination, source;

    if (myrank == proc - 1)
        destination = 0;
    else
        destination = myrank + 1;

    if (myrank == 0)
        source = proc - 1;
    else
        source = myrank - 1;


    MPI_Sendrecv(big_buffer, size, MPI_CHAR, destination, 1, small_buffer, size, MPI_CHAR, source, 1, MPI_COMM_WORLD, &status);
    printf("rank=%d, recv from %d: '%s', send to %d: '%s'\n", myrank, source, small_buffer, destination, big_buffer);

    MPI_Finalize();
    return 0;


    MPI_Finalize();
}