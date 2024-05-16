cmake_minimum_required(VERSION 3.10)
project(Gomoku)

# Set the default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add source files
file(GLOB_RECURSE MATRIX_SOURCES "src/matrix/*.cpp")
file(GLOB_RECURSE TIMER_SOURCES "src/timer/*.cpp")
file(GLOB_RECURSE ENGINE_SOURCES "src/engine/*.cpp")
file(GLOB_RECURSE AI_SOURCES "src/ai/*.cpp")
file(GLOB_RECURSE ROOM_SOURCES "src/room/*.cpp")
file(GLOB_RECURSE ARENA_SOURCES "src/arena/*.cpp")

file(GLOB_RECURSE UTILITIES_SOURCES "src/utils/*.cpp")

set(GOMOKU_SOURCES ${MATRIX_SOURCES} ${TIMER_SOURCES} ${ENGINE_SOURCES} ${AI_SOURCES} ${ROOM_SOURCES} ${UTILITIES_SOURCES})

set(GKTOOLFILE src/gktool.cpp)

set(PYBINDFILE src/pybinds.cpp)

add_subdirectory(external/pybind11)
pybind11_add_module(PyGomoku ${PYBINDFILE} ${GOMOKU_SOURCES})

# if(result)
#   set_property(TARGET pygomoku PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# endif()

add_executable(gktool ${GKTOOLFILE} ${GOMOKU_SOURCES} ${ARENA_SOURCES} ${UTILITIES_SOURCES})

find_package(GTest)

# Add tests if GTest is found
if(GTEST_FOUND)
    enable_testing()
    add_subdirectory(tests)
else()
    message("GTest not installed. Tests will not be compiled")
endif()
