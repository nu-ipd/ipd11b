# Configuration for building example C programs.
cmake_minimum_required(VERSION 3.10)
project(ipd11b C)
include(.ipd/cmake/CMakeLists.txt)

add_c_program(oops
    src/oops.c
    ASAN)

add_c_program(asan_example_without_asan
    src/asan_example.c)

add_c_program(asan_example_with_asan
    src/asan_example.c
    ASAN)

add_c_test_program(test_cons
    test/test_cons.c
    src/cons.c
    UBSAN)

set(GEO_LIB src/heap_posn.c
            src/owning_tri.c
            src/borrow_tri.c)

add_c_program(geo_client
    src/geo_client.c
    ${GEO_LIB})

add_c_program(geo_client_bt
    src/geo_client.c
    ${GEO_LIB}
    DEFINES BORROWING_TRI)
