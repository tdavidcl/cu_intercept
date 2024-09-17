#pragma once

#include "cuda_hook.hpp"
#include <cstdio>
#include <iostream>
#include "backtrace.hpp"
#include "alloc_map.hpp"

inline CUresult hooked_cuMemAlloc(CUdeviceptr *dptr, size_t bytesize){

    auto res = real_cuMemAlloc(dptr, bytesize);
    register_alloc(*dptr, bytesize, get_bt());

    if(res != CUDA_SUCCESS){
        std::cout << "something's weird bt :";
        print_bt();
    }
    return res;
}

inline CUresult hooked_cuMemFree(CUdeviceptr dptr){

    auto res = real_cuMemFree(dptr);
    register_free(dptr);
    return res;

}