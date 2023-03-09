cmake_minimum_required(VERSION 3.12)
project(rtmp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory("submodule")
add_executable(${PROJECT_NAME} 
src/main.cpp 
src/foo.cpp)