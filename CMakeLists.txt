cmake_minimum_required(VERSION 4.0)

project(poly_file_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./include)

#add_executable(main src/main.cpp src/file_manager.cpp)

#add_executable(basic_test1 test/basic_test1.cpp src/file_manager.cpp)

# add_executable(organizer_test1 test/organizer_test1.cpp src/file_system_organizer.cpp src/windows_shortcut_creator.cpp)
# if(WIN32)
#    target_link_libraries(organizer_test1 PRIVATE ole32 shell32)
# endif()

# add_executable(windows_shortcut_creator_test1 test/windows_shortcut_creator_test1.cpp src/windows_shortcut_creator.cpp)
# if(WIN32)
#     target_link_libraries(windows_shortcut_creator_test1 PRIVATE ole32 shell32)
# endif()

add_executable(serializer_test1 test/serializer_test1.cpp src/serializer.cpp)

