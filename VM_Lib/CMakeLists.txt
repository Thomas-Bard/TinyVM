cmake_minimum_required(VERSION 3.28)

# VM library - static library
add_library(vm_lib STATIC)

# Add source files
file(GLOB_RECURSE VM_SOURCES "src/*.cpp")
target_sources(vm_lib PRIVATE ${VM_SOURCES})

# Add include directory (public)
target_include_directories(vm_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link against common library
target_link_libraries(vm_lib PUBLIC common ram cpu console)

# Only expose common's public headers through VM_Lib's public headers
target_include_directories(cpu PRIVATE
    $<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:cpu,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ram,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:console,INTERFACE_INCLUDE_DIRECTORIES>
)

# Compiler options
target_compile_options(vm_lib PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op>
    $<$<CXX_COMPILER_ID:Clang>:-Wduplicated-cond>
)
