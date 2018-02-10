cmake_minimum_required(VERSION 2.8)

project(flow)

set(cxx_compile_options -std=c++14 -g -Wall -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef -Wno-switch-default -Wold-style-cast -Wshadow -Wdouble-promotion)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(cxx_compile_options ${cxx_compile_options} -Wuseless-cast -Wduplicated-cond -Wlogical-op)
endif()

include(gtest.cmake)

add_executable(tests unit_tests.cc flow.h block.h block_factory.h)
target_compile_options(tests PRIVATE ${cxx_compile_options})

find_package (Threads)
target_link_libraries(tests gtest ${CMAKE_THREAD_LIBS_INIT})

