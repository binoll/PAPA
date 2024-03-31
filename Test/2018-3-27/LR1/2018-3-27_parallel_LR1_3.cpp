#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "mpi.h"

#define BUFFSIZE 1024

int main(int argc, char * argv[]) 
{
	int opt, size, rank, tag = 0;
	int i, j;
	// Задаем статические массивы максимальной длины
	char first_text[BUFFSIZE+1];
	char second_text[BUFFSIZE+1];
	
	while ((opt = getopt(argc, argv, "f:s:")) != -1) 
	{
		switch (opt) 
		{
			case 'f':
				snprintf(first_text,BUFFSIZE,"%s",optarg);
				break;
			case 's':
				snprintf(second_text,BUFFSIZE,"%s",optarg);
				break;
			default:
				fprintf(stderr, "Unrecognized option!\n");
		}
	}
	MPI_Init( & argc, & argv);
	MPI_Comm_rank(MPI_COMM_WORLD, & rank);
	MPI_Comm_size(MPI_COMM_WORLD, & size);
	
	if (size != 2) 
	{
		fprintf(stderr, "NP must be equal to 2!\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	if (rank == 0) 
	{
		char recvbuf[BUFFSIZE];
		double start_time, end_time;
		
		MPI_Status status;
		start_time = MPI_Wtime();
		MPI_Send(first_text, i, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
		end_time = MPI_Wtime();
		MPI_Recv(recvbuf, j, MPI_CHAR, 1, tag, MPI_COMM_WORLD, & status);
		fprintf(stdout, "first_text message: %s Time:%f\n", first_text, end_time - start_time);
	} 
	else if (rank == 1) 
	{
		char recvbuf[BUFFSIZE];
		double start_time, end_time;
		
		MPI_Status status;
		MPI_Recv(recvbuf, i, MPI_CHAR, 0, tag, MPI_COMM_WORLD, & status);
		start_time = MPI_Wtime();
		MPI_Send( & second_text, j, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
		end_time = MPI_Wtime();
		fprintf(stdout, "Second message: %s Time:%f\n", second_text, end_time - start_time);
	}
	MPI_Finalize();
}
