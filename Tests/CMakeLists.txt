cmake_minimum_required(VERSION 3.28)

# ── Helper macro ─────────────────────────────────────────────────────────────
# Creates one executable per .cpp file found under `dir`, linking each against
# `lib` (which already carries all transitive include paths and dependencies).
macro(add_tests_for dir lib)
    file(GLOB TEST_SOURCES "${dir}/*.cpp")
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${TEST_NAME} PRIVATE ${lib})
        target_compile_options(${TEST_NAME} PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op>
            $<$<CXX_COMPILER_ID:Clang>:-Wduplicated-cond>
        )
    endforeach()
endmacro()

# ── Per-subsystem test executables ────────────────────────────────────────────
add_tests_for(console console)
add_tests_for(cpu     cpu)
