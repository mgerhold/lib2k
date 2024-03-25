#pragma once

#include <concepts>

template<typename T, typename... Args>
concept IsOneOf = (std::same_as<T, Args> || ...);
