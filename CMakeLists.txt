cmake_minimum_required(VERSION 4.0)

project(poly_file_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./include)

add_executable(main src/main.cpp src/file_manager.cpp)

add_executable(data_manager_test1 test/data_manager_test1.cpp src/data_manager.cpp)

add_executable(data_manager_test2 test/data_manager_test2.cpp src/data_manager.cpp)

if(WIN32)
add_executable(file_manager_test1 test/file_manager_test1.cpp src/file_manager.cpp src/windows_shortcut_creator.cpp)
    target_link_libraries(file_manager_test1 PRIVATE ole32 shell32)
endif()

if(WIN32)
add_executable(windows_shortcut_creator_test1 test/windows_shortcut_creator_test1.cpp src/windows_shortcut_creator.cpp)
    target_link_libraries(windows_shortcut_creator_test1 PRIVATE ole32 shell32)
endif()

add_executable(serializer_test1 test/serializer_test1.cpp src/serializer.cpp)

if(WIN32)
add_executable(poly_file_manager_test1 test/poly_file_manager_test1.cpp src/poly_file_manager.cpp src/data_manager.cpp src/file_manager.cpp src/serializer.cpp src/windows_shortcut_creator.cpp)
    target_link_libraries(poly_file_manager_test1 PRIVATE ole32 shell32)
endif()