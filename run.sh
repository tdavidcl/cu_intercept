cmake .
make 

LD_PRELOAD=./libcuintercept.so ./intercept_test