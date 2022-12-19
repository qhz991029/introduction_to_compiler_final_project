cmake_minimum_required(VERSION 3.22)

project(plcc)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

aux_source_directory(. SRC_LIST)

add_executable(plcc ${SRC_LIST})