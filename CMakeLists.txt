cmake_minimum_required(VERSION 3.8)
project(PostgresCxxClient VERSION 0.1.0)

# Target.
add_library(PostgresCxxClient
        src/Channel.cpp
        src/Client.cpp
        src/Command.cpp
        src/Config.cpp
        src/Connection.cpp
        src/Consumer.cpp
        src/Context.cpp
        src/Dispatcher.cpp
        src/Error.cpp
        src/Field.cpp
        src/IChannel.cpp
        src/Job.cpp
        src/PrepareData.cpp
        src/PreparedCommand.cpp
        src/Receiver.cpp
        src/Result.cpp
        src/Row.cpp
        src/Statement.cpp
        src/Status.cpp
        src/Time.cpp
        src/Transaction.cpp
        src/Visitable.cpp
        src/Visitors.cpp
        src/Worker.cpp
        )

add_library(PostgresCxxClient::PostgresCxxClient ALIAS PostgresCxxClient)

# Requirements.
target_compile_features(PostgresCxxClient PUBLIC cxx_std_17)

# Dependencies.
find_package(PostgreSQL REQUIRED)

target_include_directories(PostgresCxxClient
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${PostgreSQL_INCLUDE_DIRS}
        )

target_link_libraries(PostgresCxxClient
        PUBLIC
        ${PostgreSQL_LIBRARIES}
        )

# Installation.
include(CMakePackageConfigHelpers)

write_basic_package_version_file(PostgresCxxClientConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
        )

install(TARGETS PostgresCxxClient
        EXPORT PostgresCxxClientTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

install(EXPORT PostgresCxxClientTargets
        FILE PostgresCxxClientTargets.cmake
        NAMESPACE PostgresCxxClient::
        DESTINATION lib/cmake/PostgresCxxClient
        )

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/PostgresCxxClientConfigVersion.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/PostgresCxxClientConfig.cmake"
        DESTINATION lib/cmake/PostgresCxxClient
        )

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/postgres"
        DESTINATION include
        )

# Subprojects.
option(POSTGRES_CXX_BUILD_EXAMPLES "Build examples" OFF)
option(POSTGRES_CXX_BUILD_TESTS "Build tests" OFF)

if (POSTGRES_CXX_BUILD_EXAMPLES)
    enable_testing()
    add_subdirectory(examples)
endif ()

if (POSTGRES_CXX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(deps/googletest)
    add_subdirectory(tests/unit)
endif ()
