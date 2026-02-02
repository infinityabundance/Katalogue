cmake_minimum_required(VERSION 3.24)

project(Katalogue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Sql DBus Quick Qml Test)
find_package(Qt6 COMPONENTS LinguistTools)

qt_standard_project_setup()

enable_testing()

option(ENABLE_VVV_IMPORT "Build optional VVV catalog import tool" OFF)

set(KATALOGUE_TRANSLATIONS
    translations/katalogue_en.ts
)

if (Qt6LinguistTools_FOUND)
    qt_add_translations(katalogue_translations
        TS_FILES ${KATALOGUE_TRANSLATIONS}
    )
endif()

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
    src/common/katalogue_settings.cpp
    src/common/katalogue_settings.h
)

target_include_directories(katalogued PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daemon
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_link_libraries(katalogued
    katalogue-core
    Qt6::Core
    Qt6::DBus
)

add_executable(katalogue-export
    src/cli/katalogue_export_main.cpp
)

target_link_libraries(katalogue-export
    katalogue-core
    Qt6::Core
    Qt6::Sql
)

if(ENABLE_VVV_IMPORT)
    add_executable(katalogue-import-vvv
        src/cli/katalogue_import_vvv_main.cpp
    )

    target_link_libraries(katalogue-import-vvv
        katalogue-core
        Qt6::Core
        Qt6::Sql
    )
endif()

qt_add_executable(katalogue-gui
    src/gui/main.cpp
    src/gui/katalogue_client.cpp
    src/gui/katalogue_client.h
    src/common/katalogue_settings.cpp
    src/common/katalogue_settings.h
)

target_include_directories(katalogue-gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
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
        src/gui/qml/FileDetailsDrawer.qml
        src/gui/qml/EmptyCatalogPlaceholder.qml
        src/gui/qml/PreferencesPage.qml
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

add_subdirectory(tests)

install(TARGETS katalogue-gui katalogued katalogue-export
        RUNTIME DESTINATION bin)

if(ENABLE_VVV_IMPORT)
    install(TARGETS katalogue-import-vvv
            RUNTIME DESTINATION bin)
endif()

install(FILES data/org.kde.katalogue.desktop
        DESTINATION share/applications)

install(FILES data/org.kde.katalogue.metainfo.xml
        DESTINATION share/metainfo)

install(FILES src/daemon/org.kde.Katalogue1.xml
        DESTINATION share/dbus-1/interfaces)

install(FILES systemd/katalogued.service
        DESTINATION lib/systemd/user)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data/icons/hicolor/48x48/apps/org.kde.katalogue.png")
    install(FILES data/icons/hicolor/48x48/apps/org.kde.katalogue.png
            DESTINATION share/icons/hicolor/48x48/apps)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data/icons/hicolor/scalable/apps/org.kde.katalogue.svg")
    install(FILES data/icons/hicolor/scalable/apps/org.kde.katalogue.svg
            DESTINATION share/icons/hicolor/scalable/apps)
endif()
