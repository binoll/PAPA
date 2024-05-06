#include <stdio.h>
#include <cstdlib>

#include "mpi.h"

using namespace std;


int main(int argc, char** argv)
{
    int rank, size;
    double start, stop;
    double times_c, times_m;
    // Инициализация программы, коммутатора
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        // Очистка файла, если начинаем считать заново
        if (size == 1)
        {
            FILE* file = fopen("res", "w");
            fclose(file);
        }
    }

    MPI_Status* status;
    MPI_Request* request;
    // ------------------ Barrier
    start = MPI_Wtime();
    status = new MPI_Status[size * 2];
    request = new MPI_Request[size * 2];

    for (int i = 0, j = 0; i < size; ++i) {
        MPI_Isend(NULL, 0, MPI_BYTE, i, 0, MPI_COMM_WORLD, &request[j++]);
        MPI_Irecv(NULL, 0, MPI_BYTE, i, 0, MPI_COMM_WORLD, &request[j++]);
    }

    MPI_Waitall(size * 2, request, status);
    stop = MPI_Wtime();
    // ------------------ Barrier
    times_c = stop - start;

    delete[] status;
    delete[] request;


    start = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    stop = MPI_Wtime();
    times_m = stop - start;

    if (rank == 0)
    {
        FILE* f = fopen("res", "a");
        if (f == NULL) { printf("failed to open file: permission issue ?\n"); exit(1); }
        fprintf(f, "%#f %#f\n", times_c, times_m);
        fclose(f);
    }
    // Завершение программы
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}