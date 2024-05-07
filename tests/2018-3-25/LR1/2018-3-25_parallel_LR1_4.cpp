/*
Сравнить время реализации пересылок данных между двумя выделенными 
процессорами с блокировкой и без блокировки.
 */

#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
#include <unistd.h>
#include <stdlib.h>
#define MESSAGE_COUNT 1000

int main(int argc, char** argv) {
    int opt;
    int size_message;
    while((opt = getopt(argc, argv, "s:")) != -1){
	    switch(opt){
	      case 's':
	        size_message = atoi(optarg);
	        break;
	      default:
	        return 1;
	    }
    }
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 2){
      printf("Incorrect count processors: %d\n", size);
      return 2;
    }
    
    int i = 0;
    char* send_buf = NULL;
    send_buf = (char*)malloc(size_message * sizeof(char));
    while(i < size_message){
      send_buf[i] = 'a';
      i++;
    }
    
    char* recv_buf = NULL;
    recv_buf = (char*)malloc(size_message * sizeof(char));

    MPI_Status status;
    MPI_Request request[2*MESSAGE_COUNT];
    int tag = 0;

    double start_time_b, start_time_n_b, end_time_b, end_time_n_b;
    i = 0;
    if (rank == 0){
      
      // block message
      start_time_b = MPI_Wtime();
      while(i < MESSAGE_COUNT){
        MPI_Send(send_buf, size_message, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
        MPI_Recv(recv_buf, size_message, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);
        i++;
      }
      end_time_b = MPI_Wtime();
      printf("Time send with block: %f\n", (end_time_b - start_time_b)/1000);
      i = 0;
      

      // not block message
      start_time_n_b = MPI_Wtime();
      while (i < MESSAGE_COUNT){
        MPI_Isend(send_buf, size_message, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &request[i]);
        MPI_Irecv(recv_buf, size_message, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &request[i + 1000]);
        i++;
      }
      MPI_Status i_status[2 * MESSAGE_COUNT];
      end_time_n_b = MPI_Wtime();
      MPI_Waitall(2 * MESSAGE_COUNT, request, i_status);
      printf("Time send without block: %f\n", (end_time_n_b - start_time_n_b)/1000);
    }
    if (rank == 1){
      while (i < MESSAGE_COUNT){
        MPI_Recv(recv_buf, size_message, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Send(send_buf, size_message, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
        i++;
      }
      i = 0;
      while (i < MESSAGE_COUNT){
        MPI_Isend(send_buf, size_message, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &request[i]);
        MPI_Irecv(recv_buf, size_message, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &request[i + 1000]);
        i++;
      }
      MPI_Status i_status[2 * MESSAGE_COUNT];
      MPI_Waitall(2 * MESSAGE_COUNT, request, i_status);
    }
    MPI_Finalize();
    free(send_buf);
    free(recv_buf);
    return 0;
}
