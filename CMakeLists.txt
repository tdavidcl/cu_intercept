cmake_minimum_required(VERSION 3.10)
project(cuintercept CXX CUDA)


if (NOT DEFINED ENV{CUDA_HOME})
    set(CUDA_HOME /opt/cuda)
else()
    set(CUDA_HOME $ENV{CUDA_HOME})
endif()

#enable export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cuintercept SHARED src/lib.cpp)
set(LIBRARY_COMPILE_FLAGS -shared -fPIC -g -D_GNU_SOURCE -Wall)
include_directories(${CUDA_HOME}/include)
target_compile_options(cuintercept PUBLIC ${LIBRARY_COMPILE_FLAGS})
target_link_libraries(cuintercept PUBLIC -lcuda -lnvidia-ml)



add_executable(intercept_test test/test.cu)
set_target_properties(intercept_test PROPERTIES CUDA_ARCHITECTURES "72")

