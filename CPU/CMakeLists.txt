cmake_minimum_required(VERSION 3.28)

# CPU library - static library
add_library(cpu STATIC)

# Add source files
file(GLOB_RECURSE CPU_SOURCES "src/*.cpp")
target_sources(cpu PRIVATE ${CPU_SOURCES})

# Add include directory (public)
target_include_directories(cpu PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link against common library
target_link_libraries(cpu PUBLIC common)

# Only expose common's public headers through CPU's public headers
target_include_directories(cpu PUBLIC
    $<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>
)

# Compiler options
target_compile_options(cpu PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op>
    $<$<CXX_COMPILER_ID:Clang>:-Wduplicated-cond>
)
