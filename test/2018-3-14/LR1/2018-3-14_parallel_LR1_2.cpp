/*
   2. Разработать параллельную программу, осуществляющую разделение
      коммуникатора из лекционного примера. Программа должна вывести
      ранг в старом коммуникаторе, число процессоров и ранг в новом
      коммуникаторе. Если процесс не попал в новый коммуникатор,
      должен сообщить об этом. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_N 1024

int main(int argc, char *argv[])
{
   int n, rank;
   int n_new, rank_new;
   int color, key, opt, i;
   char name;
   char* colors = (char*)malloc(MAX_N * sizeof(char));
   char* keys = (char*)malloc(MAX_N * sizeof(char));
   MPI_Comm MPI_new_comm;

   while ((opt = getopt(argc, argv, "c:k:")) != -1) {
      switch (opt) {
      case 'c':
         for (i = 0; i < strlen(optarg); i++) {
            colors[i] = optarg[i];
         }
         break;
      case 'k':
          for (i = 0; i < strlen(optarg); i++) {
              keys[i] = optarg[i];
          }
          break;
      default:
         fprintf(stderr, "Invalid parameters. Getopt return value: 0%o\n", opt);
         exit(EXIT_FAILURE);
      }
   }
   if (argc != 5) {
         fprintf(stderr, "Invalid parameters. Set option -c <colors>\n");
         exit(EXIT_FAILURE);
   }

   MPI_Init(&argc, &argv);

   MPI_Comm_size(MPI_COMM_WORLD, &n);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if (colors[rank] != 'U') {
      color = colors[rank] - '0';
   }
   else {
       color = MPI_UNDEFINED;
   }
   key = keys[rank] - '0';

   name = (char)(97 + rank);
  
   MPI_Comm_split(MPI_COMM_WORLD, color, key, &MPI_new_comm);

   if (MPI_new_comm  != MPI_COMM_NULL) {
      MPI_Comm_size(MPI_new_comm, &n_new);
      MPI_Comm_rank(MPI_new_comm, &rank_new);

      printf("Old: %d/%d (%c, %c, %c), New: %d/%d\n", rank, n, name, colors[rank], keys[rank], rank_new, n_new);
   }
   else {
      printf("Old: %d/%d (%c, %c, %c), [Leave]\n", rank, n, name, colors[rank], keys[rank]);
   }

   MPI_Finalize();

   exit(EXIT_SUCCESS);
}