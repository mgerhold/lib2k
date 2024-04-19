#include <lib2k/utf8/string.hpp>
#include <lib2k/utf8/string_view.hpp>
#include <utf8proc.h>

namespace c2k {

    Utf8StringView::Utf8StringView(Utf8String const& string) : m_view{ string.m_data } { }

    Utf8StringView::Utf8StringView(detail::Utf8ConstIterator const& begin, detail::Utf8ConstIterator const& end)
        : m_view{ reinterpret_cast<char const*>(begin.m_next_char_start),
                  reinterpret_cast<char const*>(end.m_next_char_start) } { }

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

    [[nodiscard]] std::size_t Utf8StringView::calculate_char_width() const {
        auto width = std::size_t{ 0 };
        auto current = reinterpret_cast<utf8proc_uint8_t const*>(m_view.data());

        while (current < reinterpret_cast<utf8proc_uint8_t const*>(m_view.data() + m_view.length())) {
            auto codepoint = utf8proc_int32_t{};
            auto const bytes_read = utf8proc_iterate(current, -1, &codepoint);
            // clang-format off
            assert(
                codepoint != -1
                and utf8proc_codepoint_valid(codepoint)
                and "Utf8String objects must always contain valid UTF8"
            );
            // clang-format on
            width += static_cast<std::size_t>(utf8proc_charwidth(codepoint));
            current += bytes_read;
        }
        // clang-format off
        assert(
            current == reinterpret_cast<utf8proc_uint8_t const*>(m_view.data() + m_view.length())
            and "Utf8String objects must always contain valid UTF8"
        );
        // clang-format on
        return width;
    }

    [[nodiscard]] Utf8StringView Utf8StringView::substring(ConstIterator const begin, ConstIterator const end) const {
        return Utf8StringView{ begin, end };
    }

    [[nodiscard]] Utf8StringView Utf8StringView::substring(ConstIterator const begin) const {
        return substring(begin, this->cend());
    }

    // clang-format off
    [[nodiscard]] Utf8StringView Utf8StringView::substring(
        ConstIterator const begin,
        std::size_t const num_chars
    ) const { // clang-format on
        return substring(begin, begin + static_cast<ConstIterator::difference_type>(num_chars));
    }

    [[nodiscard]] Utf8StringView Utf8StringView::substring(std::size_t const start, std::size_t const num_chars) const {
        auto const begin = this->cbegin() + static_cast<ConstIterator::difference_type>(start);
        auto const end = begin + static_cast<ConstIterator::difference_type>(num_chars);
        return substring(begin, end);
    }

    [[nodiscard]] Utf8StringView Utf8StringView::substring(std::size_t const start) const {
        auto const begin = this->cbegin() + static_cast<ConstIterator::difference_type>(start);
        return substring(begin, this->cend());
    }
} // namespace c2k
