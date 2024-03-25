include(GoogleTest)

add_executable(types_tests
        types_tests.cpp
)

target_link_libraries(types_tests PRIVATE lib2k lib2k_options)
target_link_system_libraries(types_tests PRIVATE GTest::gtest GTest::gtest_main)

add_executable(string_utils_tests
        string_utils_tests.cpp
)

target_link_libraries(string_utils_tests PRIVATE lib2k lib2k_options)
target_link_system_libraries(string_utils_tests PRIVATE GTest::gtest GTest::gtest_main)

add_executable(random_tests
        random_tests.cpp
)

target_link_libraries(random_tests PRIVATE lib2k lib2k_options)
target_link_system_libraries(random_tests PRIVATE GTest::gtest GTest::gtest_main)

add_executable(file_utils_tests
        file_utils_tests.cpp
)

target_link_libraries(file_utils_tests PRIVATE lib2k lib2k_options)
target_link_system_libraries(file_utils_tests PRIVATE GTest::gtest GTest::gtest_main)

gtest_discover_tests(types_tests)
gtest_discover_tests(string_utils_tests)
gtest_discover_tests(random_tests)
gtest_discover_tests(file_utils_tests)
