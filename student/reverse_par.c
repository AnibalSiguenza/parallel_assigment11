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

    int * processor_chunksize=(int *)calloc(np, sizeof(np));
    int * processor_displasment=(int *)calloc(np, sizeof(np));
    int * return_displasment=(int *)calloc(np, sizeof(np));

    int displacer=0;
    for (int i=0; i<np; i++){
        processor_chunksize[i]=strlen/np+((strlen%np)/(i+1)!=0);
        processor_displasment[i]=displacer;
        displacer+=processor_chunksize[i];
    }

    char * buffer=(char *)calloc(processor_chunksize[rank], sizeof(char));

    MPI_Scatterv(str , processor_chunksize, processor_displasment, MPI_CHAR, buffer, processor_chunksize[rank], MPI_CHAR, 0, MPI_COMM_WORLD);

    printf("from processor %i received data:%s\n", rank, buffer);

    reverse_str(buffer, processor_chunksize[rank]);

    printf("from processor %i reversed data:%s\n", rank, buffer);

    if(rank==0){
        displacer=0;
        for (int i=np-1; i>=0; i--){
            return_displasment[i]=displacer;
            displacer+=processor_chunksize[i];
        }
        printf("return_disp=%i %i %i %i %i %i %i ", return_displasment[0], return_displasment[1], return_displasment[2],
                return_displasment[3], return_displasment[4], return_displasment[5], return_displasment[6]);
    }
    MPI_Gatherv(buffer, processor_chunksize[rank], MPI_CHAR,  str, processor_chunksize, return_displasment, MPI_CHAR, 0, MPI_COMM_WORLD);
}
