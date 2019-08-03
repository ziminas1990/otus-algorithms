cmake_minimum_required(VERSION 3.2)

project(HW11-Kosaraio)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(Kosaraio main.cpp DotGenerator.cpp AdjancencyVectorGraph.cpp)

set_target_properties(Kosaraio PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
