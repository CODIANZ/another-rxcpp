cmake_minimum_required(VERSION 3.0.0)
project(another-rxcpp VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(include)

file(GLOB sources
  ./test/*.cpp
)

add_executable(another-rxcpp ${sources})

set(CMAKE_CXX_FLAGS "-std=c++14")
set(CMAKE_CXX_FLAGS "-pthread") # for gcc
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
