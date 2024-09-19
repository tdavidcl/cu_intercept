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
LD_PRELOAD=<path to libcuintercept.so> <your cuda programm>
```

## Outputs

In the path where you are running the programm with the LD_PRELOAD you wil find files named `output.<pid>.txt` where `<pid>` is replace by the pid of your programm.
They contain the amount of memory allocated by the programm at every time of allocations.

you can read them with python like so:
```py
import numpy as np
import matplotlib.pyplot as plt

out = np.loadtxt("output.324547.txt")
plt.plot(out[:,0],out[:,1])

plt.xlabel("time [s]")
plt.ylabel("memory [byte]")
plt.show()
```
