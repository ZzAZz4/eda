cmake_minimum_required(VERSION 3.20)
project(eda)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_executable(eda main.cpp)

target_compile_options(eda PUBLIC -Wall -Wextra -Werror
        -Wno-missing-field-initializers
        -Wno-attributes
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-error=effc++)
