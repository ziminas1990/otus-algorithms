cmake_minimum_required(VERSION 3.2)

project(HW15-Deikstra)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(Deikstra main.cpp DotGenerator.cpp)

set_target_properties(Deikstra PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
