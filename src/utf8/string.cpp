#include "lib2k/utf8/string_view.hpp"
#include <lib2k/utf8/char.hpp>
#include <lib2k/utf8/string.hpp>
#include <utf8proc.h>

namespace c2k {
    [[nodiscard]] Utf8String operator+(Utf8Char const c, Utf8String const& string) {
        auto result = Utf8String{};
        result += c;
        result += string;
        return result;
    }

    [[nodiscard]] Utf8String operator+(Utf8String const& lhs, Utf8String const& rhs) {
        auto result = lhs;
        result += rhs;
        return result;
    }

    Utf8String::Utf8String(ConstIterator const& begin, ConstIterator const& end)
        : m_data{
              reinterpret_cast<char const*>(begin.m_next_char_start),
              reinterpret_cast<char const*>(end.m_next_char_start),
          } { }

    [[nodiscard]] Utf8String Utf8String::from_string_unchecked(std::string data) {
        auto result = Utf8String{};
        result.m_data = std::move(data);
        return result;
    }

    Utf8String::Utf8String(std::string string) {
        if (not is_valid_utf8(string)) {
            throw InvalidUtf8String{};
        }
        m_data = std::move(string);
    }

    [[nodiscard]] tl::expected<Utf8String, Utf8Error> Utf8String::from_chars(std::string chars) {
        if (not is_valid_utf8(chars)) {
            return tl::unexpected{ Utf8Error::InvalidUtf8String };
        }
        return from_string_unchecked(std::move(chars));
    }

    [[nodiscard]] bool Utf8String::is_valid_utf8(std::string_view const string) {
        auto current = string.data();
        auto const end = current + string.length();
        while (current < end) {
            auto const bytes_remaining = static_cast<utf8proc_ssize_t>(end - current);
            auto codepoint = utf8proc_int32_t{};
            auto const result =
                    utf8proc_iterate(reinterpret_cast<utf8proc_uint8_t const*>(current), bytes_remaining, &codepoint);
            if (result < 0) {
                return false;
            }
            current += result;
        }
        return true;
    }

    [[nodiscard]] std::size_t Utf8String::calculate_char_width() const {
        return Utf8StringView{ *this }.calculate_char_width();
    }

    [[nodiscard]] Utf8String Utf8String::substring(ConstIterator const& begin, ConstIterator const& end) const {
        return Utf8String{ begin, end };
    }

    [[nodiscard]] Utf8String Utf8String::substring(ConstIterator const& begin) const {
        return substring(begin, this->cend());
    }

    [[nodiscard]] Utf8String Utf8String::substring(ConstIterator const& begin, std::size_t const num_chars) const {
        return substring(begin, begin + static_cast<ConstIterator::difference_type>(num_chars));
    }

    [[nodiscard]] Utf8String Utf8String::substring(std::size_t const start, std::size_t const num_chars) const {
        auto const begin = this->cbegin() + static_cast<ConstIterator::difference_type>(start);
        auto const end = begin + static_cast<ConstIterator::difference_type>(num_chars);
        return substring(begin, end);
    }

    [[nodiscard]] Utf8String Utf8String::substring(std::size_t const start) const {
        auto const begin = this->cbegin() + static_cast<ConstIterator::difference_type>(start);
        return substring(begin, this->cend());
    }

    [[nodiscard]] bool Utf8String::operator==(Utf8String const& other) const {
        return m_data == other.m_data;
    }

    [[nodiscard]] Utf8Char Utf8String::front() const {
        if (is_empty()) {
            throw std::out_of_range{ "cannot call front() on empty string" };
        }
        return *cbegin();
    }

    [[nodiscard]] Utf8Char Utf8String::back() const {
        if (is_empty()) {
            throw std::out_of_range{ "cannot call back() on empty string" };
        }
        return *(cend() - 1);
    }

    void Utf8String::append(Utf8Char const c) {
        for (auto const byte : c.m_codepoint) {
            m_data.push_back(static_cast<char>(byte));
        }
    }

