#pragma once

using fp_dlsym = void *(*) (void *, const char *);

void* dlsym_callback(void *handle, const char *symbol, fp_dlsym real_dlsym);
