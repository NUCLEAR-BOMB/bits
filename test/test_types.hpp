#pragma once

#include <cstdint>
#include <gtest/gtest.h>
#include <type_traits>

enum weak_enum_t {
    weak_elem1 = 1,
    weak_elem2 = 10,
    weak_elem3 = 1000,
};
enum class strong_enum_t {
    a1,
    b2,
    c3,
    d4 = -1
};

using test_types = ::testing::Types<
    //int, unsigned
    std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::uint8_t, std::uint16_t,
    std::uint32_t, std::uint64_t, weak_enum_t, strong_enum_t>;

template<class T>
using as_uint_t = std::make_unsigned_t<T>;

template<class To, class From>
constexpr auto uint_as(const From from) {
    return static_cast<as_uint_t<To>>(from);
}

template<class T>
constexpr unsigned bit_width(T value) {
    unsigned result = 0;
    while (value >>= 1)
        ++result;
    return result;
}

template<class T, bool = std::is_enum_v<T>>
struct safe_underlying_type_impl {
    using type = std::underlying_type_t<T>;
};
template<class T>
struct safe_underlying_type_impl<T, false> {
    using type = T;
};

template<class T>
using safe_underlying_type = typename safe_underlying_type_impl<T>::type;

template<class T, std::size_t N, class... Args>
constexpr std::array<T, N> weak_make_array(const Args&... args) {
    return {{static_cast<T>(args)...}};
}
