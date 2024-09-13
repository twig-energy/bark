include(CMakeFindDependencyMacro)
find_dependency(absl)
find_dependency(fmt)

include("${CMAKE_CURRENT_LIST_DIR}/cpp-datadog-clientTargets.cmake")
