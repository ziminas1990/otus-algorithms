cmake_minimum_required(VERSION 3.2)

project(HW8-AVL-Tree)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(AVL main.cpp)

set_target_properties(AVL PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
