cmake_minimum_required(VERSION 3.2)

project(HW10-Splay-Binary-Tree)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(Splay main.cpp)

set_target_properties(Splay PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
