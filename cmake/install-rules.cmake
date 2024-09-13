if (PROJECT_IS_TOP_LEVEL)
    set(CMAKE_INSTALL_INCLUDEDIR
        "include/cpp-datadog-client-${PROJECT_VERSION}"
        CACHE STRING ""
    )
    set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif ()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package twig-cpp-datadog-client)

install(
    DIRECTORY include/ "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT cpp-datadog-client_Development
)

install(
    TARGETS twig-cpp-datadog-client
    EXPORT cpp-datadog-clientTargets
    RUNTIME #
            COMPONENT cpp-datadog-client_Runtime
    LIBRARY #
            COMPONENT cpp-datadog-client_Runtime NAMELINK_COMPONENT cpp-datadog-client_Development
    ARCHIVE #
            COMPONENT cpp-datadog-client_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file("${package}ConfigVersion.cmake" COMPATIBILITY SameMajorVersion)

# Allow package maintainers to freely override the path for the configs
set(cpp-datadog-client_INSTALL_CMAKEDIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE cpp-datadog-client_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(cpp-datadog-client_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${cpp-datadog-client_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT cpp-datadog-client_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${cpp-datadog-client_INSTALL_CMAKEDIR}"
    COMPONENT cpp-datadog-client_Development
)

install(
    EXPORT cpp-datadog-clientTargets
    NAMESPACE twig::
    DESTINATION "${cpp-datadog-client_INSTALL_CMAKEDIR}"
    COMPONENT cpp-datadog-client_Development
)

if (PROJECT_IS_TOP_LEVEL)
    include(CPack)
endif ()
