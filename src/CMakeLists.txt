add_library(lib2k
        include/lib2k/types.hpp
        include/lib2k/string_utils.hpp
        string_utils.cpp
        include/lib2k/random.hpp
        random.cpp
        include/lib2k/concepts.hpp
)

target_include_directories(lib2k PUBLIC include)
