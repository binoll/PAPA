#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size, position, opt, i, j, data_len, buf_len = 20, root = 0;
    char *buff = NULL;
    char *recvbuff = NULL;
    int *int_data = NULL;
    double *double_data = NULL;

    while (((opt = getopt(argc, argv, "l:")) != -1)) {
        if (opt == 'l') {
            data_len = atoi(optarg);
            buf_len *= atoi(optarg);
        }
        else{
            fprintf(stderr, "Invalid arguments\n");
            return 1;
        }
    }
    int_data = malloc(data_len * sizeof(*int_data));
    double_data = malloc(data_len * sizeof(*double_data));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("send rank %d: |", rank);
    for (i = 0; i < data_len; ++i) {
        int_data[i] = rank + i;
        double_data[i] = rank + i;
        printf(" %d, %f |", int_data[i], double_data[i]);
    }
    puts("");

    buff = malloc(buf_len * sizeof(*buff));
    position = 0;
    MPI_Pack(int_data, data_len, MPI_INT, buff, buf_len, &position, MPI_COMM_WORLD);
    MPI_Pack(double_data, data_len, MPI_DOUBLE, buff, buf_len, &position, MPI_COMM_WORLD);

    if(rank == root)
        recvbuff = malloc(size * buf_len * sizeof(*recvbuff));

    MPI_Gather(buff, position, MPI_PACKED,
               recvbuff, position, MPI_PACKED,
               root, MPI_COMM_WORLD);

    if(rank == root) {
        sleep(1);
        position = 0;
        puts("");
        for (i = 0; i < size; ++i) {
            MPI_Unpack(recvbuff, buf_len * size, &position,
                       int_data,data_len, MPI_INT, MPI_COMM_WORLD);
            MPI_Unpack(recvbuff, buf_len * size, &position,
                       double_data, data_len, MPI_DOUBLE, MPI_COMM_WORLD);
            printf("in %d from %d: |", rank, i);
            for (j = 0; j < data_len; ++j) {
                printf(" %d, %f |", int_data[j], double_data[j]);
            }
            puts("");
        }
    }

    free(int_data);
    free(double_data);
    free(buff);
    free(recvbuff);
    MPI_Finalize();
}

