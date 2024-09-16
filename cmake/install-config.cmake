include(CMakeFindDependencyMacro)
find_dependency(absl)
find_dependency(asio)
find_dependency(fmt)
find_dependency(mpmcqueue)
find_dependency(spscqueue)

include("${CMAKE_CURRENT_LIST_DIR}/cpp-datadog-clientTargets.cmake")
