#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define MAX_NUM 100 // Maximum number to check for primes

// Function to check if a number is prime
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
    { // Master process
        int num = 2, received, worker_rank;
        int primes[MAX_NUM]; // Array to store prime numbers
        int prime_count = 0, active_workers = size - 1;

        printf("Master distributing numbers up to %d...\n", MAX_NUM);

        // Send initial numbers to all available workers
        for (int i = 1; i < size; i++)
        {
            if (num <= MAX_NUM)
            {
                MPI_Send(&num, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                num++;
            }
            else
            {
                // No more numbers to test, send termination signal
                int stop_signal = -1;
                MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                active_workers--;
            }
        }

        // Receive results and assign new numbers dynamically
        while (active_workers > 0)
        {
            MPI_Status status;
            MPI_Recv(&received, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            worker_rank = status.MPI_SOURCE; // Get the worker's rank

            if (received > 0) // Store only prime numbers
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
                // No more numbers left, send termination signal
                int stop_signal = -1;
                MPI_Send(&stop_signal, 1, MPI_INT, worker_rank, 0, MPI_COMM_WORLD);
                active_workers--;
            }
        }

        // Print all prime numbers found
        printf("Prime numbers up to %d:\n", MAX_NUM);
        for (int i = 0; i < prime_count; i++)
        {
            printf("%d ", primes[i]);
        }
        printf("\n");
    }
    else
    { // Worker processes
        int num, result;

        while (1)
        {
            MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (num < 0)
                break; // Stop condition

            result = is_prime(num) ? num : 0; // Send 0 for non-primes
            MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
