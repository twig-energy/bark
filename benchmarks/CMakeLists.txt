cmake_minimum_required(VERSION 3.14)

project(twig-cpp-datadog-clientBenchmarks LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)

# ---- Dependencies ----

if (PROJECT_IS_TOP_LEVEL)
    find_package(twig-cpp-datadog-client REQUIRED)
    enable_testing()
endif ()

find_package(benchmark CONFIG REQUIRED)

add_executable(
    twig-cpp-datadog-client_benchmarks
    source/client_benchmarks.cpp
    source/event_benchmarks.cpp
    source/metric_benchmarks.cpp
    source/udp_client_benchmarks.cpp
)
target_link_libraries(twig-cpp-datadog-client_benchmarks twig::cpp-datadog-client benchmark::benchmark_main)
