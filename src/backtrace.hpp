#pragma once

#include <string>
#include <vector>
struct Backtrace{
    std::vector<std::string> entries;
};

Backtrace get_bt();
void print_bt(void);
