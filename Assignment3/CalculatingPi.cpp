#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_STEPS 100000 // Number of steps

int main(int argc, char *argv[])
{
    int rank, size, i;
    double x, sum = 0.0, pi, step;
    long num_steps = NUM_STEPS;

    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    step = 1.0 / (double)num_steps;

    // Broadcast num_steps from process 0 to all processes
    MPI_Bcast(&num_steps, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // Calculate the start and end indices for each process
    int start = rank * (num_steps / size);
    int end = (rank + 1) * (num_steps / size);

    // Each process calculates its partial sum
    for (i = start; i < end; i++)
    {
        x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    double partial_pi = step * sum;

    // Use MPI_Reduce to sum up partial results into pi at process 0
    MPI_Reduce(&partial_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Print final result from process 0
    if (rank == 0)
    {
        printf("Approximated π value: %.15f\n", pi);
    }

    MPI_Finalize(); // Finalize MPI
    return 0;
}
