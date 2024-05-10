cmake_minimum_required(VERSION 3.28)
project(lib2k CXX)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${PROJECT_SOURCE_DIR}/project_options.cmake)
include(${PROJECT_SOURCE_DIR}/dependencies.cmake)

lib2k_setup_dependencies()

add_subdirectory(src bin)

if (${lib2k_build_tests})
    enable_testing()
    add_subdirectory(test)
endif ()

install(TARGETS lib2k EXPORT lib2k-targets ARCHIVE
        FILE_SET HEADERS
)
install(EXPORT lib2k-targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/lib2k/cmake
        NAMESPACE c2k::
)
install(FILES "lib2k-config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/lib2k/cmake
)
