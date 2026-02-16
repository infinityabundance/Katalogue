# CMake generated Testfile for 
# Source directory: /home/one/Katalogue/Katalogue/katalogue/tests
# Build directory: /home/one/Katalogue/Katalogue/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[tst_katalogue_database]=] "/home/one/Katalogue/Katalogue/build/tests/tst_katalogue_database")
set_tests_properties([=[tst_katalogue_database]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/one/Katalogue/Katalogue/katalogue/tests/CMakeLists.txt;17;add_test;/home/one/Katalogue/Katalogue/katalogue/tests/CMakeLists.txt;0;")
add_test([=[tst_katalogue_scanner]=] "/home/one/Katalogue/Katalogue/build/tests/tst_katalogue_scanner")
set_tests_properties([=[tst_katalogue_scanner]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/one/Katalogue/Katalogue/katalogue/tests/CMakeLists.txt;35;add_test;/home/one/Katalogue/Katalogue/katalogue/tests/CMakeLists.txt;0;")
add_test([=[tst_katalogue_daemon]=] "/home/one/Katalogue/Katalogue/build/tests/tst_katalogue_daemon")
set_tests_properties([=[tst_katalogue_daemon]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/one/Katalogue/Katalogue/katalogue/tests/CMakeLists.txt;58;add_test;/home/one/Katalogue/Katalogue/katalogue/tests/CMakeLists.txt;0;")
add_test([=[bench_catalog]=] "/home/one/Katalogue/Katalogue/build/tests/bench_catalog")
set_tests_properties([=[bench_catalog]=] PROPERTIES  LABELS "benchmark" _BACKTRACE_TRIPLES "/home/one/Katalogue/Katalogue/katalogue/tests/CMakeLists.txt;75;add_test;/home/one/Katalogue/Katalogue/katalogue/tests/CMakeLists.txt;0;")
