find_package(Threads REQUIRED)

add_executable(threads_benchmark threads_benchmark.cpp)
target_link_libraries(threads_benchmark PRIVATE Threads::Threads)

# Pipeline: executable -> csv -> png
add_custom_command(
    OUTPUT threads_benchmark.png
    COMMAND $<TARGET_FILE:threads_benchmark>
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/threads_benchmark.py threads_benchmark.csv
    DEPENDS threads_benchmark threads_benchmark.py
    COMMENT "Running threads benchmark and generating plot..."
)

add_custom_target(thread DEPENDS threads_benchmark.png)
