#include <iostream>
#include <mpi.h>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 16;
    int local_size = N / size;
    int A[N], B[N];
    int local_A[local_size], local_B[local_size];

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            A[i] = i + 1;
            B[i] = N - i;
        }
    }

    MPI_Scatter(A, local_size, MPI_INT, local_A, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, local_size, MPI_INT, local_B, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    int local_dot_product = 0;
    for (int i = 0; i < local_size; i++)
    {
        local_dot_product += local_A[i] * local_B[i];
    }

    int global_dot_product = 0;
    MPI_Reduce(&local_dot_product, &global_dot_product, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        cout << "Dot Product: " << global_dot_product << endl;
    }

    MPI_Finalize();
    return 0;
}
