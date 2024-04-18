add_library(lib2k
        include/lib2k/types.hpp
        include/lib2k/string_utils.hpp
        string_utils.cpp
        include/lib2k/random.hpp
        random.cpp
        include/lib2k/concepts.hpp
        include/lib2k/file_utils.hpp
        file_utils.cpp
        include/lib2k/unique_value.hpp
        include/lib2k/non_null_owner.hpp
        include/lib2k/synchronized.hpp
        include/lib2k/unreachable.hpp
        include/lib2k/utf8.hpp
        include/lib2k/static_vector.hpp
        include/lib2k/utf8/string.hpp
        include/lib2k/utf8/char.hpp
        utf8/literals.cpp
        utf8/char.cpp
        utf8/string.cpp
        include/lib2k/utf8/errors.hpp
        include/lib2k/utf8/string_view.hpp
        utf8/string_view.cpp
        include/lib2k/utf8/const_iterator.hpp
        utf8/const_iterator.cpp
        include/lib2k/utf8/const_reverse_iterator.hpp
        utf8/const_reverse_iterator.cpp
)

target_include_directories(lib2k PUBLIC include)
target_link_system_libraries(lib2k PUBLIC tl::expected utf8proc)
