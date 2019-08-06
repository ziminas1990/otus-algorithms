cmake_minimum_required(VERSION 3.2)

project(HW13-Demucron)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(Demucron main.cpp DotGenerator.cpp)

set_target_properties(Demucron PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
