/*
 *   2. Разработать параллельную программу, осуществляющую разделение коммуникатора из лекционного примера.
 *      Программа должна вывести ранг в старом коммуникаторе, число процессоров и ранг в новом коммуникаторе.
 *      Если процесс не попал в новый коммуникатор, должен сообщить об этом.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

int main( int argc, char *argv[])
{
    int n, rank, new_rank, new_n, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Random function for test comm-split
    int color = rank % 3;
    if (rank % 5 == 0)
    {
        color = MPI_UNDEFINED;
    }

    MPI_Comm MPI_New_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &MPI_New_comm);
    if (MPI_New_comm == MPI_COMM_NULL)
    {
        printf("Process with old rank %d is not in communicator\n", rank);
        return 0;
    }
    MPI_Comm_rank(MPI_New_comm, &new_rank);
    MPI_Comm_size(MPI_New_comm, &new_n);

    printf("Rank in old comm - %d [All - %d] ", rank, n);
    printf("Rank in new comm - %d [All in new comm - %d]\n", new_rank, new_n);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}
