cmake_minimum_required(VERSION 3.10)
project(eda)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_executable(eda main.cpp)


if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O3")
endif(CMAKE_COMPILER_IS_GNUCC)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(eda PRIVATE Threads::Threads stdc++fs)


target_compile_options(eda PUBLIC -Wall -Wextra -Werror
        -Wno-missing-field-initializers
        -Wno-attributes
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-stringop-truncation
        -Wno-error=effc++)
        