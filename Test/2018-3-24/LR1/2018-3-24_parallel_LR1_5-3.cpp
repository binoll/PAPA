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
	
	// Получаем ранг текущего процессора
	if (MPI_Comm_rank( MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &allnum)) perror("Err at Comm_size :( \n");
	
	MPI_Request req[allnum];
	MPI_Status status;
	
	char **buff = new char*[allnum];
	for (int i = 0; i < allnum; ++i) 
		buff[i] = new char[msg_size];
	
	buff[rank] = msg1;
	buff[rank][msg_size-2] = numarr[rank];
	
	for (int i = 0; i < allnum; ++i) 
	{
		if (i == rank) continue;
		MPI_Isend(buff[rank], msg_size, MPI_CHAR, i, 1, MPI_COMM_WORLD, &req[i]);
		MPI_Recv(buff[i], msg_size, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
	}
	
	for (int i = 0; i < allnum; ++i)
	{
		printf("\n Messages in %d rank process: sended or recieved - %s\n", rank, buff[i]);
	}
	
	MPI_Finalize();
}