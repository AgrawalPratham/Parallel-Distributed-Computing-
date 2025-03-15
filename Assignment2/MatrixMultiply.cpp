#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
#define N 70
void sequential_matrix_multiplication(int A[N][N], int B[N][N], int C[N][N])
{
    double start_time, run_time;
    start_time = MPI_Wtime(); // Start timer

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < N; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    run_time = MPI_Wtime() - start_time; // End timer
    printf("Sequential Execution Time: %lf seconds\n", run_time);
}

int main(int argc, char **argv)
{
    int rank;
    int size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int A[N][N], B[N][N], C[N][N];

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 10; 
                B[i][j] = rand() % 10;
                C[i][j] = 0;
            }
        }
    }

    int rowsPerProcess = N/size;

    int A_part[rowsPerProcess][N], C_part[rowsPerProcess][N];

    double start_time = MPI_Wtime();
    MPI_Scatter(A, rowsPerProcess * N, MPI_INT,
                A_part, rowsPerProcess * N, MPI_INT,
                0, MPI_COMM_WORLD);
    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < rowsPerProcess; i++)
    {
        for (int j = 0; j < N; j++)
        {
            C_part[i][j] = 0;
            for (int k = 0; k < N; k++)
            {
                C_part[i][j] += A_part[i][k] * B[k][j];
            }
        }
    }

    MPI_Gather(C_part, rowsPerProcess * N, MPI_INT,
               C, rowsPerProcess * N, MPI_INT,
               0, MPI_COMM_WORLD);

    double run_time = MPI_Wtime() - start_time;

    if (rank == 0)
    {
        printf("Parallel Execution Time : %lf seconds\n", run_time);
        sequential_matrix_multiplication(A, B, C);
    }

    MPI_Finalize();
    return 0;
}