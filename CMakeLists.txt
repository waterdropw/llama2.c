cmake_minimum_required(VERSION 3.12)

project(llama.c)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS_DEBUG "-g")

option(LLAMA_C_O3   "llama.c: enable -Ofast flag"               ON)
option(LLAMA_C_NATIVE  "llama.c: enable -march=native flag"        ON)
option(LLAMA_C_OFAST   "llama.c: enable -Ofast flag"               OFF)
option(LLAMA_C_DEBUG  "llama.c: enable -g flag"        OFF)
option(LLAMA_C_OMP  "llama.c: enable -fopenmp flag"        OFF)
option(LLAMA_C_GNU  "llama.c: enable -std=gnu11 flag"        OFF)

# Add the executable target
add_executable(run run.c)

# Enable flags
if (LLAMA_C_NATIVE)
    target_compile_options(run PRIVATE -march=native)
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
