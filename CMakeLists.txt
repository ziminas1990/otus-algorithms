cmake_minimum_required(VERSION 3.2)

project(HW11-Kosaraja)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(Kosaraja main.cpp DotGenerator.cpp)

set_target_properties(Kosaraja PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
