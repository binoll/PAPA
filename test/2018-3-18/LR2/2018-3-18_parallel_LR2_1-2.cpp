#include "newmpi.h"
#include "unistd.h"

int main(int argc, char* argv[]){
    int rank, size, opt, len;
    double start, finish;
    int* buffer = NULL;

    while (((opt = getopt(argc, argv, "c:")) != -1)) {
        if(opt == 'c')
            len = atoi(optarg);
        else{
            fprintf(stderr, "Invalid arguments");
            return 1;
        }
    }
    buffer = (int*)malloc(len * sizeof(int));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    start = MPI_Wtime();
    mpi_bcast(buffer, len, MPI_INT, 0, MPI_COMM_WORLD);
    finish = MPI_Wtime();
    printf("NewMPI rank %d: %f\n", rank, finish - start);

    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();
    MPI_Bcast(buffer, len, MPI_INT, 0, MPI_COMM_WORLD);
    finish = MPI_Wtime();
    printf("MPI rank %d: %f\n", rank, finish - start);

    free(buffer);
    MPI_Finalize();
}
