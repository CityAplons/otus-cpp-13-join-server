cmake_minimum_required(VERSION 3.10)

set(PROJECT_REVISION 1 CACHE INTERNAL "Project revision (CI)")
project(cmake_template
    VERSION 0.0.${PROJECT_REVISION}
    DESCRIPTION "General CMake template for C++ projects by N.Mikhailovskiy"
    HOMEPAGE_URL "https://github.com/CityAplons/cmake-multiplatform-template"
    LANGUAGES CXX
)

set_property(GLOBAL PROPERTY G_CXX_STANDARD 14)
set_property(GLOBAL PROPERTY G_CXX_STANDARD_REQUIRED ON)

# Include utility functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(compiler)
include(test)

# Transparent versioning
include_directories(cmake/version)
configure_file(cmake/version/version.h.in version.h)
configure_file(cmake/version/VERSION.in VERSION)
add_library(version cmake/version/project.cpp)
target_include_directories(version
    PRIVATE "${CMAKE_BINARY_DIR}"
)

# Describe build targets
add_executable(helloworld_cli src/hello.cpp)
target_link_libraries(helloworld_cli PRIVATE
    version
)

set_compile_options(helloworld_cli)
set_compile_options(version)

install(TARGETS helloworld_cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Describe unit tests
enable_testing()
add_unit_test(test/test_version.cpp version)

# Packaging
option(MAKE_PACKAGE "Add 'package' and 'package_source' build targets for packages" ON)
if (MAKE_PACKAGE)
	add_subdirectory(cmake/packaging)
endif()