#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;



int main(int argc, char** argv)
{
	double starttime, endtime, starttime1, endtime1;
	int opt, size_x,k, p, rank;
	while ((opt = getopt(argc, argv, "r:d:k:")) != -1) {
		switch (opt) {
		case 'r':
			size_x = atoi(optarg);
			break;
		case 'd':
			p = atoi(optarg);
			break;
		case 'k':
			k = atoi(optarg);
			break;
		default:
			fprintf(stderr, "�������������: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}


	int* A = new int[size_x];
	int* B = new int[size_x];
	int* C = new int[size_x];
	int* D = new int[size_x];

	for (int i = 0; i < size_x; i++)
	{
		A[i] = rand() % 10;
		B[i] = rand() % 10;
	}

	if (size_x < 12) {
		cout << " Vector A:\n";
		for (int i = 0; i < size_x; i++)
		{
			cout << A[i] << " ";
		}
		cout << "\n";

		cout << " Vector B:\n";
		for (int i = 0; i < size_x; i++)
		{
			cout << B[i] << " ";
		}
		cout << "\n";
	}
	omp_set_num_threads(p);
	starttime = omp_get_wtime();
	#pragma omp parallel shared(A,B,C,k)
	{
		#pragma omp for
		for (int i = 0; i < size_x; i++)
		{
			C[i] = A[i] + B[i];
			C[i] = C[i] * k;
		}
	}
	endtime = omp_get_wtime();

	starttime1 = omp_get_wtime();
	for (int i = 0; i < size_x; i++)
	{
		D[i] = A[i] + B[i];
		D[i] = D[i] * k;
	}
	endtime1 = omp_get_wtime();


	cout << " Vector C:\n";
	if (size_x<12) {
		for (int i = 0; i < size_x; i++)
		{
			cout << C[i] << " ";
		}
		cout << endl;
	}
	cout << "Time:	" << endtime - starttime << endl;
	cout << " Vector D:\n";
	if (size_x < 12) {
		for (int i = 0; i < size_x; i++)
		{
			cout << D[i] << " ";
		}
		cout << endl;
	}
	cout << "Time:	" << endtime1 - starttime1 << endl;
}
