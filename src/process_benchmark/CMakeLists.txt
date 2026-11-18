add_executable(process_benchmark process_benchmark.cpp)

set(IMG_DIR "${PROJECT_SOURCE_DIR}/img/results")

# Pipeline: executable -> csv -> png -> copy to img/results
add_custom_command(
    OUTPUT ${IMG_DIR}/process_benchmark.png
    COMMAND $<TARGET_FILE:process_benchmark>
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/process_benchmark.py process_benchmark.csv
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMG_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy process_benchmark.png ${IMG_DIR}/process_benchmark.png
    DEPENDS process_benchmark process_benchmark.py
    COMMENT "Running process benchmark and generating plot..."
)

add_custom_target(process DEPENDS ${IMG_DIR}/process_benchmark.png)
