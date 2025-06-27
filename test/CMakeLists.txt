cmake_minimum_required(VERSION 3.30)

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

add_executable(
    bark_test
    source/asio_client_tests.cpp
    source/client_tests.cpp
    source/count_tests.cpp
    source/details/udp_server.cpp
    source/event_tests.cpp
    source/gauge_tests.cpp
    source/histogram_tests.cpp
    source/main.cpp
    source/mpmc_client_tests.cpp
    source/spsc_client_tests.cpp
    source/tags_tests.cpp
    source/udp_client_tests.cpp
)
target_link_libraries(bark_test PRIVATE twig::bark doctest::doctest)
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
