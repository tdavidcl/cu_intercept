
#include <cuda.h>
#include "dlsym_hook.hpp"

#define STRINGIFY(x) #x
#define CUDA_SYMBOL_STRING(x) STRINGIFY(x)

// Function pointer to the real cuGetProcAddress
typedef CUresult (*cuGetProcAddress_t)(
    const char *, void **, unsigned int, cuuint64_t, CUdriverProcAddressQueryResult *);

extern cuGetProcAddress_t real_cuGetProcAddress;


using fp_cumemalloc = CUresult (*)(CUdeviceptr *, size_t); 
extern fp_cumemalloc real_cuMemAlloc;

void load_cuda(fp_dlsym real_dlsym);