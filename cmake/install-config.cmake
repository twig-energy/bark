include(CMakeFindDependencyMacro)
find_dependency(boost COMPONENTS asio)
find_dependency(fmt)
find_dependency(MPMCQueue)
find_dependency(SPSCQueue)

include("${CMAKE_CURRENT_LIST_DIR}/barkTargets.cmake")
