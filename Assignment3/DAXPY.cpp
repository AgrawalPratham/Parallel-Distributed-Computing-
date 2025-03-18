#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N (1 << 16) // Vector size 2^16

void daxpy_serial(double a, double *X, double *Y)
{
    for (int i = 0; i < N; i++)
    {
        X[i] = a * X[i] + Y[i];
    }
}

void daxpy_parallel(double a, double *X, double *Y, int start, int end)
{
    for (int i = start; i < end; i++)
    {
        X[i] = a * X[i] + Y[i];
    }
}

int main(int argc, char **argv)
{
    int rank, size;
    double *X, *Y, a = 2.5;
    double start_time, end_time, parallel_time, serial_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = N / size;

    // Allocate memory
    if (rank == 0)
    {
        X = (double *)malloc(N * sizeof(double));
        Y = (double *)malloc(N * sizeof(double));
        for (int i = 0; i < N; i++)
        {
            X[i] = 1.0;
            Y[i] = 2.0;
        }

        // Serial execution
        start_time = MPI_Wtime();
        daxpy_serial(a, X, Y);
        end_time = MPI_Wtime();
        serial_time = end_time - start_time;

        // Reset X for parallel execution
        for (int i = 0; i < N; i++)
        {
            X[i] = 1.0;
        }
    }

    // Allocate local chunk memory
    double *X_local = (double *)malloc(chunk_size * sizeof(double));
    double *Y_local = (double *)malloc(chunk_size * sizeof(double));

    // Scatter data
    MPI_Scatter(X, chunk_size, MPI_DOUBLE, X_local, chunk_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(Y, chunk_size, MPI_DOUBLE, Y_local, chunk_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Parallel execution
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    daxpy_parallel(a, X_local, Y_local, 0, chunk_size);

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    parallel_time = end_time - start_time;

    // Gather results
    MPI_Gather(X_local, chunk_size, MPI_DOUBLE, X, chunk_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Serial Execution Time: %f seconds\n", serial_time);
        printf("Parallel Execution Time: %f seconds\n", parallel_time);
        printf("Speedup: %f\n", serial_time / parallel_time);
    }

    // Cleanup
    free(X_local);
    free(Y_local);
    if (rank == 0)
    {
        free(X);
        free(Y);
    }

    MPI_Finalize();
    return 0;
}
