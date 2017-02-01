cmake_minimum_required(VERSION 3.6)
project(babyos)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/cpuid/cpuid.c
        src/debug/debug.h
        src/include/common.h
        src/include/processes.h
        src/libraries/kernel/processes.c
        src/process-manager/architecture-186.h
        src/process-manager/architecture-x86-32.h
        src/process-manager/architecture.h
        src/process-manager/context.c
        src/process-manager/context.h
        src/process-manager/interrupts.c
        src/process-manager/interrupts.h
        src/process-manager/memory.c
        src/process-manager/memory.h
        src/process-manager/mocks.c
        src/process-manager/mocks.h
        src/process-manager/process-manager.c
        src/process-manager/process-manager.h
        src/process-manager/process-pool.c
        src/process-manager/process-pool.h
        src/process-manager/process-queue.c
        src/process-manager/process-queue.h
        src/process-manager/process-storage.c
        src/process-manager/process-storage.h
        src/process-manager/process.c
        src/process-manager/process.h
        src/process-manager/syscalls.c
        src/process-manager/syscalls.h
        src/process-manager/timer.c
        src/process-manager/timer.h)

include_directories(src/include)

add_executable(babyos ${SOURCE_FILES})