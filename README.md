# Concurrency Benchmark
A C++23 benchmarking suite comparing performance between multi-process and multi-threaded concurrency models.  
The application measures the time needed to add up all (randomly generated) values of a big matrix (10'000 x 10'000).

## Dependencies
- **Boost** (specifically `Boost.Interprocess`)
  ```shell
  # Ubuntu / Debian
  sudo apt install libboost-dev

  # Fedora
  sudo dnf install boost-devel
  ```
- **Python 3** with `matplotlib` and `numpy` (for benchmark plotting)

## Build Instructions
```shell
cmake -B build  
cmake --build build  
cmake --build build --target run_all  
```
## Project structure
After building, the project is organized as follows:

```text
ConcurrencyBenchmark/
├── build/                         # build files
├── img/                           
│   ├── example/                   # example results
│   └── results/                   # benchmark results
└── src/                           # source code
    ├── common/
    ├── process_benchmark/
    └── threads_benchmark/
```