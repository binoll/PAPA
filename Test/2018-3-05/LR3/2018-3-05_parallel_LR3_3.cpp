#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "windows.h"
#define MSMPI_NO_DEPRECATE_20

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int myrank, process_count = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
 
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	srand(time(NULL));

	int bufsize = atoi(argv[1]);

	int* inbuf_i = new int[bufsize];
	for (int i = 0; i < bufsize; i++) inbuf_i[i] = myrank;

	char* inbuf_c = new char[bufsize];
	for (int i = 0; i < bufsize; i++)
	{
		inbuf_c[i] = char(int('a') + myrank);
	}

	char* outbuf_c = new char[bufsize * 10];

	int position = 0;

	if (myrank == 0)
	{
		MPI_Pack(inbuf_i, bufsize, MPI_INT, outbuf_c, bufsize * 10, &position, MPI_COMM_WORLD);
		MPI_Pack(inbuf_c, bufsize, MPI_CHAR, outbuf_c, bufsize * 10, &position, MPI_COMM_WORLD);
	}

	MPI_Bcast(outbuf_c, bufsize * 10, MPI_PACKED, 0, MPI_COMM_WORLD);

	if (myrank != 0)
	{
		position = 0;
		MPI_Unpack(outbuf_c, bufsize * 10, &position, inbuf_i, bufsize, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(outbuf_c, bufsize * 10, &position, inbuf_c, bufsize, MPI_CHAR, MPI_COMM_WORLD);
	}
	for (int i = 0; i < bufsize; i++) printf("process %d inbuf_i[%d]=%d inbuf_c[%d]=%c\n", myrank, i, inbuf_i[i], i, inbuf_c[i]);

	MPI_Finalize();

	return 0;

}
