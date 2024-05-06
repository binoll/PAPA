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

	// �������� ��� ��������� ���������
	int opt;
	int max_ = 700;
	int min_ = 100;
	int step_ = 100;
	
	std::ofstream out; // ����� ��� ������
	out.open("res_v"); // �������� ���� ��� ������
	// �������� �����������
	if (!out.is_open())
	{
		perror("������ �������� �����. ");
		return -1;
	}
	while ((opt = getopt(argc, argv, "l:m:s:")) != -1)
	{
		switch (opt)
		{
			// �������� l �������� �� ��������� ������ �������
		case 'l':
			min_ = atoi(optarg);
			break;
			// �������� m �������� �� �������� ������ �������
		case 'm':
			max_ = atoi(optarg);
			break;
			// �������� s �������� �� ��� ��������
		case 's':
			step_ = atoi(optarg);
			break;
		default:
			fprintf(stderr, "Unrecognized option!\n");
			break;
		}
	}

	// ���������� ������
	out << min_ << " " << max_ << " " << step_ << std::endl;
	// ������� ������� ����������
	omp_set_num_threads(4);
	// ---------------------------- Vector
	for (int size = min_; size <= max_; size += step_)
	{
		// ������� �������
		int A[size][size];
		int B[size][size];
		int C[size][size];
		// ��������
		for (i = 0; i < size; i++)
		{
			for (j = 0; j < size; j++)
			{
				A[i][j] = rand() % 10;
				B[i][j] = rand() % 10;
			}
		}

		// ������� �����
		gettimeofday(&tv1, &tz);
		#pragma omp parallel for shared(A,B,C, size) private(i, j, k)
		for (i = 0; i < size; i++)
		{
			#pragma omp parallel for private(j, k)
			for (j = 0; j < size; j++)
			{
				C[i][j] = 0;
				#pragma omp parallel for private(k)
				for (k = 0; k < size; k++)
					C[i][j] += A[i][k] * B[k][j];
			}
		}
		gettimeofday(&tv2, &tz);
		elapsed = (double)(tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) *
			1.e-6;
		printf("Vector. Size %d, time = %f seconds.\n", size, elapsed);
		// ���������� ������
		out << elapsed << " ";
	}
	return 0;
}
