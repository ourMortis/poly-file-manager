cmake_minimum_required(VERSION 3.20)

project(poly_file_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include/core ${CMAKE_SOURCE_DIR}/include/cli)

# ---------------------------------------------------------------------------
# Core module: the domain classes, compiled as a reusable static library.
# ---------------------------------------------------------------------------
add_library(poly_core STATIC
    src/core/poly_file_manager.cpp
    src/core/data_manager.cpp
    src/core/file_manager.cpp
    src/core/serializer.cpp
    src/core/link_manager.cpp
)

target_include_directories(poly_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/core>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/cli>
)

if(WIN32)
    target_link_libraries(poly_core PUBLIC ole32 shell32)
endif()

if(WIN32)
    add_executable(pfm
        src/cli/main.cpp
        src/cli/cli.cpp
    )
    target_link_libraries(pfm PRIVATE poly_core)
endif()

# ---------------------------------------------------------------------------
# Optional Qt GUI (built only when Qt6 Widgets can be found with the active
# toolchain). Links against the core static library.
# ---------------------------------------------------------------------------
option(BUILD_GUI "Build the Qt graphical interface" ON)
if(BUILD_GUI)
    find_package(Qt6 COMPONENTS Widgets QUIET)
    if(Qt6_FOUND)
        add_subdirectory(gui)
    else()
        message(STATUS "Qt6 not found; GUI will not be built.")
    endif()
endif()
