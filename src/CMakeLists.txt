add_library(lib2k
        include/lib2k/overloaded.hpp)

target_sources(lib2k
        PRIVATE
        string_utils.cpp
        random.cpp
        file_utils.cpp
        utf8/literals.cpp
        utf8/char.cpp
        utf8/string.cpp
        utf8/string_view.cpp
        utf8/const_iterator.cpp
        utf8/const_reverse_iterator.cpp

        PUBLIC FILE_SET HEADERS
        BASE_DIRS include FILES
        include/lib2k/types.hpp
        include/lib2k/string_utils.hpp
        include/lib2k/random.hpp
        include/lib2k/concepts.hpp
        include/lib2k/file_utils.hpp
        include/lib2k/unique_value.hpp
        include/lib2k/non_null_owner.hpp
        include/lib2k/synchronized.hpp
        include/lib2k/unreachable.hpp
        include/lib2k/utf8.hpp
        include/lib2k/static_vector.hpp
        include/lib2k/utf8/string.hpp
        include/lib2k/utf8/char.hpp
        include/lib2k/utf8/errors.hpp
        include/lib2k/utf8/string_view.hpp
        include/lib2k/utf8/const_iterator.hpp
        include/lib2k/utf8/const_reverse_iterator.hpp
        include/lib2k/static_string.hpp
        include/lib2k/defer.hpp
        include/lib2k/pinned.hpp
)

target_include_directories(
        lib2k
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_system_libraries(lib2k
        PUBLIC
        tl::expected
        PRIVATE
        "$<BUILD_INTERFACE:utf8proc>"
)
