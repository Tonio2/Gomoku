cmake_minimum_required(VERSION 3.4)
project(pygomoku)

# Set the default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include_directories(inc)

add_subdirectory(external/pybind11)
pybind11_add_module(pygomoku src/Timer.cpp src/py_gomoku.cpp src/gomoku_engine.cpp src/gomoku_pattern_reconizer.cpp src/AI1.cpp)

# Set compiler options for performance optimization
if(MSVC)
  # Using MSVC, set /O2 for maximizing speed and consider /fp:fast for faster floating-point arithmetic
  target_compile_options(pygomoku PRIVATE /O2 /fp:fast)
else()
  # Assuming GCC or Clang, set -O3 for full optimization and -march=native to generate code optimized for the local machine
  target_compile_options(pygomoku PRIVATE -O3 -march=native)
endif()

# Optionally, add -flto for link-time optimization if supported
# include(CheckIPOSupported)
# check_ipo_supported(RESULT result)
# if(result)
#   set_property(TARGET pygomoku PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# endif()
