cmake -B build .
cd build
make 

LD_PRELOAD=./libcuintercept.so ./intercept_test