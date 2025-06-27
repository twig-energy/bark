cmake_minimum_required(VERSION 3.30)

project(barkExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if (PROJECT_IS_TOP_LEVEL)
    find_package(bark REQUIRED)
endif ()

add_custom_target(run-examples)

function (add_example NAME)
    add_executable("${NAME}" "${NAME}.cpp")
    target_link_libraries("${NAME}" PRIVATE twig::bark)
    target_compile_features("${NAME}" PRIVATE cxx_std_23)
    set_source_files_properties("${NAME}.cpp" PROPERTIES SKIP_LINTING ON)

    add_custom_target(
        "run_${NAME}"
        COMMAND "${NAME}"
        VERBATIM
    )
    add_dependencies("run_${NAME}" "${NAME}")
    add_dependencies(run-examples "run_${NAME}")
endfunction ()

add_example(getting_started)

add_folders(Example)
