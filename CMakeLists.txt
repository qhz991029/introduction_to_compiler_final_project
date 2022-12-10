cmake_minimum_required(VERSION 3.21)
project(introduction_to_compiler_final_project C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(introduction_to_compiler_final_project
        evl.c
        set.c
        PL0.c
        evl.h
        set.h
        PL0.h
        )
