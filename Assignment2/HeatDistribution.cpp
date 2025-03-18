#include <iostream>
#include <mpi.h>
#include <cmath>

#define N 10   // Grid size (10x10)
#define MAX_ITER 1000  // Maximum iterations
#define TOL 0.01       // Convergence tolerance

using namespace std;

void initializeGrid(double grid[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = (i == 0 || i == N-1 || j == 0 || j == N-1) ? 100.0 : 0.0; // Hot boundary
}

void printGrid(double grid[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            cout << grid[i][j] << "\t";
        cout << endl;
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double grid[N][N], newGrid[N][N];
    initializeGrid(grid);

    if (rank == 0) {
        cout << "Initial Grid:\n";
        printGrid(grid);
    }

    int iter = 0;
    double maxDiff;

    do {
        maxDiff = 0.0;
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                newGrid[i][j] = 0.25 * (grid[i+1][j] + grid[i-1][j] + grid[i][j+1] + grid[i][j-1]);
                maxDiff = max(maxDiff, fabs(newGrid[i][j] - grid[i][j]));
            }
        }

        // Exchange data with neighbors
        MPI_Allreduce(MPI_IN_PLACE, &maxDiff, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

        // Copy newGrid to grid
        for (int i = 1; i < N - 1; i++)
            for (int j = 1; j < N - 1; j++)
                grid[i][j] = newGrid[i][j];

        iter++;
    } while (maxDiff > TOL && iter < MAX_ITER);

    if (rank == 0) {
        cout << "\nFinal Grid after " << iter << " iterations:\n";
        printGrid(grid);
    }

    MPI_Finalize();
    return 0;
}
