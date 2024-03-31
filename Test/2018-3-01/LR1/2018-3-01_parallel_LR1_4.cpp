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
	
	double start_time_0_unblock, end_time_0_unblock;
	double start_time_1_unblock, end_time_1_unblock;
	
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
	int *recvbuf_3 = new int[x];
    int *recvbuf_4 = new int[z];
	printf("\n");
    MPI_Status status;
    MPI_Request request;
	MPI_Status stats[2]; // Init MPI_Status variable for unblock exchange
	MPI_Request reqs[4]; // Init MPI_Request variable
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0)
	{
		// BLOCK
		printf("BLOCK RANK0\n");
		start_time_0 = MPI_Wtime();
		MPI_Send(random_int_0, z, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(recvbuf_0, x, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
		end_time_0 = MPI_Wtime();
		fprintf(stdout, "That took for rank0 %f seconds:\n", end_time_0 - start_time_0);
		
		// UNBLOCK
		printf("UNBLOCK RANK0\n");
		start_time_0_unblock = MPI_Wtime();
		MPI_Isend(random_int_0, z, MPI_INT, 1, 0, MPI_COMM_WORLD, &reqs[0]);
		MPI_Irecv(recvbuf_3, x, MPI_INT, 1, 0, MPI_COMM_WORLD, &reqs[1]);
		MPI_Waitall(2, reqs, stats);
		end_time_0_unblock = MPI_Wtime();
		printf("That took for rank1 %f seconds\n\n", end_time_0_unblock - start_time_0_unblock);
    }
    else if (rank == 1) 
	{
		// BLOCK
		printf("BLOCK RANK1\n");
		start_time_1 = MPI_Wtime();
		MPI_Recv(recvbuf_1, z, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Send(random_int_1, x, MPI_INT, 0, 0, MPI_COMM_WORLD);
		end_time_1 = MPI_Wtime();
		fprintf(stdout, "That took for rank1 %f seconds:\n", end_time_1 - start_time_1);
		
		// UNBLOCK
		printf("UNBLOCK RANK1\n");
		start_time_1_unblock = MPI_Wtime();
		MPI_Isend(random_int_1, x, MPI_INT, 0, 0, MPI_COMM_WORLD, &reqs[0]);
		MPI_Irecv(recvbuf_4, z, MPI_INT, 0, 0, MPI_COMM_WORLD, &reqs[1]);
		MPI_Waitall(2, reqs, stats);
		end_time_1_unblock = MPI_Wtime();
		printf("That took for rank1 %f seconds\n\n", end_time_1_unblock - start_time_1_unblock);
    }
    MPI_Finalize();
}