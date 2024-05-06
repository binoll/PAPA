#include "mpi.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
	int myrank, namelen, allnum;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	
	MPI_Init(&argc, &argv);
	
	// Получаем ранг текущего процессора
	if (MPI_Comm_rank( MPI_COMM_WORLD, &myrank)) perror("Err at Conn_rank :( \n");
	printf("My rank is %d\n",myrank);
	
	// Получаем имя ВМ
	if (MPI_Get_processor_name(processor_name, &namelen)) perror("Err at Get_processor_name :( \n");
	printf("My proc is %s\n", processor_name);
	
	if (myrank == 0)
	{
		if (MPI_Comm_size(MPI_COMM_WORLD, &allnum)) perror("Err at Comm_size :( \n");
		printf("Group size is %d\n",allnum);
	}
	
	MPI_Finalize();
}