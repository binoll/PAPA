#include "mpi.h"
#include "stdio.h"
#include "string.h"

#define MAX_MSG_SIZE 1024

using namespace std;

int main(int argc, char *argv[])
{
	if (argc != 2) 
	{
		printf("Error in params!");
		return 0;
	}

	int rank;
	double starttime, endtime;
	int msg_size = strlen(argv[1]) + 1;
	char* msg = new char[msg_size];
	msg = argv[1];
	msg[msg_size - 1] = '\0';
	
	MPI_Init(&argc, &argv);
	MPI_Status status;
	
	char* sendbuff = new char[msg_size];
	char* recvbuff = new char[msg_size];
	
	// Получаем ранг текущего процессора
	if (MPI_Comm_rank( MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");	
	
	starttime = MPI_Wtime();
	if (rank == 0)
	{
		sendbuff = msg;
		MPI_Send(sendbuff, msg_size, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
	}
	if (rank == 1)
	{
		MPI_Recv(recvbuff, msg_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
		printf("\n Message from 0 rank process - %s\n", recvbuff);
		endtime = MPI_Wtime();
		printf("\nThat took %f seconds\n",endtime-starttime);
	}
	
	MPI_Finalize();
}