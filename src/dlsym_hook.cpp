#include "dlsym_hook.hpp"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#define FUNC_ATTR_VISIBLE __attribute__((visibility("default")))
FUNC_ATTR_VISIBLE void *dlsym(void *handle, const char *symbol) {
    printf(" --- Intercepted dlsym: Requesting symbol %s\n", symbol);

    fp_dlsym real_dlsym = NULL;
    if (real_dlsym == NULL)
        real_dlsym = (fp_dlsym) dlvsym(RTLD_NEXT, "dlsym", "GLIBC_2.2.5");

    void *f = dlsym_callback(handle, symbol, real_dlsym);
    if (f != NULL)
        return f;

    /* my target binary is even asking for dlsym() via dlsym()... */
    if (!strcmp(symbol, "dlsym"))
        return (void *) dlsym;

    return real_dlsym(handle, symbol);
}