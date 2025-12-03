cmake_minimum_required(VERSION 4.0)

project(poly_file_manager)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./include)

#add_executable(main src/main.cpp src/file_manager.cpp)

add_executable(basic_test1 test/basic_test1.cpp src/file_manager.cpp)