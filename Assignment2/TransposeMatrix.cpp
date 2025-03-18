#include <iostream>
#include <mpi.h>

#define N 4 // Matrix size
using namespace std;

void printMatrix(int matrix[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            cout << matrix[i][j] << "\t";
        cout << endl;
    }
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int matrix[N][N], transposed[N][N];

    if (rank == 0)
    {
        int val = 1;
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                matrix[i][j] = val++;
            }
        }
        cout << "Original Matrix:\n";
        printMatrix(matrix);
    }

    int local_row[N];
    int trans_row[N];

    MPI_Scatter(matrix, N, MPI_INT, local_row, N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Alltoall(local_row, 1, MPI_INT, trans_row, 1, MPI_INT, MPI_COMM_WORLD);

    MPI_Gather(trans_row, N, MPI_INT, transposed, N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        cout << "\nTransposed Matrix:\n";
        printMatrix(transposed);
    }

    MPI_Finalize();
    return 0;
}
