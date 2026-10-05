cmake_minimum_required(VERSION 3.18)
project(mandelbrot LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

option(BUILD_TESTS "Build the C++ tests" ON)

find_package(Threads REQUIRED)
find_package(Python COMPONENTS Interpreter Development.Module Development.Embed REQUIRED)
execute_process(COMMAND ${Python_EXECUTABLE} -m pybind11 --cmakedir
                OUTPUT_VARIABLE pybind11_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
find_package(pybind11 CONFIG REQUIRED)

# C++ -> Python: program computes, python/plot.py draws
add_executable(mandelbrot cpp/main.cpp)
target_link_libraries(mandelbrot PRIVATE pybind11::embed Threads::Threads)

# Python -> C++: module "mandelbrot", built into build/python
pybind11_add_module(mandelbrot_python cpp/module.cpp)
target_link_libraries(mandelbrot_python PRIVATE Threads::Threads)
set_target_properties(mandelbrot_python PROPERTIES
  OUTPUT_NAME mandelbrot
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/python)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests/cpp)
endif()
