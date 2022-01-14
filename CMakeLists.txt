cmake_minimum_required(VERSION 3.0.0)
project(another-rxcpp VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(include)

file(GLOB sources
  ./test/*.cpp
)

add_definitions(-DSUPPORTS_OPERATORS_IN_OBSERVABLE)
add_definitions(-DSUPPORTS_RXCPP_COMPATIBLE)

add_executable(another-rxcpp ${sources})

set(CMAKE_CXX_FLAGS "-std=c++14") # for clang
# set(CMAKE_CXX_FLAGS "-std=c++14 -pthread -g") # for gcc
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
