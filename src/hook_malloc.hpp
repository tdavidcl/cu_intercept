#include "cuda_hook.hpp"
#include <cstdio>
#include "backtrace.hpp"

inline CUresult hooked_cuMemAlloc(CUdeviceptr *dptr, size_t bytesize){

    auto res = real_cuMemAlloc(dptr, bytesize);
    printf("-- cuMemAlloc %p %lu\n", *dptr, bytesize);
    print_bt();
    return res;
}

inline CUresult hooked_cuMemFree(CUdeviceptr dptr){

    auto res = real_cuMemFree(dptr);
    printf("-- cuMemFree %p\n", dptr);
    print_bt();
    return res;
}