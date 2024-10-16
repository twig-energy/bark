cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)
project(
    bark
    VERSION 0.2.1
    DESCRIPTION "Datadog Agent client for C++"
    HOMEPAGE_URL "https://github.com/twig-energy/bark"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Developer mode ----
include(cmake/dev-mode.cmake)

# ---- Dependencies ----

find_package(asio CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_package(MPMCQueue CONFIG REQUIRED)
find_package(SPSCQueue CONFIG REQUIRED)

# ---- Declare library ----

add_library(
    bark
    source/asio_client.cpp
    source/client.cpp
    source/count.cpp
    source/event.cpp
    source/gauge.cpp
    source/histogram.cpp
    source/mpmc_client.cpp
    source/spsc_client.cpp
    source/udp_client.cpp
)
add_library(twig::bark ALIAS bark)

include(GenerateExportHeader)
generate_export_header(
    bark
    BASE_NAME
    bark
    EXPORT_FILE_NAME
    export/bark/export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE
    pragma_suppress_c4251
)

if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(bark PUBLIC BARK_STATIC_DEFINE)
endif ()

set_target_properties(
    bark
    PROPERTIES CXX_VISIBILITY_PRESET hidden
               VISIBILITY_INLINES_HIDDEN YES
               VERSION "${PROJECT_VERSION}"
               SOVERSION "${PROJECT_VERSION_MAJOR}"
               EXPORT_NAME bark
               OUTPUT_NAME bark
)

target_include_directories(bark ${warning_guard} PUBLIC "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

target_include_directories(bark SYSTEM PUBLIC "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>")

target_compile_features(bark PUBLIC cxx_std_23)

target_link_libraries(
    bark
    PUBLIC asio::asio MPMCQueue::MPMCQueue SPSCQueue::SPSCQueue
    PRIVATE fmt::fmt
)

# ---- Install rules ----

if (NOT CMAKE_SKIP_INSTALL_RULES)
    include(cmake/install-rules.cmake)
endif ()

# ---- Examples ----

if (PROJECT_IS_TOP_LEVEL)
    option(BUILD_EXAMPLES "Build examples tree." "${BARK_DEVELOPER_MODE}")
    if (BUILD_EXAMPLES)
        add_subdirectory(example)
    endif ()
endif ()

# ---- Code coverage ----
if (ENABLE_COVERAGE)
    target_code_coverage(bark ALL)
endif ()
