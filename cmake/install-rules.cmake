if (PROJECT_IS_TOP_LEVEL)
    set(CMAKE_INSTALL_INCLUDEDIR
        "include/bark-${PROJECT_VERSION}"
        CACHE STRING ""
    )
    set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif ()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package bark)

install(
    DIRECTORY include/ "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT bark_Development
)

install(
    TARGETS bark
    EXPORT barkTargets
    RUNTIME #
            COMPONENT bark_Runtime
    LIBRARY #
            COMPONENT bark_Runtime NAMELINK_COMPONENT bark_Development
    ARCHIVE #
            COMPONENT bark_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILE_SET HEADERS
)

write_basic_package_version_file("${package}ConfigVersion.cmake" COMPATIBILITY SameMajorVersion)

# Allow package maintainers to freely override the path for the configs
set(bark_INSTALL_CMAKEDIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE bark_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(bark_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${bark_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT bark_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${bark_INSTALL_CMAKEDIR}"
    COMPONENT bark_Development
)

install(
    EXPORT barkTargets
    NAMESPACE twig::
    DESTINATION "${bark_INSTALL_CMAKEDIR}"
    COMPONENT bark_Development
)

if (PROJECT_IS_TOP_LEVEL)
    include(CPack)
endif ()
