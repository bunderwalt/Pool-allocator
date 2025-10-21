cmake_minimum_required(VERSION 3.31)
project(PoolAllocator C)

set(CMAKE_C_STANDARD 11)

add_executable(PoolAllocator main.c
        pool_allocator.h
        pool_allocator.c)
