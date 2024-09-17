#include "backtrace.hpp"
#include <execinfo.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BT_BUF_SIZE 100

Backtrace get_bt(){
    int nptrs;
    void *buffer[BT_BUF_SIZE];
    char **strings;

    nptrs = backtrace(buffer, BT_BUF_SIZE);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
        would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    Backtrace ret;
    for (size_t i = 0; i < nptrs; i++) {
        ret.entries.push_back("# " + std::to_string(i) + " " + strings[i]);
    }

    free(strings);

    return ret;
}

void print_bt(void) {
    auto bt = get_bt();
    for (auto e : bt.entries) {
        std::cout << e << std::endl;
    }
}
