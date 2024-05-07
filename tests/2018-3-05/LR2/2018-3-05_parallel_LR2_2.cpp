#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int myrank, namelen, process_count = 0;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	//�������� ����� ���������
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	// �������� ���� �������� ����������    
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	// �������� ��� ��
	//MPI_Get_processor_name(processor_name, &namelen);

	int* matrix1_row = new int[process_count];
	for (int i = 0; i < process_count; i++)
	{
		matrix1_row[i] = myrank * process_count + i;
	}	

	int* matrix2_row = new int[process_count];
	for (int i = 0; i < process_count; i++)
	{
		matrix2_row[i] = myrank * process_count + i;
	}

	int* resultmatrix_row = new int[process_count];
	for (int i = 0; i < process_count; i++) resultmatrix_row[i] = 0;

	for (int i = 0; i < process_count; i++)
	{
		int* recvbuf_matrix1 = new int[process_count];

		MPI_Allgather(matrix2_row + i, 1, MPI_INT, recvbuf_matrix1, 1, MPI_INT, MPI_COMM_WORLD);

		//for (int j = 0; j < process_count; j++) printf("From process with rank %d process %d get element %d\n",j, myrank, recvbuf_matrix1[j]);

		for (int k = 0; k < process_count; k++)
		{
			resultmatrix_row[i] += matrix1_row[k] * recvbuf_matrix1[k];
		}

		delete recvbuf_matrix1;
	}

	printf("Result multiplication in row %d is: ", myrank);
	for (int i = 0; i < process_count; i++)
	{
		printf("%d ", resultmatrix_row[i]);
	}
	printf("\n");

	MPI_Finalize();

	return 0;

}
