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

## Outputs

In the path where you are running the programm with the LD_PRELOAD you wil find files named `output.<pid>.txt` where `<pid>` is replace by the pid of your programm