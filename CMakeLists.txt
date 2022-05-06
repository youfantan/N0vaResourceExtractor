cmake_minimum_required(VERSION 3.16)
project(N0vaResourceExtractor)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS -static)
IF (WIN32)
    execute_process(COMMAND windres N0vaResourceExtractor.exe.rc -o app.o WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_executable(N0vaResourceExtractor_windows main.cpp app.o def.h)
ENDIF ()