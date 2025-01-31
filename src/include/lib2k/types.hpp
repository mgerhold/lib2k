#pragma once

#include <cstdint>
#include <cstdlib>

// The following type aliases are deliberately not contained in a namespace
// to make their usage more convenient.

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using usize = std::size_t;

// TODO: Replace with std::float32_t and std::float64_t as soon as
//       all compilers support them.
using f32 = float;
static_assert(sizeof(f32) == 4, "f32 is not 4 bytes");
using f64 = double;
static_assert(sizeof(f64) == 8, "f64 is not 8 bytes");
