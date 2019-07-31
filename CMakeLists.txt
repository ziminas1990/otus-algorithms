cmake_minimum_required(VERSION 3.2)

project(HW11-Hash-Tables)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(Hash main.cpp)

set_target_properties(Hash PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
