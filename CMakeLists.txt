cmake_minimum_required(VERSION 3.2)

project(HW17-HeapManager)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(Heap main.cpp AwesomeHeap.cpp)

set_target_properties(Heap PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
