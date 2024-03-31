
/*
    1. С помощью неблокируемых операций точечного взаимодействия реализовать каждую
        из функций коллективного взаимодействия (MPI_Barrier, MPI_Bcast, MPI_Gather,
        MPI_Scatter, MPI_Allgather, MPI_Alltoall).
    2. Произвести экспериментальное сравнение времени выполнения разработанных функций
        с функциями коллективного взаимодействия для разного количества передаваемых данных.
   
        MPI_Barrier
        
        see https://translated.turbopages.org/proxy_u/en-ru.ru.b9d14310-636a3814-70619338-74722d776562/https/stackoverflow.com/questions/21626423/how-is-barrier-implemented-in-message-passing-systems
*/

#include "lr2-1.h"

#define ROOT_RANK 0

int main(int argc, char* argv[])
{
    int rank, n;
    double start, stop;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("[%d] Before Barrier\n", rank);

    start = MPI_Wtime();
    // ********************************** MPI_Barrier **********************************

    if (rank == ROOT_RANK) {
        int i;
        MPI_Status status_recv[n - 1], status_send[n - 1];
        MPI_Request requests_recv[n - 1], requests_send[n - 1];

        omp_set_num_threads(n - 1);
        #pragma omp parallel for ordered
        for (i = 0; i < n - 1; i++) {
            #pragma omp ordered
            MPI_Irecv(NULL, 0, MPI_BYTE, i + 1, 0, MPI_COMM_WORLD, &requests_recv[i]);
        }

        MPI_Waitall(n - 1, requests_recv, status_recv);

        #pragma omp parallel for ordered
        for (i = 0; i < n - 1; i++) {
            #pragma omp ordered
            MPI_Isend(NULL, 0, MPI_BYTE, i + 1, 1, MPI_COMM_WORLD, &requests_send[i]);
        }       

        MPI_Waitall(n - 1, requests_send, status_send);
    }
    else {
        MPI_Request request_recv, request_send;
        MPI_Isend(NULL, 0, MPI_BYTE, ROOT_RANK, 0, MPI_COMM_WORLD, &request_send);
        MPI_Wait(&request_send, MPI_STATUSES_IGNORE);

        MPI_Irecv(NULL, 0, MPI_BYTE, ROOT_RANK, 1, MPI_COMM_WORLD, &request_recv);
        MPI_Wait(&request_recv, MPI_STATUSES_IGNORE);
    }

    // ********************************** MPI_Barrier **********************************
    stop = MPI_Wtime();

    printf("[%d] After Barrier\n", rank);

    MPI_Wtime_avg_comparator(stop - start, n, rank, ROOT_RANK, NULL, 0, MPI_CHAR, NULL, 0, MPI_CHAR, MPI_COMM_WORLD, MPI_Barrier);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}