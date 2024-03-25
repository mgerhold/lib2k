#include <concepts>
#include <lib2k/types.hpp>

static_assert(std::same_as<u8, std::uint8_t>);
static_assert(std::same_as<u16, std::uint16_t>);
static_assert(std::same_as<u32, std::uint32_t>);
static_assert(std::same_as<u64, std::uint64_t>);
static_assert(std::same_as<i8, std::int8_t>);
static_assert(std::same_as<i16, std::int16_t>);
static_assert(std::same_as<i32, std::int32_t>);
static_assert(std::same_as<i64, std::int64_t>);
static_assert(std::same_as<usize, std::size_t>);
