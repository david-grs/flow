cmake_minimum_required(VERSION 2.8)

project(flow)

set(cxx_compile_options -std=c++14 -g -Wall -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef -Wno-switch-default -Wold-style-cast -Wshadow -Wdouble-promotion)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(cxx_compile_options ${cxx_compile_options} -Wuseless-cast -Wduplicated-cond -Wlogical-op)
endif()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(tests unit_tests.cc)
target_compile_options(tests PRIVATE ${cxx_compile_options})
target_link_libraries(tests ${GTEST_BOTH_LIBRARIES} pthread)
