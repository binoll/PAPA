#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <omp.h>

using namespace std;

int **createMatrix(int size)
{
	int** matr = new int*[size];
	for(int i = 0; i < size; ++i)
	{
		matr[i] = new int[size];
		for (int j = 0; j < size; ++j)
			matr[i][j] = i;
	}
	return matr;
}

void printMatrix(int **matrix, int size)
{
	for (int i = 0; i < size; ++i)
	{
		printf("\n");
		for (int j = 0; j < size; ++j)
			printf(" %d ", matrix[i][j]);
	}
	printf("\n");
}

void renewMatrix(int **matrix, int size)
{
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			matrix[i][j] = 0;
}

void Trans(int **A, int**C, int size)					//!!!!!!!!!!!!!
{
	for( int i = 0; i < size; ++i)
			for ( int j = 0; j < size; ++j)
				C[i][j] = A[j][i];
}

int* createVector(int size)
{
	int* vector = new int[size*size];
	for (int i = 0; i < size*size; ++i)
		vector[i] = i;
	return vector;
}

void printVector(int* vector, int size)
{
	printf("\n");
	for (int i = 0; i < size*size; ++i)
		printf(" %d ", vector[i]);
	printf("\n");
}

void renewVector(int* vector, int size)
{
	for (int i = 0; i < size*size; ++i)
		vector[i] = 0;
}

void vectorSum(int* A, int* B, int*C, int size)			//!!!!!!!!!!!!!
{
	for (int i = 0; i < size*size; ++i)
		C[i] = A[i] + B[i];
}

int main(int argc, char *argv[])
{
	int rank, opt, size, threads;

	while ((opt = getopt(argc, argv, "s:t:")) != -1) 
    {
        switch (opt) 
		{
        case 's':
            size = atoi(optarg);
            break;
		case 't':
            threads = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Использование: %s [-n process_id]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } 

	omp_set_num_threads(threads);
	double starttime, endtime;
	
	int** A = createMatrix(size);
	printf("\nMATRIX A:");
	printMatrix(A, size);
	
	int** B = createMatrix(size);
	printf("\nMATRIX B:");
	printMatrix(B, size);
	
	int** C = createMatrix(size);
	
	int i, j, k;
	
	//ПОЭЛЕМЕНТНОЕ ВЫЧИСЛЕНИЕ
	renewMatrix(C, size);
	starttime = omp_get_wtime();
	#pragma omp parallel shared(A,B,C) private(i,j)
	{
		#pragma omp for schedule(dynamic) private(i,j)
		for( i = 0; i < size; ++i)
			for ( j = 0; j < size; ++j)
				C[i][j] = A[i][j]*B[i][j];
	}
	endtime = omp_get_wtime();
	printf("\nMATRIX C after POELEMENTNO PARALLELNO:");
	printMatrix(C, size);
	printf("TIME: %f \n", endtime - starttime);
	
	//ПОСТОЛБЦОВОЕ ВЫЧИСЛЕНИЕ
	renewMatrix(C, size);
	starttime = omp_get_wtime();
	#pragma omp parallel shared(A,B,C) private(i,j,k)
	{
		#pragma omp for schedule(dynamic) private(i,j,k)
		for( i = 0; i < size; ++i)
			for ( j = 0; j < size; ++j)
				for ( k = 0; k < size; ++k)
					C[i][j] += A[i][k]*B[k][j];
	}
	endtime = omp_get_wtime();
	printf("\nMATRIX C after POSTOLBCOVO PARALLELNO:");
	printMatrix(C, size);
	printf("TIME: %f \n", endtime - starttime);
	
	//ТРАНСПОНИРОВАНИЕ ПОСЛЕДОВАТЕЛЬНО
	renewMatrix(C, size);
	starttime = omp_get_wtime();
	Trans(A,C,size);
	endtime = omp_get_wtime();
	printf("\nMATRIX C after TRANSPONIROVANIE POSLEDOVATELNO:");
	printMatrix(C, size);
	printf("TIME: %f \n", endtime - starttime);
	
	//ТРАНСПОНИРОВАНИЕ ПАРАЛЛЕЛЬНО
	renewMatrix(C, size);
	starttime = omp_get_wtime();
	#pragma omp parallel shared(A, B, C) private(i,j)
	{
		#pragma omp for schedule(dynamic) private(i,j)
		for( i = 0; i < size; ++i)
			for ( j = 0; j < size; ++j)
				C[i][j] = A[j][i];
	}
	endtime = omp_get_wtime();
	printf("\nMATRIX C after TRANSPONIROVANIE PARALLELNO:");
	printMatrix(C, size);
	printf("TIME: %f \n", endtime - starttime);
	
	int* vectorA = createVector(size);
	printf("\nVECTOR A:");
	printVector(vectorA, size);
	
	int* vectorB = createVector(size);
	printf("\nVECTOR B:");
	printVector(vectorB, size);
	
	int* vectorC = createVector(size);
	
	//СУММА ВЕКТОРОВ ПОСЛЕДОВАТЕЛЬНО
	renewVector(vectorC, size);
	starttime = omp_get_wtime();
	vectorSum(vectorA, vectorB, vectorC, size);
	endtime = omp_get_wtime();
	printf("\nVECTOR C after SUM POSLEDOVATELNO:");
	printVector(vectorC, size);
	printf("TIME: %f \n", endtime - starttime);
	
	//СУММА ВЕКТОРОВ ПAРАЛЛЕЛЬНО
	renewVector(vectorC, size);
	starttime = omp_get_wtime();
	#pragma omp parallel shared(vectorA, vectorB, vectorC) private(i)
	{
		#pragma omp for schedule(dynamic) private(i)
		for( i = 0; i < size*size; ++i)
			vectorC[i] = vectorA[i] + vectorB[i];
	}
	endtime = omp_get_wtime();
	printf("\nVECTOR C after SUM PARALLELNO:");
	printVector(vectorC, size);
	printf("TIME: %f \n", endtime - starttime);
	
	return 1;
}
