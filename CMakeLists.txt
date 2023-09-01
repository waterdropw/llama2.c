cmake_minimum_required(VERSION 3.12)

project(llama.c)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
# Fetch the cpuinfo library
FetchContent_Declare(
        cpuinfo
        GIT_REPOSITORY https://github.com/pytorch/cpuinfo.git
        GIT_TAG main)
set(CPUINFO_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(CPUINFO_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(CPUINFO_BUILD_MOCK_TESTS OFF CACHE BOOL "" FORCE)
set(CPUINFO_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cpuinfo)

option(LLAMA_C_PERF   "llama.c: enable perf optimization"       ON)
option(LLAMA_C_O3   "llama.c: enable -O3 flag"                  ON)
option(LLAMA_C_NATIVE  "llama.c: enable -march=native flag"     OFF)
option(LLAMA_C_OFAST   "llama.c: enable -Ofast flag"            OFF)
option(LLAMA_C_DEBUG  "llama.c: enable -g flag"                 OFF)
option(LLAMA_C_OMP  "llama.c: enable -fopenmp flag"             OFF)
option(LLAMA_C_GNU  "llama.c: enable -std=gnu11 flag"           OFF)
option(LLAMA_C_NEON "llama.c: enable NEON"                      OFF)

file(GLOB_RECURSE OPTSRC src/*.c)

# Add the executable target
add_executable(run run.c ${OPTSRC})
target_link_libraries(run PRIVATE cpuinfo)
target_include_directories(run PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Enable flags
if (LLAMA_C_NATIVE)
    target_compile_options(run PRIVATE -march=native -mavx2)
endif()
if (LLAMA_C_O3)
    target_compile_options(run PRIVATE -O3)
endif()
if (LLAMA_C_OFAST)
    target_compile_options(run PRIVATE -Ofast)
endif()
if (LLAMA_C_DEBUG)
    set(LLAMA_C_O3 OFF)
    set(LLAMA_C_OFAST OFF)
    target_compile_options(run PRIVATE -g)
endif()
if (LLAMA_C_OMP)
    # The OpenMP-enabled build
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
    target_compile_options(run PRIVATE -fopenmp)
endif()
if (LLAMA_C_GNU)
    target_compile_options(run PRIVATE -std=gnu11)
endif ()
if (NOT LLAMA_C_NEON)
    target_compile_definitions(run PRIVATE DISABLE_NEON)
endif ()
if (LLAMA_C_PERF)
    target_compile_definitions(run PRIVATE ENABLE_PERF)
endif ()