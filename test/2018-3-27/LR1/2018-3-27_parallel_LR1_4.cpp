#include <stdio.h>
#include "mpi.h"

#define BUFFSIZE 1024

int main(int argc, char * argv[])
{
	MPI_Init( &argc, &argv);
	int rank, size;
	int tag = 0;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status[BUFFSIZE];
	
	if (size != 2) 
	{
		fprintf(stderr, "NP must be equal to 2!\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	
	double starttime, endtime;
	char text_send[BUFFSIZE];
	char *text_recv;
	
	int i = 0;
	
	for (i; i < BUFFSIZE; i++) text_send[BUFFSIZE] = 'a';
	
	// Подсчет блокируемого приема/передачи
	starttime = MPI_Wtime();
	if (rank == 0)
	{
		int i = 0;
		while (i != 1000)
		{
			MPI_Send(text_send, BUFFSIZE, MPI_INT, 1, tag, MPI_COMM_WORLD);
			i++;
		}
	}
	else if (rank == 1)
	{
		int i = 0;
		while (i != 1000)
		{
			MPI_Recv(&text_recv, BUFFSIZE, MPI_INT, 0, tag, MPI_COMM_WORLD, &status[i]);
			i++;
		}
	}
	endtime = MPI_Wtime();
	if (rank==0) printf("Blocked communicate took %f seconds\n", (endtime - starttime) / 1000);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Request request;
	// Подсчет неблокируемого приема/передачи
	starttime = MPI_Wtime();
	if (rank == 0) 
	{
		int i = 0;
		while (i != 1000) 
		{
			MPI_Isend(text_send, BUFFSIZE, MPI_INT, 1, tag, MPI_COMM_WORLD, &request);
			i++;
		}
	} 
	else if (rank == 1) 
	{
		int i = 0;
		while (i != 1000) 
		{
			MPI_Irecv(&text_recv, BUFFSIZE, MPI_INT, 0, tag, MPI_COMM_WORLD, &request);
			i++;
		}
	}
	endtime = MPI_Wtime();
	if (rank==0) printf("Unblocked communicate took %f seconds\n", ( endtime - starttime ) / 1000);
	// Завершение программы
	MPI_Finalize();
}