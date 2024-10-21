cmake_minimum_required(VERSION 3.10)
project(ith_search)

set(CMAKE_CXX_STANDARD 20)
find_package(benchmark REQUIRED)

add_executable(bench benches.cpp)
target_link_libraries(bench benchmark::benchmark pthread)