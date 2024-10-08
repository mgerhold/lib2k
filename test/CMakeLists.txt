include(GoogleTest)

function(add_test_executable target)
    # the target name must match the filename of the source file
    add_executable(${target}
            ${target}.cpp
    )

    target_link_libraries(${target} PRIVATE lib2k lib2k_options)
    target_link_system_libraries(${target} PRIVATE GTest::gtest GTest::gtest_main)
    gtest_discover_tests(${target})
endfunction()

add_test_executable(types_tests)
add_test_executable(string_utils_tests)
add_test_executable(random_tests)
add_test_executable(file_utils_tests)
add_test_executable(synchronized_tests)
add_test_executable(non_null_owner_tests)
add_test_executable(unique_value_tests)
add_test_executable(static_vector_tests)
add_test_executable(static_string_tests)
add_test_executable(defer_tests)
add_test_executable(pinned_tests)

add_executable(utf8_tests
        utf8/utf8char_tests.cpp
        utf8/utf8string_tests.cpp
        utf8/utf8string_view_tests.cpp
        utf8/utf8iterator_tests.cpp
)

target_link_libraries(utf8_tests PRIVATE lib2k lib2k_options)
target_link_system_libraries(utf8_tests PRIVATE GTest::gtest GTest::gtest_main)
gtest_discover_tests(utf8_tests)
