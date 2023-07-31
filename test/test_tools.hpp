#pragma once

#include <cstdint>
#include <gtest/gtest.h>
#include <type_traits>
#include <typeinfo>

struct small_struct {
    int ivalue = 1;
    float fvalue = 2.f;
};
constexpr bool operator==(const small_struct left, const small_struct right) {
    return (left.ivalue == right.ivalue) && (left.fvalue == right.fvalue);
}

struct large_struct {
    long long lvalue = 1;
    double dvalue = 2.;
};
constexpr bool operator==(const large_struct left, const large_struct right) {
    return (left.lvalue == right.lvalue) && (left.dvalue == right.dvalue);
}

struct test_struct : ::testing::Test {
    int ivalue = 1;
    const int const_ivalue = 2;
    float fvalue = 1.f;

    small_struct ssvalue{};
    large_struct lsvalue{};
};

namespace custom_tests {

template<class Expected, class Received>
::testing::AssertionResult value_and_type_equals(const char* lexpr, const char* rexpr,
                                                 const Expected& expected,
                                                 const Received& received) {
    if constexpr (!std::is_same_v<Expected, Received>) {
        ::testing::Message msg;
        msg << "Expected equality of these types:\n";
        msg << "  " << lexpr << '\n';
        msg << "    Which is: " << typeid(Expected).name() << '\n';
        msg << "  " << rexpr << '\n';
        msg << "    Which is: " << typeid(Received).name();
        return ::testing::AssertionFailure() << msg;
    } else {
        if (expected == received) return ::testing::AssertionSuccess();

        ::testing::Message msg;
        msg << "Expected equality of these values:\n";
        msg << "  " << lexpr << '\n';
        msg << "    Which is: " << ::testing::PrintToString(expected) << '\n';
        msg << "  " << rexpr << '\n';
        msg << "    Which is: " << ::testing::PrintToString(received);
        return ::testing::AssertionFailure() << msg;
    }
}

#define EXPECT_STRICT_EQ(expected, received) \
    EXPECT_PRED_FORMAT2(::custom_tests::value_and_type_equals, expected, received)

#define EXPECT_CONST_STRICT_EQ(expr1, expr2)                     \
    {                                                            \
        constexpr decltype(auto) _bits_left_value_ = (expr1);    \
        constexpr decltype(auto) _bits_right_value_ = (expr2);   \
        EXPECT_STRICT_EQ(_bits_left_value_, _bits_right_value_); \
    }

#define EXPECT_ADDRESS_EQ(expr1, expr2) \
    EXPECT_EQ(static_cast<const void*>(expr1), static_cast<const void*>(expr2))

} // namespace custom_tests

template<class T, std::size_t N>
constexpr bool array_eq(const std::array<T, N>& left, const std::array<T, N>& right) {
    for (std::size_t i = 0; i < N; ++i) {
        if (left[i] != right[i]) return false;
    }
    return true;
}
