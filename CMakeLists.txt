cmake_minimum_required(VERSION 3.0.0)
project(utf-utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)


add_executable(
    utf-utils
    src/test.cpp
)

target_compile_definitions(
    utf-utils
    PRIVATE
    IMPLEMENT_UTFUTILS
)