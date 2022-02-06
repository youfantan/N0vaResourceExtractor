cmake_minimum_required(VERSION 3.16)
project(N0vaResourceExtractor)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(N0vaResourceExtractor main.cpp)
