#include<iostream>
#include<mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

int findInsidePoints(int sample_size)
{
    double x, y;
    unsigned int seed = time(NULL); 
    int count = 0;
    for(int i=1; i<=sample_size; i++)
    {
        x = (double)rand_r(&seed) / RAND_MAX;
        y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0)
            count++;
    }
    return count;
}
int main(int argc, char **argv)
{
    int rank;
    int size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int sample_size= 1100;
    int count = findInsidePoints(sample_size);
    int totalCount = 0;

    MPI_Reduce(&count, &totalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0)
    {
        double pi = (4.0 * totalCount)/(sample_size * size);
        cout<<"Estimated value of pi : "<< pi<< endl;
    }

    MPI_Finalize();
    return 0;
}