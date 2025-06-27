cmake_minimum_required(VERSION 3.30)

project(barkBenchmarks LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)

# ---- Dependencies ----

if (PROJECT_IS_TOP_LEVEL)
    find_package(bark REQUIRED)
    enable_testing()
endif ()

find_package(benchmark CONFIG REQUIRED)

add_executable(
    bark_benchmarks
    source/client_benchmarks.cpp
    source/event_benchmarks.cpp
    source/metric_benchmarks.cpp
    source/udp_client_benchmarks.cpp
)
target_link_libraries(bark_benchmarks twig::bark benchmark::benchmark_main)
