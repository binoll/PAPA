#include <cstdlib>
#include <unistd.h> 
#include <vector>

#include "mpi.h"
using namespace std;

int main(int argc, char** argv)
{
    int rank, size, i;
    double start, stop;
    char* sendbuf, * recvbuf;

    // Параметр для получения аргумента
    int opt;

    int max_ = 700;
    int min_ = 100;
    int step_ = 100;
    int inside_ = 10;
    float local_time = 0;
    while ((opt = getopt(argc, argv, "l:m:s:i:")) != -1)
    {
        switch (opt)
        {
            // Параметр l отвечает за начальный размер матрицы
        case 'l':
            min_ = atoi(optarg);
            break;
            // Параметр m отвечает за конечный размер матрицы
        case 'm':
            max_ = atoi(optarg);
            break;
            // Параметр s отвечает за шаг подсчета
        case 's':
            step_ = atoi(optarg);
            break;
            // Параметр i отвечает за количество подциклов
        case 'i':
            inside_ = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Invalid parameters. Value: 0%o\n", opt);
            break;
        }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Массивы для результата
    vector<double> res_c;
    vector<double> res_m;


    for (int len = min_; len <= max_; len += step_)
    {
        sendbuf = (char*)malloc(len * sizeof(char));

        // 0 ранг выделяет память
        if (rank == 0) {
            recvbuf = (char*)malloc(size * len * sizeof(char));
        }

        // Request для отправки

        for (i = 0; i < len; i++)
        {
            sendbuf[i] = (char)(rank * i);
        }

        local_time = 0;
        for (int j = 0; j < inside_; j++)
        {
            MPI_Request request;
            start = MPI_Wtime();
            // --------------- Gather
            MPI_Isend(sendbuf, len, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);

            if (rank == 0) {
                MPI_Status status[size];
                MPI_Request requests[size];

                for (i = 0; i < size; i++) {
                    MPI_Irecv(&recvbuf[i * len], len, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[i]);
                }

                MPI_Waitall(size, requests, status);
            }
            MPI_Wait(&request, MPI_STATUSES_IGNORE);
            // ----------------- Gather
            stop = MPI_Wtime();
            local_time += stop - start;
        }

        if (rank == 0)
        {
            res_c.push_back(local_time / inside_);
        }

        // Сбрасываем счетчик локального времени
        local_time = 0;
        for (int j = 0; j < inside_; j++)
        {
            // ----------------- Mpi
            start = MPI_Wtime();
            MPI_Gather(sendbuf, len, MPI_CHAR, recvbuf, len, MPI_CHAR, 0, MPI_COMM_WORLD);
            stop = MPI_Wtime();
            local_time += stop - start;
            // ----------------- Mpi
        }
        if (rank == 0)
        {
            res_m.push_back(local_time / inside_);
        }

    }

    // Запись в файл
    if (rank == 0) {
        FILE* f = fopen("res", "w+");
        //if(f==NULL){printf("failed to open file: permission issue ?\n");exit(1);}
        for (int i = 0; i < res_c.size(); ++i)
        {
            fprintf(f, "%#f ", res_c[i]);
        }
        fprintf(f, "\n");
        for (int i = 0; i < res_m.size(); ++i)
        {
            fprintf(f, "%#f ", res_m[i]);
        }
        fclose(f);
    }

    if (rank == 0) {
        FILE* f = fopen("steps", "w+");
        if (f == NULL) { printf("failed to open file: permission issue ?\n"); exit(1); }
        fprintf(f, "%d %d %d", min_, max_, step_);
        fclose(f);
    }

    MPI_Finalize();
    exit(EXIT_SUCCESS);
}