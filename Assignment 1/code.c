#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main( int argc, char *argv[])
{
    int myrank, size;
    double start_time, time, max_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    if(size!=2)
    {
        printf("Run the code for 2 processes. Exiting.\n");
        return 1;
    }

    //argv[1] is the size of data to be sent
    int BUFSIZE = (atoi(argv[1])/sizeof(int));

    int sendarr[BUFSIZE];
    MPI_Status status;
    int recvarr[BUFSIZE];

    //Tnitializing the array for the sending process.
    if(!myrank)
    {
        for(int i=0; i<BUFSIZE; i++)
        {
            sendarr[i]=i;
        }
    }

    //Sending from rank 0 to rank 1
    start_time = MPI_Wtime ();
    if (!myrank)
    {
        MPI_Send(sendarr, BUFSIZE, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(recvarr, BUFSIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    time = MPI_Wtime () - start_time;

    MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (!myrank)
    {
        printf ("%lf\n", max_time);
    }

    MPI_Finalize();
    return 0;
}