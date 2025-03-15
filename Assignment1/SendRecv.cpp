#include<mpi.h>
#include<iostream>
#include<cstring>

using namespace std;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) //root process
    {
        char msg1[] = "Hello from process 1";
        int len = strlen(msg1);
        MPI_Send(msg1, len+1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    }
    else
    {
        char msg2[100];
        int len=100;
        MPI_Recv(msg2, len, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout<<"My Process id is : "<<rank<<" Message received from root process : "<<msg2<<endl;
    }

    MPI_Finalize();
    return 0;
}