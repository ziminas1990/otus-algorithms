cmake_minimum_required(VERSION 3.2)

project(arrays)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(arrays main.cpp)

set_target_properties(arrays PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
