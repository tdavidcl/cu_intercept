#include <string>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <cuda.h>
#include <cstring>



using fp_dlsym  = void * (*)(void *, const char *);

#define STRINGIFY(x) #x
#define CUDA_SYMBOL_STRING(x) STRINGIFY(x)


// Function pointer to the real cuGetProcAddress
typedef CUresult (*cuGetProcAddress_t)(const char *, void **, unsigned int, cuuint64_t,
                             CUdriverProcAddressQueryResult *);
static cuGetProcAddress_t real_cuGetProcAddress = NULL;

void load_cuda(fp_dlsym real_dlsym){

    static bool done = false;
    if (done) {
        return;
    }

    void *table = NULL;

    printf(" --- Start hijacking --- \n");

    std::string cuda_filename = "libcuda.so.1";

    table = dlopen(cuda_filename.c_str(), RTLD_NOW | RTLD_NODELETE);
    if (!table) {
        printf("can't find library %s", cuda_filename.c_str());
    }

    if (!real_cuGetProcAddress) {
        real_cuGetProcAddress = (cuGetProcAddress_t)real_dlsym(table, "cuGetProcAddress_v2");
        if (!real_cuGetProcAddress) {
            fprintf(stderr, "Error loading original cuGetProcAddress: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }

    dlclose(table);
}

CUresult cuGetProcAddress(const char *symbol, void **funcPtr, unsigned int flags, cuuint64_t version,
                             CUdriverProcAddressQueryResult *symbolStatus) {

    printf("Intercepted cuGetProcAddress: Requesting symbol %s\n", symbol);

    // Call the original cuGetProcAddress to actually resolve the symbol
    CUresult result = real_cuGetProcAddress(symbol, funcPtr, flags, version,symbolStatus);

    // Optionally, log the result
    if (result == CUDA_SUCCESS) {
        printf("    cuGetProcAddress successful: Resolved symbol %s to address %p\n", symbol, *funcPtr);
    } else {
        printf("    cuGetProcAddress failed with error code %d\n", result);
    }

    return result;
}


void* __dlsym_hook_section(void* handle, const char* symbol, fp_dlsym real_dlsym){


    if (strcmp(symbol, CUDA_SYMBOL_STRING(cuGetProcAddress_v2)) == 0) {
        printf("Applying dlsym hook: Requesting symbol %s\n", symbol);
        cuGetProcAddress_t new_addr = &cuGetProcAddress;
        return (void*) new_addr;
    }

    return real_dlsym(handle,symbol);
    

    return NULL;
}



#define FUNC_ATTR_VISIBLE  __attribute__((visibility("default"))) 
FUNC_ATTR_VISIBLE void *dlsym(void *handle, const char *symbol)
{
    printf("Intercepted dlsym: Requesting symbol %s\n", symbol);

    
    fp_dlsym real_dlsym =NULL;
    if (real_dlsym == NULL)
        real_dlsym=(fp_dlsym) dlvsym(RTLD_NEXT, "dlsym", "GLIBC_2.2.5");


    if (strcmp(symbol, CUDA_SYMBOL_STRING(cuGetProcAddress_v2)) == 0) {
        printf("Intercepting cuGetProcAddress_v2 via dlsym!\n");
        load_cuda(real_dlsym);
        void* f = __dlsym_hook_section(handle, symbol, real_dlsym);
        if (f != NULL)
            return f;
    }

    /* my target binary is even asking for dlsym() via dlsym()... */
    if (!strcmp(symbol,"dlsym")) 
        return (void*)dlsym;

    return real_dlsym(handle,symbol);
}



