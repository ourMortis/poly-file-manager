set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 COMPONENTS Multimedia QUIET)

qt_add_executable(qt_gui WIN32
    src/main.cpp
    src/mainwindow.cpp
    src/importdialog.cpp
    src/edittagsdialog.cpp
    src/repomanagerdialog.cpp
    src/appconfig.cpp
    src/settingsdialog.cpp
    src/tagdelegate.cpp
    src/thumbnailer.cpp
    include/mainwindow.hpp
    include/importdialog.hpp
    include/edittagsdialog.hpp
    include/repomanagerdialog.hpp
    include/appconfig.hpp
    include/settingsdialog.hpp
    include/tagdelegate.hpp
    include/thumbnailer.hpp
)

target_include_directories(qt_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(qt_gui PRIVATE
    poly_core
    Qt6::Widgets
)

if(Qt6Multimedia_FOUND)
    target_link_libraries(qt_gui PRIVATE Qt6::Multimedia)
    target_compile_definitions(qt_gui PRIVATE HAVE_QT_MULTIMEDIA)
endif()

# Deploy the Qt runtime DLLs (and plugins) next to the executable so it can be
# started without adding the Qt bin directory to PATH. Best-effort only.
if(WIN32)
    get_target_property(_qmake_target Qt6::qmake IMPORTED_LOCATION)
    if(_qmake_target)
        get_filename_component(_qt_bin "${_qmake_target}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS "${_qt_bin}")
        if(WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET qt_gui POST_BUILD
                COMMAND "${WINDEPLOYQT_EXECUTABLE}" --release "$<TARGET_FILE:qt_gui>"
                COMMENT "Deploying Qt runtime beside qt_gui")
        endif()
    endif()
endif()
