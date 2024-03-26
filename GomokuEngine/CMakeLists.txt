cmake_minimum_required(VERSION 3.4)
project(pygomoku)

# Set the default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include_directories(inc)

add_subdirectory(external/pybind11)
pybind11_add_module(pygomoku src/Timer.cpp src/py_gomoku.cpp src/gomoku_engine.cpp src/gomoku_pattern_reconizer.cpp src/gomoku_ai.cpp src/gomoku_ai_data.cpp)

# Set compiler options for performance optimization
# Assuming GCC or Clang
# -O3 for full optimization 
# -march=native to generate code optimized for the local machine 
# -DNOTIMER to deactivate timers
target_compile_options(pygomoku PRIVATE -O3 -DNOTIMER -march=native -std=c++17)

# Optionally, add -flto for link-time optimization if supported
# include(CheckIPOSupported)
# check_ipo_supported(RESULT result)
# if(result)
#   set_property(TARGET pygomoku PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# endif()
