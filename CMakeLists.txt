cmake_minimum_required(VERSION 3.2)

project(HW3)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(ALL_TARGETS Euklid Power Primes)

foreach(TARGET_NAME ${ALL_TARGETS})
  add_executable(${TARGET_NAME} "${TARGET_NAME}.cpp")

  set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
  )
endforeach()
