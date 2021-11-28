cmake_minimum_required(VERSION 3.10)
project(eda)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_executable(eda main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(eda PRIVATE Threads::Threads stdc++fs)


target_compile_options(eda PUBLIC -Wall -Wextra -Werror
        -Wno-missing-field-initializers
        -Wno-attributes
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-error=effc++)