    void Utf8String::append(Utf8String const& string) {
        std::copy(string.m_data.cbegin(), string.m_data.cend(), std::back_inserter(m_data));
    }

    [[nodiscard]] Utf8String Utf8String::operator+(Utf8Char const c) const {
        auto copy = *this;
        copy += c;
        return copy;
    }

    [[nodiscard]] Utf8String Utf8String::operator+(Utf8String const& other) const {
        auto copy = *this;
        copy += other;
        return copy;
    }

    void Utf8String::reserve(std::size_t const new_capacity_in_bytes) {
        m_data.reserve(new_capacity_in_bytes);
    }

    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(Utf8Char const needle) const {
        return find(needle, cbegin());
    }


    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(Utf8Char const needle, ConstIterator const& start) const {
        auto const start_offset = reinterpret_cast<char const*>(start.m_next_char_start) - m_data.data();
        auto const substring_data = std::string_view{ m_data }.substr(start_offset);
        assert(not needle.m_codepoint.empty());
        auto const needle_substring = std::string_view{
            reinterpret_cast<char const*>(needle.m_codepoint.data()),
            reinterpret_cast<char const*>(needle.m_codepoint.data() + needle.m_codepoint.size()),
        };
        auto const position = substring_data.find(needle_substring);
        if (position == std::string_view::npos) {
            return cend();
        }
        return ConstIterator{
            reinterpret_cast<std::byte const*>(
                    &*(substring_data.begin() + static_cast<std::string_view::difference_type>(position))
            ),
        };
    }

    // clang-format off
    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(
        Utf8Char const needle,
        ConstIterator::difference_type const start_position
    ) const { // clang-format on
        return find(needle, cbegin() + start_position);
    }

    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(Utf8String const& needle) const {
        return find(needle, cbegin());
    }

    // clang-format off
    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(
        Utf8String const& needle,
        ConstIterator const& start
    ) const { // clang-format on
        auto const start_offset = reinterpret_cast<char const*>(start.m_next_char_start) - m_data.data();
        auto const substring_data = std::string_view{ m_data }.substr(start_offset);
        auto const needle_string = std::string_view{ needle.m_data };
        auto const position = substring_data.find(needle_string);
        if (position == std::string_view::npos) {
            return cend();
        }
        return ConstIterator{
            reinterpret_cast<std::byte const*>(
                    &*(substring_data.begin() + static_cast<std::string_view::difference_type>(position))
            ),
        };
    }

    // clang-format off
    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(
        Utf8String const& needle,
        ConstIterator::difference_type const start_position
    ) const { // clang-format on
        return find(needle, cbegin() + start_position);
    }

    Utf8String::ConstIterator Utf8String::erase(ConstIterator const& position) {
        return erase(position, position + 1);
    }

    Utf8String::ConstIterator Utf8String::erase(ConstIterator const& first, ConstIterator const& last) {
        auto const start_byte_offset =
                static_cast<std::size_t>(reinterpret_cast<char const*>(first.m_next_char_start) - m_data.data());
        auto const end_byte_offset =
                static_cast<std::size_t>(reinterpret_cast<char const*>(last.m_next_char_start) - m_data.data());

        m_data.erase(start_byte_offset, end_byte_offset - start_byte_offset);

        if (last == cend()) {
            return cend();
        }

        return ConstIterator{ first.m_next_char_start };
    }

    void Utf8String::reverse() {
        auto new_string = Utf8String{};
        new_string.reserve(m_data.size());
        for (auto it = crbegin(); it != crend(); ++it) {
            new_string += *it;
        }
        *this = std::move(new_string);
    }

    [[nodiscard]] Utf8String Utf8String::to_uppercase() const {
        return transform([](Utf8Char const c) { return c.to_uppercase(); });
    }

    [[nodiscard]] Utf8String Utf8String::to_lowercase() const {
        return transform([](Utf8Char const c) { return c.to_lowercase(); });
    }
} // namespace c2k
