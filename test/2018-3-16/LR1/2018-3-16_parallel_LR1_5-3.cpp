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
            strcpy(big_buffer, optarg);
            size = strlen(optarg);

            break;

        default:
            fprintf(stderr, "Use flag -b to input message \n");
            exit(EXIT_FAILURE);
        }

    }
    MPI_Init(&argc, &argv);
    int proc;
    int i = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);
    MPI_Status status;
    

    for (i = 0; i < proc; i++) {
        MPI_Sendrecv(big_buffer, size, MPI_CHAR, i, 1,
            small_buffer, size, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);
        printf("Rank %d: %s\n", myrank, small_buffer);
    }
    MPI_Finalize();
}

