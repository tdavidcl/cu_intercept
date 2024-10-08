
#include <string>
#define _GNU_SOURCE
#include "cuda_hook.hpp"
#include "dlsym_hook.hpp"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "hook_malloc.hpp"



void *get_cu_get_proc_hook(CUresult *res, const char *symbol,
    unsigned int flags,
    cuuint64_t version);


#if(CUDA_VERSION < 12000)
CUresult cuGetProcAddress(
    const char *symbol,
    void **funcPtr,
    unsigned int flags,
    cuuint64_t version) {

    printf("Intercepted cuGetProcAddress: Requesting symbol %s (flags=%u, version=%lu)\n", symbol,flags,version);

    // Call the original cuGetProcAddress to actually resolve the symbol
    CUresult result = real_cuGetProcAddress(symbol, funcPtr, flags, version);

    void *ret = get_cu_get_proc_hook(&result, symbol, flags, version);
    if (ret != nullptr) {
        printf("    cuGetProcAddress hook : replace %s ptr=%p to ptr=%p\n", symbol, *funcPtr, ret);
        *funcPtr = ret;
        result   = CUDA_SUCCESS;
    } else if (result == CUDA_SUCCESS) {
        // printf(
        //     "    cuGetProcAddress successful: Resolved symbol %s to address
        //     %p\n", symbol, *funcPtr);
    } else {
        printf("    cuGetProcAddress failed with error code %d\n", result);
    }

    return result;
}
#else
CUresult cuGetProcAddress(
    const char *symbol,
    void **funcPtr,
    unsigned int flags,
    cuuint64_t version,
    CUdriverProcAddressQueryResult *symbolStatus) {

    printf("Intercepted cuGetProcAddress: Requesting symbol %s (flags=%u, version=%lu)\n", symbol,flags,version);

    // Call the original cuGetProcAddress to actually resolve the symbol
    CUresult result = real_cuGetProcAddress(symbol, funcPtr, flags, version, symbolStatus);

    void *ret = get_cu_get_proc_hook(&result, symbol, flags, version);
    if (ret != nullptr) {
        printf("    cuGetProcAddress hook : replace %s ptr=%p to ptr=%p\n", symbol, *funcPtr, ret);
        *funcPtr = ret;
        result   = CUDA_SUCCESS;
    } else if (result == CUDA_SUCCESS) {
        // printf(
        //     "    cuGetProcAddress successful: Resolved symbol %s to address
        //     %p\n", symbol, *funcPtr);
    } else {
        printf("    cuGetProcAddress failed with error code %d\n", result);
    }

    return result;
}
#endif

void *get_cu_get_proc_hook(CUresult *res, const char *symbol,
    unsigned int flags,
    cuuint64_t version) {

    if (!strcmp(symbol, "cuGetProcAddress")) {
        cuGetProcAddress_t new_addr = &cuGetProcAddress;
        return (void *) new_addr;
    }
    
    else if (!strcmp(symbol, "cuMemAlloc")) {
        auto new_addr = &hooked_cuMemAlloc;
        return (void *) new_addr;
    }
    else if (!strcmp(symbol, "cuMemFree")) {
        auto new_addr = &hooked_cuMemFree;
        return (void *) new_addr;
    }

    return nullptr;
}

void* dlsym_callback(void *handle, const char *symbol, fp_dlsym real_dlsym){
    load_cuda(real_dlsym);

    if (strcmp(symbol, CUDA_SYMBOL_STRING(cuGetProcAddress_v2)) == 0) {
        printf(" --- Applying dlsym hook: Requesting symbol %s\n", symbol);
        cuGetProcAddress_t new_addr = &cuGetProcAddress;
        return (void *) new_addr;
    }

    if (strcmp(symbol, "cuMemAlloc") == 0) {
        printf(" --- Applying dlsym hook: Requesting symbol %s\n", symbol);
        auto new_addr = &hooked_cuMemAlloc;
        return (void *) new_addr;
    }

    if (strcmp(symbol, CUDA_SYMBOL_STRING(cuMemAlloc_v2)) == 0) {
        printf(" --- Applying dlsym hook: Requesting symbol %s\n", symbol);
        auto new_addr = &hooked_cuMemAlloc;
        return (void *) new_addr;
    }

    return NULL;
}