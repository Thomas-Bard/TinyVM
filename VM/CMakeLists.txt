cmake_minimum_required(VERSION 3.28)

# VM executable
add_executable(vm)

# Add source files
file(GLOB_RECURSE VM_SOURCES "src/*.cpp")
target_sources(vm PRIVATE ${VM_SOURCES})

# Add own include directory
target_include_directories(vm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Find SDL3 and SDL3_ttf libraries


# Link against all libraries
target_link_libraries(vm PRIVATE vm_lib)

# Get include directories from linked libraries
target_include_directories(vm PRIVATE
    $<TARGET_PROPERTY:vm_lib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Compiler options
target_compile_options(vm PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op>
    $<$<CXX_COMPILER_ID:Clang>:-Wduplicated-cond>
)
