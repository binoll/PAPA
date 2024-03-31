#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int i, j, k, opt, size, rank, len, root = 0;
    long sizeint;
    int* mat = NULL;
    int* mats = NULL;
    MPI_Datatype row, mat_type;

    while (((opt = getopt(argc, argv, "s:")) != -1)) {
        if (opt == 's')
            len = atoi(optarg);
        else{
            fprintf(stderr, "Invalid arguments\n");
            return 1;
        }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    mat = (int*)malloc(len * len * sizeof(int));
    mats = (int*)malloc(size * len * len * sizeof(int));

    srand(time(NULL) + rank);
    MPI_Type_vector(len, 1, len, MPI_INT, &row);
    MPI_Type_extent(MPI_INT, &sizeint);
    MPI_Type_hvector(len, 1, sizeint, row, &mat_type);
    MPI_Type_commit(&mat_type);

    printf("rank %d: \n", rank);
    for (i = 0; i < len; ++i){
        for (j = 0; j < len; ++j) {
            mat[i * len + j] = rand() % 10;
            printf("%4d ", mat[i * len + j]);
        }
        puts("");
    }

    MPI_Gather(mat, 1, mat_type,
               mats, len*len, MPI_INT,
               root, MPI_COMM_WORLD);

    if(rank == root){
        sleep(1);
        printf("Sum-matrix in rank %d:\n", rank);
        for (i = 0; i < len; ++i) {
            for (j = 0; j < len; ++j){
                mat[i * len + j] = 0;
                for (k = 0; k < size; ++k) {
                    mat[i * len + j] += mats[(k * len * len) + (i * len + j)];
                }
                printf("%4d ", mat[i * len + j]);
            }
            puts("");
        }
    }
    MPI_Type_free(&mat_type);
    free(mat);
    free(mats);
    MPI_Finalize();
}
