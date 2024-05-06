#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "mpi.h"
#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]){
    int rank, size;
    double start_time_0, end_time_0;
    double start_time_1, end_time_1;
    MPI_Init(&argc, &argv);
    int opt;
	char* sendbuf_0;
	char* sendbuf_1;
	int *random_int_0;
	int *random_int_1;
    int i = 0, y = 0, z = 0, x = 0; 
	int len_0, len_1;
	srand(time(NULL));
    while ((opt = getopt(argc, argv, "s:w:")) != -1){
	switch(opt){
		case 's':
		{
			len_0 = atoi(optarg);
			random_int_0 = new int[len_0];
			while (z < len_0){
				random_int_0[z] = 4;
				z++;
				}
			break;
		}
		case 'w':
		{
			len_1 = atoi(optarg);
			random_int_1 = new int[len_1];
			while (x < len_1){
				random_int_1[x] = 4;
				x++;
				}
			break;
		}
	    default:
		fprintf(stderr, "Использование: %s [-n process_id]\n", argv[0]);
		exit(EXIT_FAILURE);
		}
	}
    int *recvbuf_0 = new int[x];
    int *recvbuf_1 = new int[z];
	printf("\n");
    MPI_Status status;
    MPI_Request request;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0)
	{
		start_time_0 = MPI_Wtime();
		MPI_Send(random_int_0, z, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(recvbuf_0, x, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
		end_time_0 = MPI_Wtime();
		fprintf(stdout, "Message to %d: ", rank);
		for (int d = 0; d < x; d++)
		{
			printf("%i ", recvbuf_0[d]);
		}
		fprintf(stdout, "\nThat took for rank0 %f seconds: ", end_time_0 - start_time_0);
		fprintf(stdout, "\n");
    }
    else if (rank == 1) 
	{
		start_time_1 = MPI_Wtime();
		MPI_Recv(recvbuf_1, z, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Send(random_int_1, x, MPI_INT, 0, 0, MPI_COMM_WORLD);
		end_time_1 = MPI_Wtime();
		fprintf(stdout, "Message to %d: ", rank);
		for (int d = 0; d < z; d++)
		{
			printf("%i ", recvbuf_1[d]);
		}
		fprintf(stdout, "\nThat took for rank1 %f seconds: ", end_time_1 - start_time_1);
		fprintf(stdout, "\n");
    }
    MPI_Finalize();
}