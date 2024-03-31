#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv)
{
	int i, j, k;
	struct timeval tv1, tv2;
	struct timezone tz;
	double elapsed;
	// Параметр для получения аргумента
	int opt;
	int max_ = 700;
	int min_ = 100;
	int step_ = 100;
	std::ofstream out; // поток для записи
	out.open("res_s"); // окрываем файл для записи
	// Проверка доступности
	if (!out.is_open())
	{
		perror("Ошибка открытия файла. ");
		return -1;
	}
	while ((opt = getopt(argc, argv, "l:m:s:")) != -1)
	{
		switch (opt)
		{
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
	// записываем строку
	out << min_ << " " << max_ << " " << step_ << std::endl;
	// Функция задания количества
	omp_set_num_threads(4);

	// ---------------------------- Elements
	for (int size = min_; size <= max_; size += step_)
	{
		// Объявим матрицу
		int A[size][size];
		int B[size][size];
		int C[size][size];
		// Заполним
		for (i = 0; i < size; i++)
			for (j = 0; j < size; j++)
			{
				A[i][j] = rand() % 10;
				B[i][j] = rand() % 10;
			}
		// Замерим время
		gettimeofday(&tv1, &tz);
#pragma omp parallel for shared(A,B,C, size) private(i, j)
		// private - Своя локальная переменная в каждом потоке
		// shared - Разделяемая (общая) переменная
		for (i = 0; i < size; i++)
		{
#pragma omp parallel for shared(A,B,C, size) private(j)
			for (j = 0; j < size; j++)
			{
				C[i][j] += A[i][j] * B[i][j];
			}
		}

		gettimeofday(&tv2, &tz);
		elapsed = (double)(tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) *
			1.e-6;
		printf("Elements. Size %d, time = %f seconds.\n", size, elapsed);
		// записываем строку
		out << elapsed << " ";
	}
	return 0;
}
