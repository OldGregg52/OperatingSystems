cmake_minimum_required(VERSION 2.8)
project(Main)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
target_link_libraries(lab2.cpp pthread)
set(CMAKE_CXX_FLAGS -std=c++14)
add_executable(Main lab2.cpp)
