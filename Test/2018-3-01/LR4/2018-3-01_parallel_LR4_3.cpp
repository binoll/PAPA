#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <omp.h>

int main(int argc, char** argv)
{
	srand(time(NULL));
	int N;
	int NUM;
	int opt;
	
	while ((opt = getopt(argc, argv, "s:p:")) != -1)
	{
		switch(opt)
		{
			case 's':
			{
				N = atoi(optarg); // Кол-во строк в матрице A
				break;
			}
			case 'p':
			{
				NUM = atoi(optarg); // Кол-во столбцов в матрице B
				break;
			}
			default:
			fprintf(stderr, "Использование: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	// Создаем матрицы
	int** A = new int* [N];
	int** B = new int* [N];
	for (int s = 0; s < N; s++)
	{
		A[s] = new int[N];
		B[s] = new int[N];
	}
	int i, j, k;
	double start, end;

	// Заполнение случайными значениями
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			A[i][j] = rand() % 10;
		}
	}
	/*printf("\nMATRIX A:\n");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%i ", A[i][j]);
		}
		printf("\n");
	}*/

	start = omp_get_wtime(); // Начало отсчета
#pragma omp parallel for shared(A,B,N) private(i,j) num_threads(NUM)
// private - Своя локальная переменная в каждом потоке
// shared - Разделяемая (общая) переменная
	for (i = 0; i < N; i++)
	{
		#pragma omp parallel for private(j) num_threads(NUM)
		for (j = 0; j < N; j++)
		{
			B[j][i] = A[i][j];
		}
	}
	end = omp_get_wtime(); // Конец отсчета

	// Вывод итогового результата
	/*printf("\nResult:\n");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%i ", B[i][j]);
		}
		printf("\n");
	}*/
	printf("\nTime % lf\n", end - start);
	return 0;
}