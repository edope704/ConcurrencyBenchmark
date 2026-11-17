cmake_minimum_required(VERSION 3.20)
project(ConcurrencyBenchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/common)

add_subdirectory(src/process_benchmark)
add_subdirectory(src/threads_benchmark)

# Optional: run both from the root
add_custom_target(run_all DEPENDS process thread)