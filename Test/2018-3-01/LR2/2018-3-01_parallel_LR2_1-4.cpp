#include <stdio.h>
#include <cstdlib>
#include <algorithm>
#include "mpi.h"
using namespace std;

// MPI_Scatter с помощью неблокируемых операций (разные данные от одного ко всем)
int main(int argc, char** argv)
{
	int rank, size;
	int buf = 0;
	double start_time_0, end_time_0;
	double start_time_1, end_time_1;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса

	MPI_Request* reqs = new MPI_Request[size - 1];
	MPI_Status* stats = new MPI_Status[size - 1];

	// Один процесс отправляет
	start_time_0 = MPI_Wtime();
	if (rank == 0)
	{
		for (int i = 1; i < size; i++)
		{
			int buf = i * 2;
			MPI_Isend(&buf, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[i - 1]);
		}
		MPI_Waitall(size - 1, reqs, stats);
	}

	// Все остальные процессы получают
	else
	{
		int temp;
		MPI_Irecv(&temp, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &reqs[rank - 1]);
		MPI_Wait(&reqs[rank - 1], &stats[rank - 1]);
		printf("Proccess %d got message from rank0 %i\n", rank, temp);
	}
	end_time_0 = MPI_Wtime();

	if (rank == size - 1)
	{
		printf("\nMY program: %f\n", end_time_0 - start_time_0); // Вывод общего времени для моей программы
	}

	int* send_buf = new int[size];
	for (int i = 0; i < size; i++)
	{
		send_buf[i] = i * 2;
	}
	int recv_buf;
	
	start_time_1 = MPI_Wtime();
	MPI_Scatter(send_buf, 1, MPI_INT, &recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
	end_time_1 = MPI_Wtime();
	double time = end_time_1 - start_time_1;
	MPI_Isend(&time, 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD, &reqs[rank + size]);
	if (rank == 0)
	{
		double* total_time = new double[size];
		for (int i = 0; i < size; i++)
		{
			MPI_Irecv(&total_time[i], 1, MPI_DOUBLE, i, 5, MPI_COMM_WORLD, &reqs[i]);
			MPI_Wait(&reqs[i], &stats[i]);
			MPI_Wait(&reqs[rank + size], &stats[rank + size]);

		}
		printf("\n");
		// Вывод общего времени для программы с MPI_Scatter (используем функцию для поиска max элемента из массива времен)
		printf("HIS program: %f\n", *max_element(total_time, total_time + size)); 
	}

	printf("Process %d got message from root: %i\n", rank, recv_buf);

	MPI_Finalize();
}