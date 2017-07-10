#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "helper.h"

void reverse(char *str, int strlen)
{
	// parallelize this function and make sure to call reverse_str()
	// on each processor to reverse the substring.
	
	int np, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int strngChunkLen=1+((strlen-1)/np);
    int NumRecieveData=strngChunkLen;

    char * receiveBuffer=(char *)calloc(sizeof(char),strngChunkLen); //receive from streaming array
    int * send_counts=(int *)calloc(sizeof(int), np); //streaming sendCounts
    int * sendDispl=(int *)calloc(sizeof(int), np);   //streaming displ

    int * recvcounts=(int *)calloc(sizeof(int), np); //gather counts
    int * receiveDispl=(int *)calloc(sizeof(int), np); //gather displ

    if (rank == 0)
    {
        NumRecieveData=strlen-strngChunkLen*(np-1);
        send_counts[0]=NumRecieveData;
        for(int i=1; i<np; i++){
            send_counts[i]=strngChunkLen;
        }

        sendDispl[1]=NumRecieveData;
        for(int i=2; i<np; i++){
            sendDispl[i]=sendDispl[i-1]+strngChunkLen;
        }
    }

    MPI_Scatterv(str, send_counts, sendDispl, MPI_CHAR, receiveBuffer, NumRecieveData, MPI_CHAR, 0, MPI_COMM_WORLD);

    printf("from process %i received:%s\n", rank, receiveBuffer);

    reverse_str(receiveBuffer, NumRecieveData);

    printf("from process %i reversed:%s\n", rank, receiveBuffer);

    if(rank==0){
        for(int i=0; i<np; i++){
            receiveDispl[np-1-i]=i*strngChunkLen;
        }
        //receiveDispl[0]=receiveDispl[1]+NumRecieveData+1;
        for(int i=0; i<np; i++){
            recvcounts[i]=strngChunkLen;
        }
        for(int i=0; i<np; i++){
            printf("receiveDispl[%i]=%i\n", i, receiveDispl[i]);
            printf("recvcounts[%i]=%i\n", i, recvcounts[i]);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(receiveBuffer, strngChunkLen, MPI_CHAR, str, recvcounts, receiveDispl, MPI_CHAR, 0, MPI_COMM_WORLD);

    /*if(rank==0){
        str[receiveDispl[0]+NumRecieveData]=0;
    }*/
}



