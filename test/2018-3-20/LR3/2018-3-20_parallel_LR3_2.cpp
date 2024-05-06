#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

using namespace std;

int getSizeData(int argc, char** argv, int* sizeData)
{
    int code, count, err = MPI_SUCCESS;

    code = getopt(argc, argv, "n:");
    if (code == 'n') {
        count = sscanf(optarg, "%d", sizeData);
    }
    return err;
}

int getRandomNumber(int min, int max)
{
    int num = min + rand() % (max - min + 1);

    return num;
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int number, err;
    err = getSizeData(argc, argv, &number);

    int rank, procs;
    char proc_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Status* status;

    int a[number][number], b[number][number];
    long sizeofint;

    MPI_Datatype column, xpose;

    MPI_Type_extent(MPI_INT, &sizeofint);
    MPI_Type_vector(number, 1, number, MPI_INT, &column);

    MPI_Type_hvector(number, 1, sizeofint, column, &xpose);
    MPI_Type_commit(&xpose);

    if (rank == 0)
    {
        srand(time(NULL));

        for (int i = 0; i < number; i++) 
        {
            for (int j = 0; j < number; j++)
            {
                a[i][j] = getRandomNumber(0, 10);
            }
        }

        //MPI_Sendrecv(a, 1, xpose, 0, 0, b, 10*10, MPI_INT, 0, 0,
        //            MPI_COMM_WORLD, status);

        //*for (int i = 0; i < 10; i++)
        //{
        //    for (int j = 0; j < 10; j++)
        //    {
        //        printf("%d ", b[i][j]);
        //    }
        //    printf("\n");
        //}*/
    }

    MPI_Bcast(a, 1, xpose, 0, MPI_COMM_WORLD);

    printf("+++++++++++++++++++++ Rank %d +++++++++++++++++++++\n", rank);

    for (int i = 0; i < number; i++)
    {
        for (int j = 0; j < number; j++)
        {
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("\n");

    ////MPI_Barrier(MPI_COMM_WORLD);
    //
    //if (rank == 1)
    //{
    //    MPI_Recv(b, 10 * 10, MPI_INT, 0, 0, MPI_COMM_WORLD, status);

    //    for (int i = 0; i < 10; i++)
    //    {
    //        for (int j = 0; j < 10; j++)
    //        {
    //            printf("%d ", b[i][j]);
    //        }
    //        printf("\n");
    //    }
    //}

    MPI_Finalize();

}