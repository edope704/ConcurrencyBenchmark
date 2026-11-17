add_executable(process_benchmark process_benchmark.cpp)

# Pipeline: executable -> csv -> png
add_custom_command(
    OUTPUT process_benchmark.png
    COMMAND $<TARGET_FILE:process_benchmark>
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/process_benchmark.py process_benchmark.csv
    DEPENDS process_benchmark process_benchmark.py
    COMMENT "Running process benchmark and generating plot..."
)

add_custom_target(process DEPENDS process_benchmark.png)