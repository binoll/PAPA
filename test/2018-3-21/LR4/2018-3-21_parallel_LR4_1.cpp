#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <ctime>

using namespace std;



int main(int argc, char** argv) 
{
	double starttime, endtime, starttime1, endtime1;
	int opt, size_x, p, rank;
	while ((opt = getopt(argc, argv, "r:d:")) != -1) {
		switch (opt) {
		case 'r':
			size_x = atoi(optarg);
			break;
		case 'd':
			p = atoi(optarg);
			break;
		default:
			fprintf(stderr, "�������������: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	int** A = new int* [size_x];
	int** B = new int* [size_x];
	int** C = new int* [size_x];
	int** D = new int* [size_x];

	for (int i = 0; i < size_x; i++)
	{
		A[i] = new int[size_x];
		B[i] = new int[size_x];
		C[i] = new int[size_x];
		D[i] = new int[size_x];
	}
	for (int i = 0; i < size_x; i++)
	{
		for (int j = 0; j < size_x; j++)
		{
			C[i][j] = 0;
			A[i][j] = rand() % 10;
			B[i][j] = rand() % 10;
		}
	}
	if (size_x < 12)
	{
		cout << " Matrix A:\n";
		for (int i = 0; i < size_x; i++)
		{
			cout << "		";
			for (int j = 0; j < size_x; j++)
			{
				cout << A[i][j] << " ";
			}
			cout << "\n";
		}
		cout << " Matrix B:\n";
		for (int i = 0; i < size_x; i++)
		{
			cout << "		";
			for (int j = 0; j < size_x; j++)
			{
				cout << B[i][j] << " ";
			}
			cout << "\n";
		}
	}

	omp_set_num_threads(p);
	starttime = omp_get_wtime();
	#pragma omp parallel shared(A,B,C)
	{
		#pragma omp for
		for (int i = 0; i < size_x; i++)
		{
			for (int j = 0; j < size_x; j++)
			{
				for (int k = 0; k < size_x; k++)
				{
					C[i][j] += A[i][k] * B[k][j];
				}

			}
		}
	}
	endtime = omp_get_wtime();

	starttime1 = omp_get_wtime();
	#pragma omp parallel shared(A,B,D)
	{
		#pragma omp for
		for (int i = 0; i < size_x; i++)
		{
			for (int j = 0; j < size_x; j++)
			{
				D[i][j] = A[i][j] * B[i][j];
			}
		}
	}
	endtime1 = omp_get_wtime();


	cout << " Matrix Algebra Resault:\n";
	if (size_x < 12) {
		for (int i = 0; i < size_x; i++)
		{
			cout << "		";
			for (int j = 0; j < size_x; j++)
			{
				cout << C[i][j] << " ";
			}
			cout << "\n";
		}
	}
	cout << "time:		" << (endtime - starttime) << endl;
	
	cout << " Matrix NOT Algebra Resault:\n";
	if (size_x < 12)
	{
		for (int i = 0; i < size_x; i++)
		{
			cout << "		";
			for (int j = 0; j < size_x; j++)
			{
				cout << D[i][j] << " ";
			}
			cout << "\n";
		}
	}
	cout << "time:		" << (endtime1 - starttime1) / (double)CLOCKS_PER_SEC << endl;
}