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
	double start, end;
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
	double* x = new double[N];
	double* y = new double[N];
	double* z = new double[N];
	int i = 0;
	for (i = 0; i < N; i++) x[i] = y[i] = i;
	start = omp_get_wtime();
#pragma omp parallel shared(x,y,z,N) private(i) num_threads(NUM)
	{
#pragma omp for private(i)
		for (i = 0; i < N; i++)
		{
			z[i] = x[i] + y[i];
		}
#pragma omp for private(i)
		for (i = 0; i < N; i++)
		{
			z[i] = x[i] - y[i];
		}
#pragma omp for private(i)
		for (i = 0; i < N; i++)
		{
			z[i] = x[i] * y[i];
		}
#pragma omp for private(i)
		for (i = 0; i < N; i++)
		{
			z[i] = x[i] / y[i];
		}
	}
	end = omp_get_wtime();
	printf("\nTime % lf\n", end - start);
	return 0;

}