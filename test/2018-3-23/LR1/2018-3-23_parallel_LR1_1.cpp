/*
*Разработать параллельную программу, использующую базовые конструкции MPI 
*(получение числа процессоров, своего ранга и имени вычислительного модуля). 
*Программа должна производить вывод информации в стандартный поток вывода, а сообщения об ошибках – 
*в стандартный поток ошибок.
*/
#include "mpi.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    int rank, name_len, n;
    char processor_name[30];
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &name_len);
    fprintf(stderr, "rank: %d proc: %s count: %d\n", rank, processor_name, n);
    printf("rank: %d proc: %s count: %d\n", rank, processor_name, n);
    
    MPI_Finalize();
    
    return 0;
}