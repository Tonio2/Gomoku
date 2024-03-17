cmake_minimum_required(VERSION 3.4)
project(pygomoku)

include_directories(inc)

add_subdirectory(external/pybind11)
pybind11_add_module(pygomoku src/py_gomoku.cpp src/gomoku_engine.cpp src/gomoku_structure_pattern.cpp src/AI1.cpp)
