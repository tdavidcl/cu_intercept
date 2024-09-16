#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <math.h>
#include <stdio.h>
#include <sys/time.h>

const int Row = 2048;
const int Col = 2048;

__global__ void matrix_mul_gpu(int *M, int *N, int *P, int width) {
  int i = threadIdx.x + blockDim.x * blockIdx.x;
  int j = threadIdx.y + blockDim.y * blockIdx.y;

  int sum = 0;
  for (int k = 0; k < width; k++) {
    int a = M[j * width + k];
    int b = N[k * width + i];
    sum += a * b;
  }
  P[j * width + i] = sum;
}

int main() {
  cudaError_t cuda_err = cudaSuccess;
  printf("func start \n");
  int *A = (int *)malloc(sizeof(int) * Row * Col);
  int *B = (int *)malloc(sizeof(int) * Row * Col);
  int *C = (int *)malloc(sizeof(int) * Row * Col);
  // malloc device memory
  int *d_dataA, *d_dataB, *d_dataC;
  printf("before cudaMalloc()\n");
  cuda_err = cudaMalloc((void **)&d_dataA, sizeof(int) * Row * Col);
  // cuda_err = cudaGetLastError();
  if (cudaSuccess != cuda_err) {
    fprintf(stderr, "(%s:%s:%d)", __FILE__, __FUNCTION__, __LINE__);
    fprintf(stderr, "%s\n", cudaGetErrorString(cuda_err));
    printf("cuda_err is %d\n", cuda_err);
    exit(1);
  }
  printf("after cudaMalloc()\n");
  cudaMalloc((void **)&d_dataB, sizeof(int) * Row * Col);
  cudaMalloc((void **)&d_dataC, sizeof(int) * Row * Col);
  // set value
  for (int i = 0; i < Row * Col; i++) {
    A[i] = 90;
    B[i] = 10;
  }

  cudaMemcpy(d_dataA, A, sizeof(int) * Row * Col, cudaMemcpyHostToDevice);
  cudaMemcpy(d_dataB, B, sizeof(int) * Row * Col, cudaMemcpyHostToDevice);
  dim3 threadPerBlock(16, 16);
  dim3 blockNumber((Col + threadPerBlock.x - 1) / threadPerBlock.x,
                   (Row + threadPerBlock.y - 1) / threadPerBlock.y);
  matrix_mul_gpu<<<blockNumber, threadPerBlock>>>(d_dataA, d_dataB, d_dataC,
                                                  Col);
  cudaDeviceSynchronize();
  cudaMemcpy(C, d_dataC, sizeof(int) * Row * Col, cudaMemcpyDeviceToHost);
  free(A);
  free(B);
  free(C);
  cudaFree(d_dataA);
  cudaFree(d_dataB);
  cudaFree(d_dataC);

  return 0;
}