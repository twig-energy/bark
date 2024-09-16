cmake_minimum_required(VERSION 3.14)

project(twig-cpp-datadog-clientTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if (PROJECT_IS_TOP_LEVEL)
    find_package(twig-cpp-datadog-client REQUIRED)
    enable_testing()
endif ()

find_package(doctest REQUIRED)

# ---- Tests ----

add_executable(
    twig-cpp-datadog-client_test
    source/gauge_tests.cpp
    source/main.cpp
    source/tags_tests.cpp
    source/udp_tests.cpp
)
target_link_libraries(twig-cpp-datadog-client_test PRIVATE twig::cpp-datadog-client doctest::doctest)
target_compile_features(twig-cpp-datadog-client_test PRIVATE cxx_std_20)

add_test(NAME twig-cpp-datadog-client_test COMMAND twig-cpp-datadog-client_test)

target_compile_definitions(twig-cpp-datadog-client_test PRIVATE REPO_TEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data")

# ---- End-of-file commands ----

add_folders(Test)

# ---- Code coverage ----
if (ENABLE_COVERAGE)
    target_code_coverage(
        twig-cpp-datadog-client_test
        ALL
        EXCLUDE
        source
    )
endif ()
