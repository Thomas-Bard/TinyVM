cmake_minimum_required(VERSION 3.28)
project(TinyVM)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type options
set(CMAKE_CONFIGURATION_TYPES Debug MinDebug Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Ninja generator
set(CMAKE_GENERATOR Ninja)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler flags - common to all configurations
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic -Wshadow -Wconversion")

# Debug configuration
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-fsanitize=address")

# MinDebug configuration (Debug without sanitizer)
set(CMAKE_CXX_FLAGS_MINDEBUG "-g -O0")
set(CMAKE_EXE_LINKER_FLAGS_MINDEBUG "")

# Release configuration
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add subdirectories
add_subdirectory(common)
add_subdirectory(CPU)
add_subdirectory(RAM)
add_subdirectory(ASM)
add_subdirectory(VM)

# Generate compile_commands.json symlink at root on first configure
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(symlink_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
        COMMENT "Creating symlink to compile_commands.json at project root"
    )
endif()
