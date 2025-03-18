#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_STEPS 100000 

int main(int argc, char *argv[])
{
    int rank, size, i;
    double x, sum = 0.0, pi, step;
    long num_steps = NUM_STEPS;

    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    step = 1.0 / (double)num_steps;

    MPI_Bcast(&num_steps, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    int start = rank * (num_steps / size);
    int end = (rank + 1) * (num_steps / size);

    for (i = start; i < end; i++)
    {
        x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    double partial_pi = step * sum;

    MPI_Reduce(&partial_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  
    if (rank == 0)
    {
        printf("Approximated π value: %.15f\n", pi);
    }

    MPI_Finalize(); 
    return 0;
}
