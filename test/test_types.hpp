#pragma once

#include <cstdint>
#include <type_traits>
#include <gtest/gtest.h>

using test_types = ::testing::Types<
	//int, unsigned
	std::int8_t, std::int16_t, std::int32_t, std::int64_t,
	std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
>;

template<class T>
using as_uint_t = std::make_unsigned_t<T>;

constexpr std::byte operator""_b(unsigned long long int x) {
	return static_cast<std::byte>(x);
}

template<class T>
constexpr unsigned bit_width(T value) {
	unsigned result = 0;
	while (value >>= 1) ++result;
	return result;
}
