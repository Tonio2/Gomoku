cmake_minimum_required(VERSION 3.10)
project(GomokuTests)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB_RECURSE UTILITIES_SOURCES "../src/utils/*.cpp")

############ MATRIX TESTS ############

file(GLOB_RECURSE MATRIX_SOURCES "../src/matrix/*.cpp")

add_executable(matrix_tests matrix_test.cpp ${MATRIX_SOURCES})

target_link_libraries(matrix_tests ${GTEST_LIBRARIES} pthread)

add_test(test matrix_tests)

############ TIMER TESTS ############

file(GLOB_RECURSE TIMER_SOURCES "../src/timer/*.cpp")

add_executable(timer_tests timer_test.cpp ${TIMER_SOURCES})

target_link_libraries(timer_tests ${GTEST_LIBRARIES} pthread)

add_test(test timer_tests)

############ ENGINE TESTS ############

file(GLOB_RECURSE ENGINE_SOURCES "../src/engine/*.cpp" ${UTILITIES_SOURCES})

add_executable(engine_tests engine_test.cpp ${ENGINE_SOURCES} ${TIMER_SOURCES} ${MATRIX_SOURCES})

target_link_libraries(engine_tests ${GTEST_LIBRARIES} pthread)

add_test(test engine_tests)

############ AI TESTS ############

file(GLOB_RECURSE AI_SOURCES "../src/ai/*.cpp" ${UTILITIES_SOURCES})

add_executable(ai_tests ai_test.cpp ${AI_SOURCES} ${ENGINE_SOURCES} ${TIMER_SOURCES} ${MATRIX_SOURCES})

target_link_libraries(ai_tests ${GTEST_LIBRARIES} pthread)

add_test(test ai_tests)

############ ROOM TESTS ############

file(GLOB_RECURSE ROOM_SOURCES "../src/room/*.cpp" ${UTILITIES_SOURCES})

add_executable(room_tests room_test.cpp ${ROOM_SOURCES} ${AI_SOURCES} ${ENGINE_SOURCES} ${TIMER_SOURCES} ${MATRIX_SOURCES})

target_link_libraries(room_tests ${GTEST_LIBRARIES} pthread)

add_test(test room_tests)

############ UTILS TESTS ############

file(GLOB_RECURSE UTILS_SOURCES "../src/utils/*.cpp" ${UTILITIES_SOURCES})

add_executable(utils_tests utils_test.cpp ${UTILS_SOURCES} ${AI_SOURCES} ${ENGINE_SOURCES} ${TIMER_SOURCES} ${MATRIX_SOURCES})

target_link_libraries(utils_tests ${GTEST_LIBRARIES} pthread)

add_test(test utils_tests)
