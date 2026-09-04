cmake_minimum_required(VERSION 3.28)

# RAM library - static library
add_library(ram STATIC)

# Add source files
file(GLOB_RECURSE RAM_SOURCES "src/*.cpp")
target_sources(ram PRIVATE ${RAM_SOURCES})

# Add include directory (public)
target_include_directories(ram PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link against common library
target_link_libraries(ram PUBLIC common)

# Only expose common's public headers through RAM's public headers
target_include_directories(ram PUBLIC
    $<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>
)

# Compiler options
target_compile_options(ram PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op>
    $<$<CXX_COMPILER_ID:Clang>:-Wduplicated-cond>
)
