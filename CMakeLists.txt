cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
 
project(4k_comp)
set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS *.cpp)
    
file(GLOB HEADERS *.h)

add_executable(4k_comp ${SRCS} ${HEADERS})
