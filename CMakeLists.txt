cmake_minimum_required(VERSION 3.2)

project(HW9-Random-Binary-Tree)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(RBT main.cpp)

set_target_properties(RBT PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
