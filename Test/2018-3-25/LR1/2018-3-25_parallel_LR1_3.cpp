/*
Смоделировать последовательный двусторонний обмен сообщениями между двумя процессами, 
замерить с помощью функции MPI_Wtime() время на одну итерацию обмена.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include "mpi.h"

#define SIZE_ARRAY 1024

int main(int argc, char* argv[])
{
    int opt;
    char* arg_message_first = NULL;
    char* arg_message_second = NULL;
    int i, j;
    while((opt = getopt(argc, argv, "f:s:")) != -1){
	switch(opt){
	case 'f':
	    i = 0;
		arg_message_first = (char*)malloc(strlen(optarg) * sizeof(char));
	    while(i <= strlen(optarg) && i < SIZE_ARRAY){
		arg_message_first[i] = optarg[i];
		i++;
	    }
	    break;
	case 's':
	    j = 0;
		arg_message_second = (char*)malloc(strlen(optarg) * sizeof(char));
	    while(j <= strlen(optarg) && j < SIZE_ARRAY){
		arg_message_second[j] = optarg[j];
		j++;
	    }
	    break;
	default:
	    exit(EXIT_FAILURE);
	}
    }
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char recvbuf[SIZE_ARRAY];
    double start_time, end_time;
    MPI_Status status;
    int tag = 0;
    if (rank == 0){
	start_time = MPI_Wtime();
	MPI_Send(arg_message_first, i, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
	MPI_Recv(recvbuf, j, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);
	end_time = MPI_Wtime();
	fprintf(stdout, "Rank 0: %s Time:%f\n", recvbuf, (end_time - start_time)/2);
    }
    
    else if(rank == 1){
	start_time = MPI_Wtime();
	MPI_Recv(recvbuf, i, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
	MPI_Send(arg_message_second, j, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
	end_time = MPI_Wtime();
	fprintf(stdout, "Rank 1: %s Time:%f\n", recvbuf, (end_time - start_time)/2);
    }
	free(arg_message_first);
	free(arg_message_second);
    MPI_Finalize();
}
