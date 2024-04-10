#include <lib2k/utf8/string.hpp>
#include <lib2k/utf8/string_view.hpp>

namespace c2k {

    Utf8StringView::Utf8StringView(Utf8String const& string) : m_view{ string.m_data } { }

    Utf8StringView::Utf8StringView(detail::Utf8ConstIterator const& begin, detail::Utf8ConstIterator const& end)
        : m_view{ static_cast<char const*>(static_cast<void const*>(begin.m_next_char_start)),
                  static_cast<char const*>(static_cast<void const*>(end.m_next_char_start)) } { }

    Utf8StringView::Utf8StringView(std::string_view const view) {
        if (not Utf8String::is_valid_utf8(view)) {
            throw InvalidUtf8String{};
        }
        *this = from_string_view_unchecked(view);
    }

    Utf8StringView::Utf8StringView(char const* const chars) : Utf8StringView{ std::string_view{ chars } } { }

    [[nodiscard]] Utf8StringView Utf8StringView::from_string_view_unchecked(std::string_view const view) {
        auto result = Utf8StringView{};
        result.m_view = view;
        return result;
    }

} // namespace c2k
