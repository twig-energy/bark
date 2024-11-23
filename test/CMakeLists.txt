cmake_minimum_required(VERSION 3.14)

project(barkTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if (PROJECT_IS_TOP_LEVEL)
    find_package(bark REQUIRED)
    enable_testing()
endif ()

find_package(doctest REQUIRED)

# ---- Tests ----

add_library(bark_test_lib source/details/udp_server.cpp source/details/uds_server.cpp)
target_link_libraries(bark_test_lib PUBLIC twig::bark)

add_executable(
    bark_test
    source/client_tests.cpp
    source/count_tests.cpp
    source/event_tests.cpp
    source/gauge_tests.cpp
    source/histogram_tests.cpp
    source/main.cpp
    source/mpmc_client_tests.cpp
    source/spsc_client_tests.cpp
    source/tags_tests.cpp
    source/transports/async_udp_transport_tests.cpp
    source/transports/async_uds_transport_tests.cpp
    source/transports/udp_transport_tests.cpp
    source/transports/uds_transport_tests.cpp
)
target_link_libraries(bark_test PRIVATE bark_test_lib doctest::doctest)
target_compile_features(bark_test PRIVATE cxx_std_20)

add_test(NAME bark_test COMMAND bark_test)

target_compile_definitions(bark_test PRIVATE REPO_TEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data")

# ---- End-of-file commands ----

add_folders(Test)

# ---- Code coverage ----
if (ENABLE_COVERAGE)
    target_code_coverage(
        bark_test
        ALL
        EXCLUDE
        source
    )
endif ()
