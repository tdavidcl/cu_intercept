# cu_intercept

A small preload utility to intercept CUDA memory managment.

---

## Compile 

```
cmake -B build .
cd build
make
```

## Usage

```
LD_PRELOAD=<path to libcuintercept.so> <your programm>
```
