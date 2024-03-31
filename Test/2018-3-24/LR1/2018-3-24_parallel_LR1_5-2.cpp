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
	char* msg1 = new char[msg_size];
	msg1 = argv[1];
	msg1[msg_size - 1] = '\0';

	MPI_Init(&argc, &argv);
	
	MPI_Request req;
	MPI_Status status;
	char* sendbuff = new char[msg_size];
	
	// Получаем ранг текущего процессора
	if (MPI_Comm_rank( MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &allnum)) perror("Err at Comm_size :( \n");
	char** recvbuff;
	
	if (rank == 0)
	{
		recvbuff = new char*[allnum - 1];
		for (int i = 0; i < allnum - 1; ++i) recvbuff[i] = new char[msg_size];
	
		sendbuff = msg1;
		for (int i = 1; i < allnum; ++i)
		{
			MPI_Send(sendbuff, msg_size, MPI_CHAR, i, 1, MPI_COMM_WORLD);
		}
		for (int i = 0; i < allnum - 1; ++i)
		{
			MPI_Recv(recvbuff[i], msg_size, MPI_CHAR, i+1, 1, MPI_COMM_WORLD,  &status);
		}
	}
	else
	{
		MPI_Recv(sendbuff, msg_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
		printf("\n Message in %d rank process: recieved - %s\n", rank, sendbuff);
		sendbuff[msg_size - 2] = numarr[rank];
		MPI_Send(sendbuff, msg_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
		printf("\n From %d rank process:sended - %s\n", rank, sendbuff);
	}
	
	if (rank == 0)
	{
		for (int i = 0; i < allnum - 1; ++i)
		{
			printf("\n Messages in %d rank process: sended - %s, recieved - %s\n", rank, sendbuff, recvbuff[i]);
		}
	}
	
	MPI_Finalize();
}