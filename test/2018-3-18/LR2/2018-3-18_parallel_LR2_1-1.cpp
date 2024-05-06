#include "newmpi.h"
#include "unistd.h"

int main(int argc, char* argv[]){
    int rank, size, opt, len;
    double start, finish;
    int* sendbuf = NULL;
    int* recvbuf = NULL;

    while (((opt = getopt(argc, argv, "c:")) != -1)) {
        if(opt == 'c')
            len = atoi(optarg);
        else{
            fprintf(stderr, "Invalid arguments");
            return 1;
        }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    sendbuf = (int*)malloc(len * sizeof(int));

    start = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    finish = MPI_Wtime();
    printf("MPI rank %d: %f\n", rank, finish - start);

    start = MPI_Wtime();
    mpi_barrier(MPI_COMM_WORLD);
    finish = MPI_Wtime();
    printf("NewMPI rank %d: %f\n", rank, finish - start);


    free(sendbuf);
    free(recvbuf);
    MPI_Finalize();
}



