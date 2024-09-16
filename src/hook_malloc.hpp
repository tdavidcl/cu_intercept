#include "cuda_hook.hpp"
#include <cstdio>


inline CUresult hooked_cuMemAlloc(CUdeviceptr *dptr, size_t bytesize){
    printf("hijacking cuMemAlloc!\n");

    using fp_real = CUresult (*)(CUdeviceptr *, size_t);
    

    return real_cuMemAlloc(dptr, bytesize);
}