include(cmake/folders.cmake)

if (NOT BARK_DEVELOPER_MODE)
    return()
elseif (NOT PROJECT_IS_TOP_LEVEL)
    message(AUTHOR_WARNING "Developer mode is intended for developers of bark")
endif ()

option(ENABLE_COVERAGE "Enable coverage support separate from CTest's" OFF)
if (ENABLE_COVERAGE)
    include(cmake/code-coverage.cmake)
    add_code_coverage_all_targets(EXCLUDE test/* example/*)
endif ()

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(test)
    add_subdirectory(benchmarks)
endif ()

include(cmake/lint-targets.cmake)
include(cmake/spell-targets.cmake)

add_folders(Project)
