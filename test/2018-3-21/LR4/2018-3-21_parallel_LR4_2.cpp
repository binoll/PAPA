#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;



int main(int argc, char** argv)
{
	double starttime, endtime;
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

	for (int i = 0; i < size_x; i++)
	{
		A[i] = new int[size_x];
		B[i] = new int[size_x];
	}
	for (int i = 0; i < size_x; i++)
	{
		for (int j = 0; j < size_x; j++)
		{
			A[i][j] = rand() % 10;
		}
	}
	if (size_x<12) {
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
	}


	omp_set_num_threads(p);
	starttime = omp_get_wtime();
	#pragma omp parallel shared(A,B)
	{
		#pragma omp for
		for (int i = 0; i < size_x; i++)
		{
			for (int j = 0; j < size_x; j++)
			{
				B[i][j] = A[j][i];
			}
		}
	}
	endtime = omp_get_wtime();
	if (size_x<12) {
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
	cout << "Time:	" << endtime - starttime << endl;
}