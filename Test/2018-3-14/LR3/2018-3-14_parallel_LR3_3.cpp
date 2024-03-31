
/*
    3. Разработать параллельную программу, демонстрирующую передачу элементов разного типа
        из нескольких областей памяти с использованием функций MPI_Pack и MPI_Unpack.
        Продемонстрировать передачу упакованных элементов с использованием операций коллективного взаимодействия.
    
    see ex. 3.40 from https://www.opennet.ru/docs/RUS/mpi-1/node66.html
*/

#include "lr3.h"

#define ROOT_RANK 0

int main(int argc, char* argv[])
{
    int rank, n, i, position, msglen;
    char *sendbuf, *message;
    int pack_size1, pack_size2, pack_size;
    FILE *fp;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char line[100];
    if ((fp = fopen("lr3-3.txt","r")) == NULL) { printf("File open error\n"); MPI_Exit(EXIT_FAILURE); }
    for (i = 0; i <= rank; i++) {
        if (fgets(line, sizeof(line), fp) == NULL) {
            sprintf(line, "\n");
            if (feof(fp) != 0) { /*printf("End of file\n");*/ break; }
            else { /*printf("Read file error\n");*/ break; }
        }
    }
    if (fclose(fp) == EOF) { printf("File close error\n"); MPI_Exit(EXIT_FAILURE); }

    // ********************************** MPI_Func **********************************

    // MPI_Pack(   const void* sendbuf , MPI_Count incount , MPI_Datatype datatype , void* concatbuf , MPI_Count outsize ,  MPI_Count* position ,   MPI_Comm comm);
    // MPI_Unpack( const void* sendbuf , MPI_Count insize ,  MPI_Count* position ,   void* concatbuf , MPI_Count outcount , MPI_Datatype datatype , MPI_Comm comm);

    message = line;
    msglen = strlen(message);
    printf("[%2d] (%2d) %s", rank, msglen, message);

    MPI_Pack_size(1,      MPI_INT,  MPI_COMM_WORLD, &pack_size1);
    MPI_Pack_size(msglen, MPI_CHAR, MPI_COMM_WORLD, &pack_size2);
    pack_size = pack_size1 + pack_size2;
    sendbuf = (char*)malloc(pack_size * sizeof(char));

    position = 0;
    MPI_Pack(&msglen,      1, MPI_INT,  sendbuf, pack_size, &position, MPI_COMM_WORLD);
    MPI_Pack(message, msglen, MPI_CHAR, sendbuf, pack_size, &position, MPI_COMM_WORLD);

    if (rank != ROOT_RANK) {
        MPI_Gather( &position,       1,    MPI_INT, NULL,    0,       MPI_INT, ROOT_RANK, MPI_COMM_WORLD); 
        MPI_Gatherv(sendbuf,  position, MPI_PACKED, NULL, NULL, NULL, MPI_INT, ROOT_RANK, MPI_COMM_WORLD); 
    }
    else {
        int total_pack_size, concat_pos;
        int pack_sizes[n], displs[n];
        char *recvbuf, *concatbuf;

        MPI_Gather(&position, 1, MPI_INT, pack_sizes, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD); 
        
        displs[0] = 0; 
        for (i = 1; i < n; i++) {
            displs[i] = displs[i - 1] + pack_sizes[i - 1];
            // printf("displs[%d]=%3d pack_sizes[%d]=%3d\n", i - 1, displs[i - 1], i - 1, pack_sizes[i - 1]);
        }
        total_pack_size = displs[n - 1] + pack_sizes[n - 1];
        // printf("displs[%d]=%3d pack_sizes[%d]=%3d\ntotal_pack_size=%3d\n", n - 1, displs[n - 1], n - 1, pack_sizes[n - 1], total_pack_size);
        recvbuf = (char*)malloc(total_pack_size * sizeof(char)); 
        concatbuf = (char*)malloc(total_pack_size * sizeof(char)); 
        MPI_Gatherv(sendbuf, position, MPI_PACKED, recvbuf, pack_sizes, displs, MPI_PACKED, ROOT_RANK, MPI_COMM_WORLD); 
        
        concat_pos = 0; 
        for (i = 0; i < n; i++) { 
            position = 0; 
            MPI_Unpack(&recvbuf[displs[i]], total_pack_size - displs[i], &position, &msglen,                     1, MPI_INT,  MPI_COMM_WORLD);
            MPI_Unpack(&recvbuf[displs[i]], total_pack_size - displs[i], &position, &concatbuf[concat_pos], msglen, MPI_CHAR, MPI_COMM_WORLD); 
            concat_pos += msglen;
            // printf("displs[%d]=%3d  pack_sizes[%d]=%3d  msglen=%3d  position=%3d  concat_pos=%3d\n", i, displs[i], i, pack_sizes[i], msglen, position, concat_pos);
        } 
        concatbuf[concat_pos] = '\0'; 

        printf("\n[%d] Messages:\n\n%s\n", rank, concatbuf);
    }

    // ********************************** MPI_Func **********************************

    MPI_Exit(EXIT_SUCCESS);
}