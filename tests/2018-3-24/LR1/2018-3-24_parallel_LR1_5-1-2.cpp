#include "mpi.h"
#include "stdio.h"
#include "string.h"

#define MAX_MSG_SIZE 1024

using namespace std;

char numarr[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

int main(int argc, char *argv[])
{
	if (argc != 2) 
	{
		printf("Error in params!");
		return 0;
	}

	int rank, allnum;
	int msg_size = strlen(argv[1]) + 1;
	char* msg = new char[msg_size];
	msg = argv[1];
	msg[msg_size - 1] = '\0';
	
	MPI_Init(&argc, &argv);
	
	MPI_Status status;
	char* sendbuff = new char[msg_size];
	char* recvbuff = new char[msg_size];
	sendbuff = msg;
	
	// Получаем ранг текущего процессора
	if (MPI_Comm_rank( MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");	
	if (MPI_Comm_size(MPI_COMM_WORLD, &allnum)) perror("Err at Comm_size :( \n");
	sendbuff[msg_size - 2] = numarr[rank];
	
	if ((rank % 2 == 0) && (rank!=allnum-1))
	{
		MPI_Send(sendbuff, msg_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD);
		if (rank == 0) MPI_Recv(recvbuff, msg_size, MPI_CHAR, allnum - 1, 1, MPI_COMM_WORLD, &status);
		else MPI_Recv(recvbuff, msg_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &status);
	}
	else if (rank!=allnum-1)
	{
		MPI_Recv(recvbuff, msg_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &status);
		MPI_Send(sendbuff, msg_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Recv(recvbuff, msg_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &status);
		MPI_Send(sendbuff, msg_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
	} 
	
	printf("\n Messages in %d rank process: sended - %s, recieved - %s\n", rank, sendbuff, recvbuff);
	MPI_Finalize();
}