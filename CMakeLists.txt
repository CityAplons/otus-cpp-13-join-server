cmake_minimum_required(VERSION 3.10)

set(PROJECT_REVISION 1 CACHE INTERNAL "Project revision (CI)")
project(join_server
    VERSION 0.0.${PROJECT_REVISION}
    DESCRIPTION "Bulk task with boost.asio"
    HOMEPAGE_URL "https://github.com/CityAplons/otus-cpp-13-join-server"
    LANGUAGES CXX
)

set_property(GLOBAL PROPERTY G_CXX_STANDARD 20)
set_property(GLOBAL PROPERTY G_CXX_STANDARD_REQUIRED ON)

# Include utility functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(compiler)
include(test)

# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_BUILD_COMPONENTS system program_options)
find_package(Boost COMPONENTS ${Boost_BUILD_COMPONENTS})

# Transparent versioning
include_directories(cmake/version)
configure_file(cmake/version/version.h.in version.h)
configure_file(cmake/version/VERSION.in VERSION)
add_library(version cmake/version/project.cpp)
target_include_directories(version
    PRIVATE "${CMAKE_BINARY_DIR}"
)

# Describe build targets
add_executable(join_server src/server.cpp src/table.cpp)

# Mess around with boost on Windows/Ubuntu targets
if(NOT Boost_FOUND)
    set(BOOST_ROOT_DIR ${CMAKE_BINARY_DIR}/contrib/boost)
    set(BOOST_ROOT ${BOOST_ROOT_DIR})

    set(Boost_hardcoded_INCLUDE_DIRS ${BOOST_ROOT_DIR}/include)

    foreach(component ${Boost_BUILD_COMPONENTS})
        list(APPEND Boost_hardcoded_libraries ${BOOST_ROOT_DIR}/${CMAKE_INSTALL_LIBDIR}/libboost_${component}.so)
    endforeach()

    set(BOOST_REQUESTED_VERSION 1.81.0)
    message(WARNING "Boost library NOT found, download boost-${BOOST_REQUESTED_VERSION} and build from boost website.")
    include(boost)
    add_custom_target(ExternalProject_Boost
        DEPENDS Boost
    )
    add_dependencies(join_server ExternalProject_Boost)
    target_include_directories(join_server
        PUBLIC ${Boost_hardcoded_INCLUDE_DIRS}
    )
    target_link_libraries(join_server
        PUBLIC ${Boost_hardcoded_libraries})

    install(DIRECTORY ${BOOST_ROOT_DIR}/${CMAKE_INSTALL_LIBDIR}
        DESTINATION "."
        FILES_MATCHING PATTERN "*.so*")
else()
    target_link_libraries(join_server PUBLIC
        Boost::system
        Boost::program_options
    )
    install(IMPORTED_RUNTIME_ARTIFACTS ${Boost_LIBRARIES} DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

target_link_libraries(join_server PRIVATE
    version
)

# Runtime deps
set_target_properties(join_server PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)

if(UNIX AND NOT APPLE)
    set_target_properties(join_server PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
elseif(APPLE)
    set_target_properties(join_server PROPERTIES INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
endif()

set_target_properties(join_server PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS join_server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_compile_options(join_server)
set_compile_options(version)

# Describe unit tests
enable_testing()
add_unit_test(test/test_version.cpp version)

# Packaging
option(MAKE_PACKAGE "Add 'package' and 'package_source' build targets for packages" ON)

if(MAKE_PACKAGE)
    add_subdirectory(cmake/packaging)
endif()