#define __USE_GNU

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cuda.h>
#include <iostream>

extern "C" {
void *__libc_dlsym(void *map, const char *name);
}
extern "C" {
void *__libc_dlopen_mode(const char *name, int mode);
}

#define STRINGIFY(x) #x
#define CUDA_SYMBOL_STRING(x) STRINGIFY(x)
#define SYNCP_MESSAGE

typedef void *(*fnDlsym)(void *, const char *);

static void *real_dlsym(void *handle, const char *symbol) {
  static fnDlsym internal_dlsym =
      (fnDlsym)__libc_dlsym(__libc_dlopen_mode("libdl.so.2", RTLD_LAZY), "dlsym");
  return (*internal_dlsym)(handle, symbol);
}

/*
** If the user of this library does not wish to include CUDA specific
*code/headers in the code,
** then all the parameters can be changed and/or simply casted before calling
*the callback.
*/
#define CU_HOOK_GENERATE_INTERCEPT(funcname, params,callback_pre, callback_post, ...)        \
  CUresult CUDAAPI funcname params {                                         \
    static void* real_func =                                                 \
        (void*)real_dlsym(RTLD_NEXT, CUDA_SYMBOL_STRING(funcname));          \
    CUresult result = CUDA_SUCCESS;                                          \
                                                                             \
                                                                      \
    if (callback_pre) {                                         \
      ((CUresult CUDAAPI(*) params)callback_pre)(__VA_ARGS__);  \
    }   \
    if(!real_func ){std::cout << "error" << std::endl;}                             \
    result = ((CUresult CUDAAPI(*) params)real_func)(__VA_ARGS__);           \
    if (callback_post && result == CUDA_SUCCESS) {              \
      ((CUresult CUDAAPI(*) params)callback_post)(__VA_ARGS__); \
    }                                                                        \
    return (result);                                                         \
  }



/*
 ** Interposed Functions
 */
void *dlsym(void *handle, const char *symbol) {
  // Early out if not a CUDA driver symbol
  if (strncmp(symbol, "cu", 2) != 0) {
    return (real_dlsym(handle, symbol));
  }

  if (strcmp(symbol, CUDA_SYMBOL_STRING(cuMemAlloc)) == 0) {
    return (void *)(&cuMemAlloc);
  } else if (strcmp(symbol, CUDA_SYMBOL_STRING(cuMemFree)) == 0) {
    return (void *)(&cuMemFree);
  } 
  //else if (strcmp(symbol, CUDA_SYMBOL_STRING(cuCtxGetCurrent)) == 0) {
  //  return (void *)(&cuCtxGetCurrent);
  //} else if (strcmp(symbol, CUDA_SYMBOL_STRING(cuCtxSetCurrent)) == 0) {
  //  return (void *)(&cuCtxSetCurrent);
  //} else if (strcmp(symbol, CUDA_SYMBOL_STRING(cuCtxDestroy)) == 0) {
  //  return (void *)(&cuCtxDestroy);
  //}
  return (real_dlsym(handle, symbol));
}



void cu_malloc_callback(CUdeviceptr * dptr, size_t bytesize){
    std::cout << "cu_malloc_callback"<< dptr << " " << bytesize << std::endl;
}
void cu_free_callback(CUdeviceptr * dptr){
    std::cout << "cu_free_callback"<< dptr << std::endl;
}

CU_HOOK_GENERATE_INTERCEPT(cuMemAlloc,
                           (CUdeviceptr * dptr, size_t bytesize),nullptr,cu_malloc_callback, dptr,
                           bytesize)

CU_HOOK_GENERATE_INTERCEPT(cuMemFree, (CUdeviceptr dptr),nullptr,cu_free_callback,
                           dptr)


//CU_HOOK_GENERATE_INTERCEPT(cuCtxGetCurrent,
//                           (CUcontext * pctx), pctx)
//CU_HOOK_GENERATE_INTERCEPT(cuCtxSetCurrent,
//                           (CUcontext ctx), ctx)
//CU_HOOK_GENERATE_INTERCEPT(cuCtxDestroy, (CUcontext ctx),
//                           ctx)