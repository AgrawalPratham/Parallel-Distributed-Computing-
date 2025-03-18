#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define MAX_NUM 100 
int is_prime(int num)
{
    if (num < 2)
        return 0;
    for (int i = 2; i <= sqrt(num); i++)
    {
        if (num % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    { 
        int num = 2, received, worker_rank;
        int primes[MAX_NUM]; 
        int prime_count = 0, active_workers = size - 1;

        printf("Master distributing numbers up to %d...\n", MAX_NUM);

        for (int i = 1; i < size; i++)
        {
            if (num <= MAX_NUM)
            {
                MPI_Send(&num, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                num++;
            }
            else
            {
                int stop_signal = -1;
                MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                active_workers--;
            }
        }

        while (active_workers > 0)
        {
            MPI_Status status;
            MPI_Recv(&received, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            worker_rank = status.MPI_SOURCE; 
            if (received > 0) 
            {
                primes[prime_count++] = received;
            }

            if (num <= MAX_NUM)
            {
                MPI_Send(&num, 1, MPI_INT, worker_rank, 0, MPI_COMM_WORLD);
                num++;
            }
            else
            {
              
                int stop_signal = -1;
                MPI_Send(&stop_signal, 1, MPI_INT, worker_rank, 0, MPI_COMM_WORLD);
                active_workers--;
            }
        }

     
        printf("Prime numbers up to %d:\n", MAX_NUM);
        for (int i = 0; i < prime_count; i++)
        {
            printf("%d ", primes[i]);
        }
        printf("\n");
    }
    else
    { 
        int num, result;

        while (1)
        {
            MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (num < 0)
                break; 

            result = is_prime(num) ? num : 0; 
            MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
