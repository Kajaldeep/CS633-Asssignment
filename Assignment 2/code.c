#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

long long int min(long long int a, long long int b) // Function to find minimum of two long long int numbers
{
    if (a < b)
        return a;
    else
        return b;
}

long long int max(long long int a, long long int b) // Function to find maximum of two long long int numbers
{
    if (a > b)
        return a;
    else
        return b;
}

double fun(long long int px, long long int py, long long int n)
{
    long long int NUMBER_OF_ITERATIONS = 20; // We set the number of iterations to 20
    MPI_Status status;
    int myrank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double start_time, time, max_time;
    long long int myrow = myrank / px;                  // myrow is the row number of the process in the 2D grid
    long long int mycol = myrank % px;                  // mycol is the column number of the process in the 2D grid
    long long int realx = (n / px) * mycol;             // realx is the x coordinate of the first element of the process in the 2D grid
    long long int realy = (n / py) * myrow;             // realy is the y coordinate of the first element of the process in the 2D grid
    double **arr = malloc((n / py) * sizeof(double *)); // We allocate memory for the 2D array owned by the process in the following 7 lines
    arr[0] = malloc((n / py) * (n / px) * sizeof(double));
    for (int i = 0; i < (n / py); i++)
        arr[i] = &arr[0][i * (n / px)];
    for (long long int i = 0; i < n / py; i++)
        for (long long int j = 0; j < n / px; j++)
            arr[i][j] = (double)rand() / (double)RAND_MAX;             // We initialize the array with random double values
    long long int amount_to_send = min(max(realy - realx, 0), n / px); // amount_to_send is the number of elements to be sent to the process above
    if (myrow == 0)                                                    // If the process is in the first row, it does not send any elements
        amount_to_send = 0;
    long long int amount_to_recv = min(max(realy + (n / py) - realx, 0), n / px); // amount_to_recv is the number of elements to be received from the process below
    if (myrow == py - 1)                                                          // If the process is in the last row, it does not receive any elements
        amount_to_recv = 0;
    long long int send_to = myrank - px;   // send_to is the rank of the process to which the process sends elements
    long long int recv_from = myrank + px; // recv_from is the rank of the process from which the process receives elements
    double buff[amount_to_recv];           // buff is the buffer used to receive elements
    start_time = MPI_Wtime();              // start_time is the time at which the computation and communication starts
    for (long long int iter = 0; iter < NUMBER_OF_ITERATIONS; iter++)
    {
        if (myrow % 2 == 0) // We alternate the direction of communication, so that the processes do not deadlock. This is done by checking the parity of myrow
        {
            if (amount_to_send > 0 && myrow > 0) // Even rows send first if they have to send
                MPI_Send(&arr[0][0], amount_to_send, MPI_DOUBLE, send_to, iter, MPI_COMM_WORLD);
            if (amount_to_recv > 0 && myrow < py - 1) // Even rows receive second if they have to receive
                MPI_Recv(&buff[0], amount_to_recv, MPI_DOUBLE, recv_from, iter, MPI_COMM_WORLD, &status);
            for (long long int i = 0; i < n / py - 1; i++)
            {
                for (long long int j = 0; j < n / px; j++)
                {
                    if (realx + j <= realy + i)
                        arr[i][j] = arr[i][j] - arr[i + 1][j]; // We perform the computation in top-down fashion
                    else
                        break;
                }
            }
            for (long long int i = 0; i < amount_to_recv; i++) // We subtract the received elements from the last row
                arr[n / py - 1][i] = arr[n / py - 1][i] - buff[i];
        }
        else
        {
            if (amount_to_recv > 0 && myrow < py - 1) // Odd rows receive first if they have to receive
                MPI_Recv(&buff[0], amount_to_recv, MPI_DOUBLE, recv_from, iter, MPI_COMM_WORLD, &status);
            if (amount_to_send > 0 && myrow > 0) // Odd rows send second if they have to send
                MPI_Send(&arr[0][0], amount_to_send, MPI_DOUBLE, send_to, iter, MPI_COMM_WORLD);
            for (long long int i = 0; i < n / py - 1; i++) // We perform the computation in similar fashion as above
            {
                for (long long int j = 0; j < n / px; j++)
                {
                    if (realx + j <= realy + i)
                        arr[i][j] = arr[i][j] - arr[i + 1][j];
                    else
                        break;
                }
            }
            for (long long int i = 0; i < amount_to_recv; i++)
                arr[n / py - 1][i] = arr[n / py - 1][i] - buff[i];
        }
    }
    time = MPI_Wtime() - start_time;                                         // time is the time taken for the computation and communication
    MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); // We find the maximum time taken by any process
    free(arr[0]);                                                            // We free the memory allocated for the 2D array in the following 2 lines
    free(arr);
    return max_time; // We return the maximum time taken by any process
}

int main(int argc, char *argv[])
{
    double max_time;
    int myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    long long int px = atoi(argv[1]), py = atoi(argv[2]), n = atoi(argv[3]);
    max_time = fun(px, py, n); // We call the function with the given values of px, py and n
    if (myrank == 0)           // We print the maximum time taken by any process
        printf("px=%lld py=%lld n=%lld 2D Time=%lf\n", px, py, n, max_time);
    py = px * py;              // We set py to the total number of processes
    px = 1;                    // We set px to 1
    max_time = fun(px, py, n); // We call the function again with px=1, py=total number of processes and the same value of n
    if (myrank == 0)           // We print the maximum time taken by any process
        printf("px=%lld py=%lld n=%lld 1D Time=%lf\n", px, py, n, max_time);
    MPI_Finalize();
    return 0;
}