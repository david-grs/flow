cmake_minimum_required(VERSION 2.8)

project(flow)
add_executable(flow main.cpp)

set(cxx_compile_options -std=c++14 -g -Wall -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef -Wno-switch-default -Wold-style-cast -Wshadow -Wdouble-promotion)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(cxx_compile_options ${cxx_compile_options} -Wuseless-cast -Wduplicated-cond -Wlogical-op)
endif()

target_compile_options(flow PRIVATE ${cxx_compile_options})

