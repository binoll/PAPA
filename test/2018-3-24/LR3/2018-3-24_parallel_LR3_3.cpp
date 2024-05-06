#include <stdio.h>   
#include <mpi.h>     
#include <unistd.h>
#include <stdlib.h>

#define BUFFSIZE 50
#define INT_VALUE 65536
#define FLOAT_VALUE 0.5f

int main(int argc, char* argv[])
{

	int rank;
	
	MPI_Init(&argc, &argv);
	if (MPI_Comm_rank( MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");
	void* outbuff[BUFFSIZE];
	
	if (rank == 0)
	{
		int int_pck = INT_VALUE;
		float flt_pck = FLOAT_VALUE;
		char char_pck = '!';
		int size = sizeof(int) + sizeof(char) + sizeof(float);
		int position = 0;
		MPI_Pack(&int_pck, 1, MPI_INT, outbuff, BUFFSIZE, &position, MPI_COMM_WORLD);
		MPI_Pack(&flt_pck, 1, MPI_FLOAT, outbuff, BUFFSIZE, &position, MPI_COMM_WORLD);
		MPI_Pack(&char_pck, 1, MPI_CHAR, outbuff, BUFFSIZE, &position, MPI_COMM_WORLD);
		printf("Pack in rank %d. int_pck: %d, flt_pck: %f, char_pck: %c\n", rank, int_pck, flt_pck, char_pck);
	}
	
	MPI_Bcast(outbuff, BUFFSIZE, MPI_PACKED, 0, MPI_COMM_WORLD);

	int position = 0;
	int int_pck;
	float flt_pck;
	char char_pck;
	MPI_Unpack(outbuff, BUFFSIZE, &position, &int_pck, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(outbuff, BUFFSIZE, &position, &flt_pck, 1, MPI_FLOAT, MPI_COMM_WORLD);
	MPI_Unpack(outbuff, BUFFSIZE, &position, &char_pck, 1, MPI_CHAR, MPI_COMM_WORLD);
	printf("Unpack in rank %d. int_pck: %d, flt_pck: %f, char_pck: %c\n", rank, int_pck, flt_pck, char_pck);
	
	MPI_Finalize();
	
	return 0;
}