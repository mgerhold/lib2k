cmake_minimum_required(VERSION 3.28)
project(lib2k)

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
