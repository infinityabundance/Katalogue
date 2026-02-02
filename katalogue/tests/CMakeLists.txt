add_executable(tst_katalogue_database
    tst_katalogue_database.cpp
)

target_link_libraries(tst_katalogue_database
    PRIVATE
        katalogue-core
        Qt6::Test
        Qt6::Core
        Qt6::Sql
)

target_include_directories(tst_katalogue_database PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
)

add_test(NAME tst_katalogue_database COMMAND tst_katalogue_database)

add_executable(tst_katalogue_scanner
    tst_katalogue_scanner.cpp
)

target_link_libraries(tst_katalogue_scanner
    PRIVATE
        katalogue-core
        Qt6::Test
        Qt6::Core
        Qt6::Sql
)

target_include_directories(tst_katalogue_scanner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
)

add_test(NAME tst_katalogue_scanner COMMAND tst_katalogue_scanner)

add_executable(tst_katalogue_daemon
    tst_katalogue_daemon.cpp
    ../src/daemon/katalogue_daemon.cpp
    ../src/common/katalogue_settings.cpp
)

target_link_libraries(tst_katalogue_daemon
    PRIVATE
        katalogue-core
        Qt6::Test
        Qt6::Core
        Qt6::Sql
        Qt6::DBus
)

target_include_directories(tst_katalogue_daemon PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/daemon
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/common
)

add_test(NAME tst_katalogue_daemon COMMAND tst_katalogue_daemon)
