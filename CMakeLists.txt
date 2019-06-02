cmake_minimum_required(VERSION 3.2)

project(HW5-Shell-Sorting)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(Sorting main.cpp)

set_target_properties(Sorting PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
