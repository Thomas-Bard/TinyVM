cmake_minimum_required(VERSION 3.28)

# ASM executable
add_executable(asm)

# Add source files
file(GLOB_RECURSE ASM_SOURCES "src/*.cpp")
target_sources(asm PRIVATE ${ASM_SOURCES})

# Add own include directory
target_include_directories(asm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link against common library only
target_link_libraries(asm PRIVATE common)

# Get include directories from linked libraries
target_include_directories(asm PRIVATE
    $<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>
)

# Compiler options
target_compile_options(asm PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op>
    $<$<CXX_COMPILER_ID:Clang>:-Wduplicated-cond>
)
