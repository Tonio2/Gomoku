cmake_minimum_required(VERSION 3.4)
project(pygomoku)

add_subdirectory(external/pybind11)
pybind11_add_module(pygomoku py_gomoku.cpp gomoku_engine.cpp)