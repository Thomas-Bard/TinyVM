cmake_minimum_required(VERSION 3.28)

# CPU library - static library
add_library(console STATIC)

# Add source files
file(GLOB_RECURSE CONSOLE_SOURCES "src/*.cpp")
target_sources(console PRIVATE ${CONSOLE_SOURCES})

# Add include directory (public)
target_include_directories(console PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)



# Link against common library
target_link_libraries(console PUBLIC common SDL3_ttf::SDL3_ttf SDL3::SDL3)

# Only expose common's public headers through CPU's public headers
target_include_directories(console PUBLIC
    $<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>
)

# Compiler options
target_compile_options(console PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op>
    $<$<CXX_COMPILER_ID:Clang>:-Wduplicated-cond>
)
