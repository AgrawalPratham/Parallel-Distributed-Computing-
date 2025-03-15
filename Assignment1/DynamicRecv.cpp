#include<iostream>
#include<mpi.h>
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
        char msg1[] = "Sending string from root process to be handled dynamically";
        int msg1Len = strlen(msg1);
        MPI_Send(msg1, msg1Len+1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Status status;
        int msg2Len = 0;
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &msg2Len);

        char *msg2= new char[msg2Len];
        MPI_Recv(msg2, msg2Len, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        cout<<"My rank is : "<<rank<<endl;
        cout<<"Received string : \""<<msg2<<"\" from source rank : "<<status.MPI_SOURCE<<" with tag : "<<status.MPI_TAG<<endl;
    }
    MPI_Finalize();
}