cmake_minimum_required(VERSION 4.0)
project(poly_file_manager LANGUAGES CXX)
include_directories(include)

add_executable(
    file_meta_test
    test/file_meta_test.cpp
    src/file_meta.cpp
    src/poly_file_manager_error.cpp
)


