cmake_minimum_required(VERSION 4.0)

project(poly_file_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./include/core ./include/cli)

# set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# add_executable(data_manager_test1 test/data_manager_test1.cpp src/core/data_manager.cpp)

# add_executable(data_manager_test2 test/data_manager_test2.cpp src/core/data_manager.cpp)

# if(WIN32)
# add_executable(file_manager_test1 test/file_manager_test1.cpp src/core/file_manager.cpp src/core/windows_shortcut_creator.cpp)
#     target_link_libraries(file_manager_test1 PRIVATE ole32 shell32)
# endif()

# if(WIN32)
# add_executable(windows_shortcut_creator_test1 test/windows_shortcut_creator_test1.cpp src/core/windows_shortcut_creator.cpp)
#     target_link_libraries(windows_shortcut_creator_test1 PRIVATE ole32 shell32)
# endif()

# add_executable(serializer_test1 test/serializer_test1.cpp src/core/serializer.cpp)

# if(WIN32)
# add_executable(poly_file_manager_test1 test/poly_file_manager_test1.cpp src/core/poly_file_manager.cpp src/core/data_manager.cpp src/core/file_manager.cpp src/core/serializer.cpp src/core/windows_shortcut_creator.cpp)
#     target_link_libraries(poly_file_manager_test1 PRIVATE ole32 shell32)
# endif()

if(WIN32)
add_executable(pfm src/cli/main.cpp src/core/poly_file_manager.cpp src/core/data_manager.cpp src/core/file_manager.cpp src/core/serializer.cpp src/core/windows_shortcut_creator.cpp src/cli/cli_parser.cpp src/cli/manager_cmd.cpp src/cli/tag_cmd.cpp src/cli/path_cmd.cpp)
    target_link_libraries(pfm PRIVATE ole32 shell32)
endif()

