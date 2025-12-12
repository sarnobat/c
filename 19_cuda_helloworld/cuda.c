#include <stdio.h>

// Kernel runs on GPU
__global__ void add(int *a, int *b, int *c) {
    int i = threadIdx.x;
    c[i] = a[i] + b[i];
}

int main() {
    int a[3] = {1, 2, 3};
    int b[3] = {4, 5, 6};
    int c[3] = {0, 0, 0};
    
    int *d_a, *d_b, *d_c;  // Device pointers
    int size = 3 * sizeof(int);
    
    // Allocate GPU memory
    cudaMalloc(&d_a, size);
    cudaMalloc(&d_b, size);
    cudaMalloc(&d_c, size);
    
    // Copy data to GPU
    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);
    
    // Run kernel with 1 block, 3 threads
    add<<<1, 3>>>(d_a, d_b, d_c);
    
    // Copy result back to CPU
    cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);
    
    printf("Results: %d %d %d\n", c[0], c[1], c[2]);
    
    // Free GPU memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    
    return 0;
}

// Compile with: nvcc -o hello_cuda hello.cu
// Requires: NVIDIA GPU + CUDA toolkit