cmake_minimum_required(VERSION 3.24)

project(Katalogue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Sql DBus Quick Qml)

qt_standard_project_setup()

enable_testing()

add_library(katalogue-core
    src/core/katalogue_database.cpp
    src/core/katalogue_scanner.cpp
)

target_include_directories(katalogue-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_link_libraries(katalogue-core
    Qt6::Core
    Qt6::Sql
)

add_executable(katalogued
    src/daemon/main.cpp
    src/daemon/katalogue_daemon.cpp
)

target_include_directories(katalogued PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daemon
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_link_libraries(katalogued
    katalogue-core
    Qt6::Core
    Qt6::DBus
)

qt_add_executable(katalogue-gui
    src/gui/main.cpp
    src/gui/katalogue_client.cpp
    src/gui/katalogue_client.h
)

target_include_directories(katalogue-gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_link_libraries(katalogue-gui
    katalogue-core
    Qt6::Core
    Qt6::DBus
    Qt6::Quick
    Qt6::Qml
)

qt_add_qml_module(katalogue-gui
    URI org.kde.Katalogue
    VERSION 1.0
    QML_FILES
        src/gui/qml/Main.qml
        src/gui/qml/VolumeSidebar.qml
        src/gui/qml/DirectoryTree.qml
        src/gui/qml/FileView.qml
        src/gui/qml/VirtualFoldersSidebar.qml
        src/gui/qml/VirtualFolderItemsView.qml
)

add_executable(core_test
    src/core/core_test.cpp
)

target_link_libraries(core_test
    katalogue-core
    Qt6::Core
    Qt6::Sql
)

add_test(NAME core_test COMMAND core_test)
