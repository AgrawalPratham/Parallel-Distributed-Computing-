#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int arr[] = {1, 2, 3, 4, 5};
    int res = 0;

    if (rank < 5)
    {
        MPI_Scan(&arr[rank], &res, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }

    int final_res[5] = {0};
    MPI_Gather(&res, 1, MPI_INT, final_res, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int i = 0; i < 5; i++)
        {
            cout << final_res[i] << "\t";
        }
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}
