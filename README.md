# Concurrency Benchmark
A C++23 benchmarking suite comparing performance between multi-process and multi-threaded concurrency models.  
The application measures the time needed to add up all (randomly generated) values of a big matrix (10'000 x 10'000).

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
├── build/                           # benchmark results
│   ├── process_benchmark/
│   └── threads_benchmark/
└── src/                             # source code
    ├── common/
    ├── process_benchmark/
    └── threads_benchmark/
```