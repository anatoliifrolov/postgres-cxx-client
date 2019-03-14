include(CMakeFindDependencyMacro)
find_dependency(PostgreSQL REQUIRED)
include("${CMAKE_CURRENT_LIST_DIR}/PostgresCxxClientTargets.cmake")
