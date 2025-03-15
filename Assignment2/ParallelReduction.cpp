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

    int N = 16; 
    int local_size = N / size;
    int local_array[local_size];

    int global_array[N];
    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
            global_array[i] = i + 1; 
    }

    MPI_Scatter(global_array, local_size, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    int local_sum = 0;
    for (int i = 0; i < local_size; i++)
    {
        local_sum += local_array[i];
    }

    int global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        cout << "Total Sum: " << global_sum << endl;
    }

    MPI_Finalize();
    return 0;
}
