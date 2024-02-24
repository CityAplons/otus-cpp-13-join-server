# #
# Function to create a test for the project.
# Engine: GTest: https://cmake.org/cmake/help/latest/module/GoogleTest.html
# #

find_package(GTest REQUIRED)
include(compiler)
include(GoogleTest)

function(add_unit_test source_file target_links_list)
    get_filename_component(UT_TARGET_NAME ${source_file} NAME_WLE)
    add_executable(${UT_TARGET_NAME} ${source_file})

    set_compile_options(${UT_TARGET_NAME})
    target_link_libraries(${UT_TARGET_NAME}
        GTest::gtest GTest::gtest_main
        ${target_links_list}
    )

    gtest_discover_tests(${UT_TARGET_NAME})
    message(STATUS "Unit-test '${UT_TARGET_NAME}' is assigned for ${source_file}")
endfunction()