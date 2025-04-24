#include<iostream>
#include<vector>
#include<chrono>

using namespace std;
using namespace std::chrono;

__global__ void vectorAdditionKernel(int *D_A, int size)
{
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if(idx >= size)
        return;
    int val = D_A[idx];
    for(int i=val/2; i>=0; i--)
    {
        if( (i * i) <= val)
        {
            D_A[idx] = i;
            break;
        }
    }
}
int main()
{
    vector<int> A;
    int size;
    cout<<"Enter the size of vector needed : ";
    cin>>size;

    for(int i=0; i<size; i++)
    {
        A.push_back( rand()% 100);
    }

    int batchSize = 256;
    int *d_A;
    vector<int> C(size);

    auto start = high_resolution_clock::now();
    
    for(int batchStart = 0; batchStart < size; batchStart += batchSize)
    {
        int currentBatchSize = min(batchSize, size - batchStart);
        cudaMalloc((void **)&d_A, batchSize * sizeof(int));
        cudaMemcpy(d_A, A.data() + batchStart, currentBatchSize * sizeof(int), cudaMemcpyHostToDevice);

        dim3 thread(batchSize, 1, 1);
        dim3 block((size + batchSize-1)/batchSize, 1, 1);
       
        vectorAdditionKernel<<<block, thread>>>(d_A, currentBatchSize);
        cudaDeviceSynchronize();
        cudaMemcpy(C.data() + batchStart, d_A, currentBatchSize * sizeof(int), cudaMemcpyDeviceToHost);
        cudaFree(d_A);
    }
    auto end = high_resolution_clock::now();
    // cout<<"Original array : "<<endl;
    // for(int element:A)
    // {
    //     cout<<element<<"\t";
    // }
    // cout<<endl;

    // cout<<"Square root array : "<<endl;
    // for(int element:C)
    // {
    //     cout<<element<<"\t";
    // }
    // cout<<endl;

    auto duration = duration_cast<milliseconds>(end- start);
    cout<<" Time taken in milliseconds : "<<duration.count()<<endl;
    return 0;
}