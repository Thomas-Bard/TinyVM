cmake_minimum_required(VERSION 3.28)

# Common library - static library with no external dependencies
add_library(common STATIC)

# Add source files
file(GLOB_RECURSE COMMON_SOURCES "src/*.cpp")
target_sources(common PRIVATE ${COMMON_SOURCES})

# Add include directory
target_include_directories(common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler options
target_compile_options(common PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op>
    $<$<CXX_COMPILER_ID:Clang>:-Wduplicated-cond>
)
