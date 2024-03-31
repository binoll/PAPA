#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>


int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	int nproc, rank, tag = 99;
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status[2 * (nproc - 1)];
	MPI_Request request[2 * (nproc - 1)];
	int send_buf[nproc];
	int recv_buf[nproc];
	int matrix[nproc][nproc];
	int i, j;
	for (i = 0; i < nproc; i++)
	{
		send_buf[i] = (nproc * rank) + i;
	}

	if (rank != 0) {
		MPI_Isend(send_buf, nproc, MPI_INT, 0, tag, MPI_COMM_WORLD, &request[rank - 1]);
	}
	j = 0;
	if (rank == 0)
	{
		for (i = 1; i < nproc; i++)
		{
			MPI_Irecv(matrix[i], nproc, MPI_INT, i, tag, MPI_COMM_WORLD, &request[j]);
			j += 1;

		}

		MPI_Waitall(nproc - 1, request, status);

		for (i = 0; i < nproc; i++)
			printf("%d\t", send_buf[i]);
		for (i = 1; i < nproc; i++)
		{
			printf(" \n");
			for (j = 0; j < nproc; j++)
				printf("%d\t", matrix[i][j]);
		}
		printf("\n\n");
	}


	j = 0;
	for (i = 0; i < nproc; i++)
		if (i != rank)
		{
			MPI_Isend(&send_buf[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request[j]);
			MPI_Irecv(&recv_buf[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request[nproc - 1 + j]);
			j += 1;
		}
	MPI_Waitall(2 * (nproc - 1), request, status);

	if (rank != 0) {
		recv_buf[rank] = send_buf[rank];
		MPI_Isend(recv_buf, nproc, MPI_INT, 0, tag, MPI_COMM_WORLD, &request[rank - 1]);
	}

	j = 0;
	if (rank == 0)
	{
		for (i = 1; i < nproc; i++)
		{
			MPI_Irecv(matrix[i], nproc, MPI_INT, i, tag, MPI_COMM_WORLD, &request[j]);
			j += 1;
		}

		MPI_Waitall(nproc - 1, request, status);
		printf("%d\t", send_buf[0]);
		for (i = 1; i < nproc; i++)
			printf("%d\t", recv_buf[i]);

		for (i = 1; i < nproc; i++)
		{
			printf(" \n");
			for (j = 0; j < nproc; j++)
				printf("%d\t", matrix[i][j]);
		}
		printf("\n\n");
	}

	MPI_Finalize();
}
