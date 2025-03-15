#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 16;
    int local_size = N / size;
    int local_array[local_size];
    int global_array[N];

    if (rank == 0)
    {
        srand(time(0));
        for (int i = 0; i < N; i++)
        {
            global_array[i] = rand() % 100;
        }
        cout << "Unsorted Array: ";
        for (int i = 0; i < N; i++)
            cout << global_array[i] << " ";
        cout << endl;
    }

    MPI_Scatter(global_array, local_size, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    sort(local_array, local_array + local_size);

    for (int phase = 0; phase < size; phase++)
    {
        if (phase % 2 == 0)
        { // Even phase
            if (rank % 2 == 0 && rank < size - 1)
            {
                int neighbor[local_size];
                MPI_Send(local_array, local_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(neighbor, local_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int merged[2 * local_size];
                merge(local_array, local_array + local_size, neighbor, neighbor + local_size, merged);
                copy(merged, merged + local_size, local_array);
            }
            else if (rank % 2 == 1)
            {
                int neighbor[local_size];
                MPI_Recv(neighbor, local_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(local_array, local_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

                int merged[2 * local_size];
                merge(neighbor, neighbor + local_size, local_array, local_array + local_size, merged);
                copy(merged + local_size, merged + 2 * local_size, local_array);
            }
        }
        else
        { // Odd phase
            if (rank % 2 == 1 && rank < size - 1)
            {
                int neighbor[local_size];
                MPI_Send(local_array, local_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(neighbor, local_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int merged[2 * local_size];
                merge(local_array, local_array + local_size, neighbor, neighbor + local_size, merged);
                copy(merged, merged + local_size, local_array);
            }
            else if (rank % 2 == 0 && rank > 0)
            {
                int neighbor[local_size];
                MPI_Recv(neighbor, local_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(local_array, local_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

                int merged[2 * local_size];
                merge(neighbor, neighbor + local_size, local_array, local_array + local_size, merged);
                copy(merged + local_size, merged + 2 * local_size, local_array);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Gather(local_array, local_size, MPI_INT, global_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    double run_time = MPI_Wtime() - start_time;

    if (rank == 0)
    {
        cout << "Sorted Array: ";
        for (int i = 0; i < N; i++)
            cout << global_array[i] << " ";
        cout << endl;
        printf("Parallel Execution Time: %lf seconds\n", run_time);
    }

    MPI_Finalize();
    return 0;
}
