#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mpi.h"

using namespace std;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int count, rank, opt, size;
    float matrix[count][count], matrix2[count][count];
    long sizeoffloat;

    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
        case 'n':
            sscanf(optarg, "%d", &count);
            break;

        default:
            fprintf(stderr, "Usage: %s\n",
                argv[0]);
            return 1;
        }
    }
    //MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Datatype column, xpose;
    
    MPI_Type_extent(MPI_FLOAT, &sizeoffloat);
    
    MPI_Type_vector(count, 1, count, MPI_FLOAT, &column);
    
    MPI_Type_hvector(count, 1, sizeoffloat, column, &xpose);
    
    MPI_Type_commit(&xpose);

    if (rank == 0)
    {
        srand(time(NULL));
        for (int i = 0; i < count; i++) for (int j = 0; j < count; j++) matrix[i][j] = rand();
    }

    MPI_Bcast(matrix, 1, xpose, 0, MPI_COMM_WORLD);
    //MPI_Sendrecv(matrix, 1, xpose, 1, 0, matrix2, count * count, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);

    printf("rank=%d, matrix: \n", rank);

    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < count; j++) printf("%f ", matrix[i][j]);
        printf("\n");
    }
    printf("\n");

    MPI_Finalize();
}
