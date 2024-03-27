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
)

target_include_directories(lib2k PUBLIC include)
target_link_system_libraries(lib2k PUBLIC tl::expected)
