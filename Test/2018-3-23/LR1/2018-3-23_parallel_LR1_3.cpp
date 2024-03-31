#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char* argv[])
{
    int opt, i, j, size_array;
    char arg_2 [size_array];
    char arg_3 [size_array];
    while((opt = getopt(argc, argv, "s:o:t:")) != -1)
    {
		switch(opt)
		{
		case 's':
			size_array = atoi(optarg);
			break;
		case 'o':
			i = 0;
			while(optarg[i] != '\0' && i < size_array)
			{
				arg_2[i] = optarg[i];
				i++;
			}
			break;
		case 't':
			j = 0;
			while(optarg[j] != '\0' && j < size_array)
			{
				arg_3[j] = optarg[j];
				j++;
			}
			break;
		default:
			fprintf(stderr, "Bad argument");
			exit(EXIT_FAILURE);
		}
    }

    MPI_Init(&argc, &argv);
    int rank;
    int tag = 0;
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char recv_message[size_array];
    if (rank == 0)
    {
		double start_time, end_time;
		start_time = MPI_Wtime();
		MPI_Send(arg_2, i, MPI_CHAR, 1, tag, MPI_COMM_WORLD);			
		MPI_Recv(recv_message, j, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);
		end_time = MPI_Wtime();
		printf("First message:%s from rank:%d len:%d Time:%f \n", arg_2, rank, size_array, (end_time - start_time)/2);
	}

    else if (rank == 1) 
    {
		double start_time, end_time;
		start_time = MPI_Wtime();
		MPI_Recv(recv_message, i, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);			
		MPI_Send(arg_3, j, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
		end_time = MPI_Wtime();
		printf("Second message:%s from rank:%d len:%d Time:%f \n", arg_3, rank, size_array, (end_time - start_time)/2);
    }

    MPI_Finalize();
    return 0;
}
