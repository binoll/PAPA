#include <cstdlib>
#include <unistd.h> 
#include <vector>

#include "mpi.h"
using namespace std;

int main(int argc, char** argv)
{
    int rank, size, i, k;
    double start, stop;
    char* sendbuf, * recvbuf;

    // ѕараметр дл¤ получени¤ аргумента
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
            // ѕараметр l отвечает за начальный размер матрицы
        case 'l':
            min_ = atoi(optarg);
            break;
            // ѕараметр m отвечает за конечный размер матрицы
        case 'm':
            max_ = atoi(optarg);
            break;
            // ѕараметр s отвечает за шаг подсчета
        case 's':
            step_ = atoi(optarg);
            break;
            // ѕараметр i отвечает за количество подциклов
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

    // ћассивы дл¤ результата
    vector<double> res_c;
    vector<double> res_m;


    for (int len = min_; len <= max_; len += step_)
    {
        // ¬се выдел¤ют пам¤ть дл¤ приема
        recvbuf = (char*)malloc(size * len * sizeof(char));
        // ¬се выдел¤ют пам¤ть дл¤ записи и заполн¤ют ее
        sendbuf = (char*)malloc(size * len * sizeof(char));
        for (i = 0; i < len; i++)
        {
            sendbuf[i] = (char)(rank * i);
        }

        // Ћокальное врем¤ дл¤ суммы времени
        local_time = 0;

        // ÷икл дл¤ точной проверки
        for (int j = 0; j < inside_; j++)
        {
            MPI_Request requests[2 * size];
            start = MPI_Wtime();
            // --------------- AllGather
            for (i = 0, k = 0; i < size; i++) {
                MPI_Isend(&sendbuf[i * len], len, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[k++]);
                MPI_Irecv(&recvbuf[i * len], len, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[k++]);
            }

            MPI_Waitall(2 * size, requests, MPI_STATUSES_IGNORE);

            // ----------------- AllGather
            stop = MPI_Wtime();
            local_time += stop - start;
        }

        if (rank == 0)
        {
            res_c.push_back(local_time / inside_);
        }

        // —брасываем счетчик локального времени
        local_time = 0;
        for (int j = 0; j < inside_; j++)
        {
            // ----------------- Mpi
            start = MPI_Wtime();
            MPI_Alltoall(sendbuf, len, MPI_CHAR, recvbuf, len, MPI_CHAR, MPI_COMM_WORLD);
            stop = MPI_Wtime();
            local_time += stop - start;
            // ----------------- Mpi
        }
        if (rank == 0)
        {
            res_m.push_back(local_time / inside_);
        }

    }

    // «апись в файл
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