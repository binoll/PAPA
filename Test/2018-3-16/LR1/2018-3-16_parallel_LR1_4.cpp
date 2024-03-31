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
    char* big_buffer;
    int size;
    int i;
    while ((opt = getopt(argc, argv, "b:t:")) != -1) {
        switch (opt) {
        case 'b':
            big_buffer = malloc(strlen(optarg));
            strcpy(big_buffer, optarg);
            size = strlen(optarg);

            break;
        case 't':
            size = atoi(optarg);
            big_buffer = malloc(size * sizeof(int));
            for (i = 0; i < size; i++) {
                big_buffer[i] = 'a';
            }
            break;
        default:
            fprintf(stderr, "Use flag -b to input message \n");
            exit(EXIT_FAILURE);
        }



    }
    printf("Size of message: %d \n", strlen(big_buffer));
    
    MPI_Init(&argc, &argv);
    MPI_Request request;
    MPI_Request request1;
    MPI_Status status;
    MPI_Status status1;
    int tag = 0;
    /*printf("%d \n", sizeof(small_buffer));*/
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    double starttime, endtime;
    starttime = MPI_Wtime();
    if (myrank == 0) {
        MPI_Isend(big_buffer, size, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &request);

    }
    else if (myrank == 1) {
        MPI_Irecv(big_buffer, size, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        endtime = MPI_Wtime();
        printf("Time in %d: %f \n", myrank, endtime - starttime);

    }



    MPI_Finalize();
}