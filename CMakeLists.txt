cmake_minimum_required(VERSION 3.21)

project(plcc)

set(CMAKE_C_STANDARD 11)

aux_source_directory(. SRC_LIST)

add_executable(plcc ${SRC_LIST})