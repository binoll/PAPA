#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <vector>
#include <stddef.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
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
	while ((opt = getopt(argc, argv, "l:m:s:")) != -1)
	{
		switch (opt)
		{
			// Параметр l отвечает за начальный размер массива
		case 'l':
			min_ = atoi(optarg);
			break;
			// Параметр m отвечает за конечный размер массива
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
	out << min_ << " " << max_ << " " << step_ << std::endl;// Объявляем тип массива

	// Максимальное ускорение при трех потоках
	int total_threads = 3;
	//Задание количества потоков

	#pragma omp parallel num_threads(total_threads)
	for (int size = min_; size <= max_; size += step_)
	{
		int* a = (int*)malloc(sizeof(int) * size);
		int* b = (int*)malloc(sizeof(int) * size);
		int* c = (int*)malloc(sizeof(int) * size);

		int i;

		// инициализируем массивы
		for (i = 0; i < size; i++)
		{
			a[i] = rand() % 100;
			b[i] = rand() % 100;
		}

		// Выведем, если маленькие размеры
		if (size < 10)
		{
			cout << endl;
			cout << "Show before: " << endl;
			for (int i = 0; i < 5; i++)
				cout << a[i] << " " << b[i] << endl;
			cout << endl;
		}
		int n_per_thread = (int)size / total_threads;
		int repeat = 10;

		gettimeofday(&tv1, &tz);
		// вычисляем сумму массивов


		for (int ri = 0; ri < repeat; ri++)
		{
			#pragma omp parallel for private(i) schedule(static, n_per_thread)
			for (i = 0; i < size; i++)
			{
				c[i] = a[i] + b[i];
			}
		}
		// Получаем время
		gettimeofday(&tv2, &tz);

		// Выводим в файл 
		#pragma omp single
		{
			elapsed = (double)(tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) *
				1.e-6;
			elapsed /= repeat;
			printf("Size %d, time = %f seconds.\n", size, elapsed);
			// записываем строку
			out << elapsed << " ";
			if (size < 10)
			{
				cout << "Show summing: " << endl;
				for (int i = 0; i < 10; i++)
					cout << c[i] << " ";
				cout << endl;
			}
		}

		// Очищаем во избежании переполнения памяти
		free(a);
		free(b);
		free(c);
	}
	return 0;
}
