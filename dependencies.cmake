include("${PROJECT_SOURCE_DIR}/cmake/CPM.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/system_link.cmake")

function(lib2k_setup_dependencies)
    CPMAddPackage(
            NAME TL_EXPECTED
            GITHUB_REPOSITORY TartanLlama/expected
            VERSION 1.1.0
            OPTIONS
            "EXPECTED_BUILD_PACKAGE OFF"
            "EXPECTED_BUILD_TESTS OFF"
            "EXPECTED_BUILD_PACKAGE_DEB OFF"
    )

    # todo: add option to include this dependency
    CPMAddPackage(
            NAME UTF8_PROC
            GITHUB_REPOSITORY JuliaStrings/utf8proc
            VERSION 2.9.0
            OPTIONS
            "UTF8PROC_INSTALL OFF"
            "BUILD_SHARED_LIBS OFF"
    )

    if (${lib2k_build_tests})
        CPMAddPackage(
                NAME GOOGLE_TEST
                GITHUB_REPOSITORY google/googletest
                VERSION 1.14.0
                OPTIONS
                "BUILD_GMOCK OFF"
                "INSTALL_GTEST OFF"
        )
    endif ()
endfunction()
