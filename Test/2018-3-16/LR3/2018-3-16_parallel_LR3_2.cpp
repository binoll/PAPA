#include <stdio.h>
#include "mpi.h"
#include <time.h> 
#include <getopt.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
    srand(time(NULL));
	int n, rank, opt, N;
    
    while ((opt = getopt(argc, argv, "n:")) != -1) 
    {
        switch (opt) 
        {
        case 'n':
            sscanf(optarg, "%d", &N);
            break;
        default:
            fprintf(stderr, "Invalid parameter. Set option -n <size>\n", argv[0]);
            return 0;
        }
    }
    if (argc != 3) 
    {
        fprintf(stderr, "Invalid parameter. Set option -n <size>\n");
        return 0;
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int** Matrix_A = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++)
    {
        Matrix_A[i] = (int*)malloc(N * sizeof(int));
    }

    //Заполнение матрицы
    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                Matrix_A[i][j] = rand() % 100;
            }
        }

        printf("Send rows\nrank:%d send matrix:\n", rank);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("|%2d| ", Matrix_A[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }

    MPI_Datatype ROW;
    MPI_Type_contiguous(N, MPI_INT, &ROW);
    MPI_Type_commit(&ROW);
 

    //Передадим матрицу по строчкам всем процессам
    for (int i = 0; i < N; i++)
    {
        MPI_Bcast(&Matrix_A[i][0], 1, ROW, 0, MPI_COMM_WORLD);
    }
    //Первый ранг выведет матрицу
    if (rank == 1)
    {
        printf("Send rows\nrank:%d recv matrix:\n", rank);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("|%2d| ", Matrix_A[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }
    MPI_Type_free(&ROW);



    //--------------------------------------------------------


    long int sizeofint;
    int Matrix_B[N][N];

    MPI_Datatype column, xpose;
    MPI_Type_extent(MPI_INT, &sizeofint);
    MPI_Type_vector(N, 1, N, MPI_INT, &column);
    MPI_Type_hvector(N, 1, sizeofint, column, &xpose);
    MPI_Type_commit(&xpose);

    //Заполнение матрицы
    srand(1);
    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                Matrix_B[i][j] = rand() % 100;
            }
        }

        printf("Send matrix\nrank:%d send matrix:\n", rank);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("|%2d| ", Matrix_B[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }


    MPI_Bcast(&Matrix_B, 1, xpose, 0, MPI_COMM_WORLD);
    //Первый ранг выведет матрицу
    if (rank == 1)
    {
        printf("Send matrix\nrank:%d recv matrix:\n", rank);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("|%2d| ", Matrix_B[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }
    MPI_Finalize();
    return 0;
}
