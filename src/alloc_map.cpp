#include "alloc_map.hpp"
#include "time.hpp"
#include <fstream>
#include <mutex>
#include <string>
#include <iostream>
#include <unistd.h>


std::mutex alloc_map_mutex;
std::unordered_map<CUdeviceptr, AllocInfo> cuda_alloc_map;

size_t get_total_alloc(){
    size_t total_alloc = 0;
    for (auto [ptr,alloc] : cuda_alloc_map) {
        total_alloc += alloc.bytesize;
    }
    return total_alloc;
}

void print_alloc_map(){
    printf("Alloc map:\n");
    printf("  -- Total alloc %lu\n", get_total_alloc());

    return;
    for (auto [ptr,alloc] : cuda_alloc_map) {
        std::cout << " -- alloc" << std::endl;
        for (auto e : alloc.bt_alloc.entries) {

            if (e.find("libcuintercept.so") == std::string::npos) {
                std::cout <<"     "<< e << std::endl;
                //break;
            }
        }
    }
}




std::mutex file_mutex;
void file_write_total(){

    std::lock_guard<std::mutex> guard(file_mutex);
    static std::ofstream out("output."+std::to_string(getpid())+".txt");

    out << std::to_string(get_sec_elapsed()) + " " + std::to_string(get_total_alloc()) + "\n";
}



void register_alloc(CUdeviceptr ptr, size_t bytesize, Backtrace bt){
    std::lock_guard<std::mutex> guard(alloc_map_mutex);
    const bool ok = cuda_alloc_map.insert({ptr, AllocInfo{
        bytesize,ptr,bt
    }}).second;
    if(!ok){
        throw "";
    }
    
    printf(" -- Cuda alloc ptr=%p size=%lu | total mem = %lu\n",ptr,bytesize,get_total_alloc());
    file_write_total();
    //print_alloc_map();
    //print_bt();
}

void register_free(CUdeviceptr ptr){
    std::lock_guard<std::mutex> guard(alloc_map_mutex);
    cuda_alloc_map.erase(ptr);
    printf(" -- Cuda free  ptr=%p  | total mem = %lu\n",ptr,get_total_alloc());
    file_write_total();
}

