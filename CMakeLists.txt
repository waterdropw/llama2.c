cmake_minimum_required(VERSION 3.12)

project(llama.c)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

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
# Fetch mperf
FetchContent_Declare(
        mperf
        GIT_REPOSITORY https://github.com/MegEngine/mperf.git
        GIT_TAG master)
FetchContent_MakeAvailable(mperf)

option(LLAMA_C_OPT   "llama.c: enable optimization"             ON)
option(LLAMA_C_NEON "llama.c: enable NEON"                      ON)
option(LLAMA_C_TMA  "llama.c: enable mperf TMA analysis"        OFF)
option(LLAMA_C_O3   "llama.c: enable -O3 flag"                  ON)
option(LLAMA_C_NATIVE  "llama.c: enable -march=native flag"     OFF)
option(LLAMA_C_OFAST   "llama.c: enable -Ofast flag"            OFF)
option(LLAMA_C_OMP  "llama.c: enable -fopenmp flag"             OFF)
option(LLAMA_C_GNU  "llama.c: enable -std=gnu11 flag"           OFF)

# Add the executable target
add_executable(run run.c
        src/matvecmul_row.h
        src/matvecmul.c
        src/matvecmul_common.c
        src/matvecmul_neon.c
        src/matvecmul_neon64.c
        src/matvecmul_avx2.c)

target_link_libraries(run PRIVATE cpuinfo mperf)
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
if (LLAMA_C_OPT)
    target_compile_definitions(run PRIVATE ENABLE_OPT)
endif ()
if (LLAMA_C_TMA)
    target_compile_definitions(run PRIVATE ENABLE_TMA)
endif ()