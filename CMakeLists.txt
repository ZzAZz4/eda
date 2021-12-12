cmake_minimum_required(VERSION 3.10)
project(eda)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_executable(eda main.cpp)

target_include_directories(eda PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(eda PUBLIC -Wall -Wextra -Werror
        -Wno-missing-field-initializers
        -Wno-attributes
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-stringop-truncation
        -Wno-error=effc++)
        