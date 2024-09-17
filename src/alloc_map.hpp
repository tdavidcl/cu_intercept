#pragma once

#include <cstddef>
#include <cuda.h>
#include "backtrace.hpp"
#include <unordered_map>

struct AllocInfo{
    size_t bytesize;
    CUdeviceptr ptr;
    Backtrace bt_alloc;
};


size_t get_total_alloc();

void print_alloc_map();


void register_alloc(CUdeviceptr ptr, size_t bytesize, Backtrace bt);

void register_free(CUdeviceptr ptr);