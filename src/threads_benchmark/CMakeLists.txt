find_package(Threads REQUIRED)

add_executable(threads_benchmark threads_benchmark.cpp)
target_link_libraries(threads_benchmark PRIVATE Threads::Threads)

set(IMG_DIR "${PROJECT_SOURCE_DIR}/img/results")

# Pipeline: executable -> csv -> png -> copy to img/results
add_custom_command(
    OUTPUT ${IMG_DIR}/threads_benchmark.png
    COMMAND $<TARGET_FILE:threads_benchmark>
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/threads_benchmark.py threads_benchmark.csv
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMG_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy threads_benchmark.png ${IMG_DIR}/threads_benchmark.png
    DEPENDS threads_benchmark threads_benchmark.py
    COMMENT "Running threads benchmark and generating plot..."
)

add_custom_target(thread DEPENDS ${IMG_DIR}/threads_benchmark.png)
