cmake_minimum_required(VERSION 3.5)
project(teplica-api)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(teplica-api main.cpp)
target_link_libraries(teplica-api Threads::Threads)
