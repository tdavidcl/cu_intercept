#include "cuda_hook.hpp"
#include <string>
#include <dlfcn.h>

cuGetProcAddress_t real_cuGetProcAddress = NULL;


fp_cumemalloc real_cuMemAlloc = NULL;
fp_cumemfree real_cuMemFree = NULL;

void load_cuda(fp_dlsym real_dlsym) {

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
        real_cuGetProcAddress = (cuGetProcAddress_t) real_dlsym(table, "cuGetProcAddress_v2");
        if (!real_cuGetProcAddress) {
            fprintf(stderr, "Error loading original cuGetProcAddress: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }


    if (!real_cuMemAlloc) {
        void * res;
        CUdriverProcAddressQueryResult_enum ret;
        real_cuGetProcAddress("cuMemAlloc", &res, 3020, 0, &ret);

        if(res != NULL){
            real_cuMemAlloc = (fp_cumemalloc) res;
        }

        if (!real_cuMemAlloc) {
            fprintf(stderr, "Error loading original cuMemAlloc: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }

    if (!real_cuMemFree) {
        void * res;
        CUdriverProcAddressQueryResult_enum ret;
        real_cuGetProcAddress("cuMemFree", &res, 3020, 0, &ret);

        if(res != NULL){
            real_cuMemFree = (fp_cumemfree) res;
        }

        if (!real_cuMemFree) {
            fprintf(stderr, "Error loading original cuMemFree: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }

    dlclose(table);

    printf(" --- hijacking initialized --- \n");
    done = true;
}