cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    twig-cpp-datadog-client
    VERSION 0.0.6
    DESCRIPTION "Datadog Agent client for C++"
    HOMEPAGE_URL "https://github.com/twig-energy/cpp-datadog-client"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Developer mode ----
include(cmake/dev-mode.cmake)

# ---- Dependencies ----

find_package(absl CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_package(MPMCQueue CONFIG REQUIRED)
find_package(SPSCQueue CONFIG REQUIRED)

# ---- Declare library ----

add_library(
    twig-cpp-datadog-client
    source/client.cpp
    source/count.cpp
    source/event.cpp
    source/gauge.cpp
    source/histogram.cpp
    source/mpmc_client.cpp
    source/spsc_client.cpp
    source/udp_client.cpp
)
add_library(twig::cpp-datadog-client ALIAS twig-cpp-datadog-client)

include(GenerateExportHeader)
generate_export_header(
    twig-cpp-datadog-client
    BASE_NAME
    twig-cpp-datadog-client
    EXPORT_FILE_NAME
    export/twig/datadog/export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE
    pragma_suppress_c4251
)

if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(twig-cpp-datadog-client PUBLIC TWIG_CPP_DATADOG_CLIENT_STATIC_DEFINE)
endif ()

set_target_properties(
    twig-cpp-datadog-client
    PROPERTIES CXX_VISIBILITY_PRESET hidden
               VISIBILITY_INLINES_HIDDEN YES
               VERSION "${PROJECT_VERSION}"
               SOVERSION "${PROJECT_VERSION_MAJOR}"
               EXPORT_NAME cpp-datadog-client
               OUTPUT_NAME cpp-datadog-client
)

target_include_directories(twig-cpp-datadog-client ${warning_guard} PUBLIC "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

target_include_directories(twig-cpp-datadog-client SYSTEM PUBLIC "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>")

target_compile_features(twig-cpp-datadog-client PUBLIC cxx_std_23)

target_link_libraries(
    twig-cpp-datadog-client
    PUBLIC absl::civil_time
           absl::container_memory
           absl::time
           asio::asio
           MPMCQueue::MPMCQueue
           SPSCQueue::SPSCQueue
    PRIVATE fmt::fmt
)

# ---- Install rules ----

if (NOT CMAKE_SKIP_INSTALL_RULES)
    include(cmake/install-rules.cmake)
endif ()

# ---- Examples ----

if (PROJECT_IS_TOP_LEVEL)
    option(BUILD_EXAMPLES "Build examples tree." "${TWIG_CPP_DATADOG_CLIENT_DEVELOPER_MODE}")
    if (BUILD_EXAMPLES)
        add_subdirectory(example)
    endif ()
endif ()

# ---- Code coverage ----
if (ENABLE_COVERAGE)
    target_code_coverage(twig-cpp-datadog-client ALL)
endif ()
