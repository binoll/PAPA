#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STATIC_COUNT 3

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	int size,rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int i,j;
	int send_buf[STATIC_COUNT];
	int recv_buf[STATIC_COUNT];
	
	for(i=0;i<STATIC_COUNT;i++)
		send_buf[i]=rank*10+i;

	//MPI_Reduce
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
		printf("=-==--==-MPI_Reduce-==--==-\n");
	
	MPI_Reduce(send_buf,recv_buf,STATIC_COUNT,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	
	if (rank == 0)
	{
		for(i=0;i<STATIC_COUNT;i++)
			printf("%d ",recv_buf[i]);
		printf("\n");
	}
			
	
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
		printf("=-==--==-MPI_Allreduce-==--==-\n");
	//MPI_Allreduce
	for (i=0;i<STATIC_COUNT;i++)
		recv_buf[i] = 0;
	
	MPI_Allreduce(send_buf,recv_buf,STATIC_COUNT,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	
	for(i=0;i<STATIC_COUNT;i++)
	{
		if (i==0)
		printf("%d: ",rank);
		printf("%d ",recv_buf[i]);
	}
	printf("\n");
	
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
		printf("=-==--==-MPI_Reduce_scatter-==--==-\n");
	//MPI_Reduce_scatter
	for (i=0;i<STATIC_COUNT;i++)
		recv_buf[i] = 0;
		
	int count[3] = {1,1,1};
	MPI_Reduce_scatter(send_buf,recv_buf,count,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	
	printf("%d: ",rank);
	printf("%d\n",recv_buf[0]);
	
	
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
		printf("=-==--==-MPI_Scan-==--==-\n");
	//MPI_Scan
	for (i=0;i<STATIC_COUNT;i++)
		recv_buf[i] = 0;
		
	MPI_Scan(send_buf,recv_buf,STATIC_COUNT,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	
	for(i=0;i<STATIC_COUNT;i++)
	{
		if (i==0)
		printf("%d: ",rank);
		printf("%d ",recv_buf[i]);
	}
	printf("\n");
	
	MPI_Finalize();
}
