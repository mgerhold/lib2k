#pragma once

#include "../concepts.hpp"
#include "char.hpp"
#include "literals.hpp"
#include <string>

namespace c2k {
    class Utf8String final {
        friend Utf8String Utf8Literals::operator""_utf8(char const* str, std::size_t length);

    private:
        std::string m_data;

    public:
        class ConstIterator {
            friend class Utf8String;

        private:
            std::byte const* m_next_char_start{ nullptr };
            std::uint8_t m_next_char_num_bytes{ 0 };
            // To make it impossible to assign into the dereferenced iterator, we hold the next Utf8Char instance
            // to return in a member. This allows us to return a const& and thus prohibit assignment.
            Utf8Char m_next;

            explicit ConstIterator(std::byte const* start);

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = Utf8Char;

            constexpr ConstIterator() = default;

            [[nodiscard]] Utf8Char const& operator*() const;
            [[nodiscard]] Utf8Char const* operator->() const;
            ConstIterator& operator++();
            ConstIterator operator++(int);
            [[nodiscard]] ConstIterator operator+(difference_type offset) const;
            [[nodiscard]] difference_type operator-(ConstIterator other) const;
            [[nodiscard]] bool operator==(ConstIterator const& other) const;
        };

        [[nodiscard]] static Utf8String from_string_unchecked(std::string data);

    public:
        Utf8String() = default;
        Utf8String(std::string string);
        Utf8String(char const* const string) : Utf8String{ std::string{ string } } { }
        [[nodiscard]] static tl::expected<Utf8String, Utf8Error> from_chars(std::string chars);
        [[nodiscard]] static bool is_valid_utf8(std::string_view string);

        [[nodiscard]] char const* c_str() const {
            return m_data.data();
        }

        [[nodiscard]] std::size_t calculate_char_count() const {
            return static_cast<std::size_t>(cend() - cbegin());
        }

        [[nodiscard]] std::size_t calculate_char_width() const;

        [[nodiscard]] bool operator==(Utf8String const& other) const;

        [[nodiscard]] ConstIterator begin() const {
            return ConstIterator{
                static_cast<std::byte const*>(static_cast<void const*>(m_data.data())),
            };
        }

        [[nodiscard]] ConstIterator cbegin() const {
            return begin();
        }

        [[nodiscard]] ConstIterator end() const {
            return ConstIterator{
                static_cast<std::byte const*>(static_cast<void const*>(m_data.data() + m_data.size())),
            };
        }

        [[nodiscard]] ConstIterator cend() const {
            return end();
        }

        void append(Utf8Char c);

        void append(Utf8String const& string);

        Utf8String& operator+=(Utf8Char const c) {
            append(c);
            return *this;
        }

        Utf8String& operator+=(Utf8String const& other) {
            append(other);
            return *this;
        }

        [[nodiscard]] Utf8String operator+(Utf8Char c);
        [[nodiscard]] Utf8String operator+(Utf8String const& other);
        friend Utf8String operator+(Utf8Char c, Utf8String const& string);
        friend Utf8String operator+(Utf8String const& lhs, Utf8String const& rhs);

        [[nodiscard]] bool is_empty() const {
            return m_data.empty();
        }

        void clear() {
            m_data.clear();
        }

        void reserve(std::size_t new_capacity_in_bytes);

        [[nodiscard]] ConstIterator find(Utf8Char needle) const;
        [[nodiscard]] ConstIterator find(Utf8Char needle, ConstIterator start) const;
        [[nodiscard]] ConstIterator find(Utf8Char needle, ConstIterator::difference_type start_position) const;
        [[nodiscard]] ConstIterator find(Utf8String const& needle) const;
        [[nodiscard]] ConstIterator find(Utf8String const& needle, ConstIterator start) const;
        [[nodiscard]] ConstIterator find(Utf8String const& needle, ConstIterator::difference_type start_position) const;
        ConstIterator erase(ConstIterator position);
        ConstIterator erase(ConstIterator first, ConstIterator last);
        void reverse();
        [[nodiscard]] Utf8String to_uppercase() const;
        [[nodiscard]] Utf8String to_lowercase() const;

        [[nodiscard]] Utf8String transform(Invocable<Utf8Char, Utf8Char> auto&& transformer) const {
            auto result = Utf8String{};
            for (auto const c : *this) {
                result += transformer(c);
            }
            return result;
        }

        friend std::ostream& operator<<(std::ostream& os, Utf8String const& string) {
            return os << string.m_data;
        }
    };

    /* These asserts must not appear within the Utf8String class since some compilers
     * will consider ConstIterator to be incomplete there. According to an answer on
     * StackOverflow (see https://stackoverflow.com/a/75815361/7540548), this is an open
     * defect in the standard and would be undefined behavior.
     * By moving the asserts out of the enclosing class, we can ensure that the inner
     * class is complete and the following asserts do not expose undefined behavior. */
    static_assert(std::input_or_output_iterator<Utf8String::ConstIterator>);
    static_assert(std::input_iterator<Utf8String::ConstIterator>);
    static_assert(std::forward_iterator<Utf8String::ConstIterator>);
} // namespace c2k
