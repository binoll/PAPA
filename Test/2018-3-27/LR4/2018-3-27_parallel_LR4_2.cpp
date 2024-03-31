#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <omp.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#pragma comment(linker, "/STACK:46777216")

int main(int argc, char** argv)
{
	int i, j, k;
	struct timeval tv1, tv2;
	struct timezone tz;
	double elapsed;

	// Параметр для получения аргумента
	int opt;
	int max_ = 70;
	int min_ = 10;
	int step_ = 10;

	while ((opt = getopt(argc, argv, "l:m:s:")) != -1)
	{
		switch (opt) {
			// Параметр l отвечает за начальный размер матрицы
		case 'l':
			min_ = atoi(optarg);
			break;
			// Параметр m отвечает за конечный размер матрицы
		case 'm':
			max_ = atoi(optarg);
			break;
			// Параметр s отвечает за шаг подсчета
		case 's':
			step_ = atoi(optarg);
			break;
		default:
			fprintf(stderr, "Unrecognized option!\n");
			break;
		}
	}

	string file_name = "res";
	#if defined(_OPENMP)
		file_name = "res_omp";
	#endif

	if (file_name != "res")
	{
		cout << "OPENMP VERSION" << endl;
	}
	else
	{
		cout << "CLASSIC VERSION" << endl;
	}

	std::ofstream out; // поток для записи
	out.open(file_name); // окрываем файл для записи
	// Проверка доступности
	if (!out.is_open())
	{
		perror("Ошибка открытия файла. ");
		return -1;
	}
	// записываем строку
	out << min_ << " " << max_ << " " << step_ << std::endl;

	#pragma omp num_threads(4)
	for (int size = min_; size <= max_; size += step_)
	{
		// Объявим матрицу
		int A[size][size];
		int B[size][size];
		// Заполним
		for (i = 0; i < size; i++)
		{
			for (j = 0; j < size; j++)
			{
				A[i][j] = rand() % 10;
			}
		}

		// Вывод при маленьких значениях
		if (size < 10)
		{
			printf("\n");
			printf("Before: \n");
			for (i = 0; i < size; i++)
			{
				for (j = 0; j < size; j++)
				{
					printf("%d\t", A[i][j]);
				}
				printf("\n");
			}
		}

		// Замерим время
		gettimeofday(&tv1, &tz);
		int i, j;

#pragma omp parallel for private(i, j)
		for (i = 0; i < size; i += 1)
		{
#pragma omp parallel for private(j)
			for (j = 0; j < size; j += 1)
			{
				B[j][i] = A[i][j];
			}
		}
		gettimeofday(&tv2, &tz);
		elapsed = (double)(tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) *
			1.e-6;
		printf("Size %d, time = %f seconds.\n", size, elapsed);
		// записываем строку
		out << elapsed << " ";


		// Вывод при маленьких значениях
		if (size < 10)
		{
			printf("After: \n");
			for (i = 0; i < size; i++)
			{
				for (j = 0; j < size; j++)
				{
					printf("%d\t", B[i][j]);
				}
				printf("\n");
			}
			printf("\n\n");
		}
	}

	return 0;
}
