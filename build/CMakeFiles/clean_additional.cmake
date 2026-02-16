# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/core_test_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/core_test_autogen.dir/ParseCache.txt"
  "CMakeFiles/katalogue-core_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/katalogue-core_autogen.dir/ParseCache.txt"
  "CMakeFiles/katalogue-export_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/katalogue-export_autogen.dir/ParseCache.txt"
  "CMakeFiles/katalogue-gui_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/katalogue-gui_autogen.dir/ParseCache.txt"
  "CMakeFiles/katalogued_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/katalogued_autogen.dir/ParseCache.txt"
  "core_test_autogen"
  "katalogue-core_autogen"
  "katalogue-export_autogen"
  "katalogue-gui_autogen"
  "katalogued_autogen"
  "tests/CMakeFiles/bench_catalog_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/bench_catalog_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/tst_katalogue_daemon_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/tst_katalogue_daemon_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/tst_katalogue_database_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/tst_katalogue_database_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/tst_katalogue_scanner_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/tst_katalogue_scanner_autogen.dir/ParseCache.txt"
  "tests/bench_catalog_autogen"
  "tests/tst_katalogue_daemon_autogen"
  "tests/tst_katalogue_database_autogen"
  "tests/tst_katalogue_scanner_autogen"
  )
endif()
