#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // For getpid()

#define MIN 0
#define MAX 20
#define STEPS 10 // Number of steps the walker takes

int main(int argc, char *argv[])
{
    int rank, size;
    int walker_pos = -1; // Walker's position (-1 means inactive)

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Better random seeding using getpid() to introduce more variation
    srand(time(NULL) + rank + getpid());

    // Process 0 starts the walker
    if (rank == 0)
    {
        walker_pos = MIN;
        printf("Process %d: Walker starts at position %d\n", rank, walker_pos);
    }

    for (int step = 0; step < STEPS; step++)
    {
        if (walker_pos != -1)
        {                                     // If walker is active in this process
            int move = (rand() % 2) ? 1 : -1; // Random move (+1 or -1)
            walker_pos += move;

            // Handle boundary conditions (wrap-around)
            if (walker_pos < MIN)
            {
                walker_pos = MAX;
            }
            else if (walker_pos > MAX)
            {
                walker_pos = MIN;
            }

            int target_rank = (walker_pos * size) / (MAX - MIN + 1);
            if (target_rank != rank)
            {
                MPI_Send(&walker_pos, 1, MPI_INT, target_rank, 0, MPI_COMM_WORLD);
                walker_pos = -1; // Mark as inactive
            }
        }

        // Check if the walker is received
        MPI_Status status;
        int received;
        MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &received, &status);
        if (received)
        {
            MPI_Recv(&walker_pos, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d: Walker received at position %d from process %d\n", rank, walker_pos, status.MPI_SOURCE);
        }
    }

    MPI_Finalize();
    return 0;
}
