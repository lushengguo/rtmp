cmake_minimum_required(VERSION 3.12)
project(rtmp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR})
include_directories("reactor")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("reactor")
add_subdirectory("rtmp")