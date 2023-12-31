#include <cmath>
#include <gtest/gtest.h>
#include <tuple>

//#define BITS_COMPILER_INTRINSICS 1
#include <bits.hpp>
#include <test_tools.hpp>

namespace {

using basic = test_base;

TEST_F(basic, constructor) {
    (void)bits{ivalue};
    (void)bits{const_ivalue};

    (void)bits{1};
    (void)bits{1.f};

    (void)bits{lsvalue};
    (void)bits{ssvalue};
}

TEST_F(basic, value) {
    EXPECT_STRICT_EQ(bits{ivalue}.value(), ivalue);
    EXPECT_STRICT_EQ(bits{const_ivalue}.value(), const_ivalue);
    EXPECT_STRICT_EQ(bits{1}.value(), 1);

    EXPECT_EQ(&bits{ivalue}.value(), &ivalue);
}

TEST_F(basic, as_uint) {
    EXPECT_STRICT_EQ(bits{1U}.as_uint(), 1U);
    EXPECT_STRICT_EQ(bits{1}.as_uint(), 1U);
    EXPECT_STRICT_EQ(bits{2LL}.as_uint(), 2ULL);

    EXPECT_STRICT_EQ(bits{1.f}.as_uint(), 0x3f800000U);
    EXPECT_STRICT_EQ(bits{ssvalue}.as_uint(), 1ULL | 0x40000000ULL << 32);
}

TEST_F(basic, as_int) {
    EXPECT_STRICT_EQ(bits{1}.as_int(), 1);
    EXPECT_STRICT_EQ(bits{1U}.as_int(), 1);
    EXPECT_STRICT_EQ(bits{1ULL}.as_int(), 1LL);

    EXPECT_STRICT_EQ(bits{2.f}.as_int(), 0x40000000);
    EXPECT_STRICT_EQ(bits{ssvalue}.as_int(), 1LL | 0x40000000LL << 32);
}

TEST_F(basic, as) {
    EXPECT_STRICT_EQ(bits{ivalue}.as<unsigned>(), 1U);
    EXPECT_STRICT_EQ(bits{1}.as<unsigned>(), 1U);
    EXPECT_CONST_STRICT_EQ(bits{10}.as<unsigned>(), 10U);

    EXPECT_STRICT_EQ(bits{0x40000000}.as<float>(), 2.f);
    EXPECT_STRICT_EQ(bits{1.f}.as<unsigned>(), 0x3f800000U);
    EXPECT_STRICT_EQ(bits{ssvalue}.as<long long>(), 1LL | 0x40000000LL << 32);

    constexpr bool compile_time_as = [] {
        int val = -1;
        auto uval = bits{val}.as<unsigned>();
        if (uval != unsigned(-1)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_as);
}

TEST_F(basic, as_ref) {
    EXPECT_STRICT_EQ(bits{1}.as_ref<unsigned>(), 1U);
    EXPECT_STRICT_EQ(bits{1}.as_ref<const unsigned>(), 1U);

    EXPECT_STRICT_EQ(bits{ivalue}.as_ref<unsigned>(), 1U);
    EXPECT_ADDRESS_EQ(&bits{ivalue}.as_ref<float>(), &ivalue);

    float& flt_ref = bits{ivalue}.as_ref<float>();
    flt_ref = 1.f;
    EXPECT_STRICT_EQ(bits{ivalue}.as<float>(), 1.f);

    std::uint64_t& ull_ref = bits{ssvalue}.as_ref<std::uint64_t>();
    ull_ref = 0;
    EXPECT_EQ(ssvalue, (small_struct{0, 0.f}));

    constexpr bool compile_time_as_ref = [] {
        int val = 1;
        if (bits{val}.as_ref<int>() != 1) return false;
        if (bits{val}.as_ref<const int>() != 1) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_as_ref);
}

TEST_F(basic, as_bytes) {
    using byte_array4 = std::array<std::uint8_t, 4>;
    using byte_array8 = std::array<std::uint8_t, 8>;
    using byte_array16 = std::array<std::uint8_t, 16>;

    EXPECT_EQ(bits{1}.as_bytes(), (byte_array4{1, 0, 0, 0}));
    EXPECT_EQ(bits{0x00'00'01'FF}.as_bytes(), (byte_array4{255, 1, 0, 0}));
    EXPECT_EQ(bits{1.f}.as_bytes(), (byte_array4{0, 0, 128, 63}));

    EXPECT_EQ(bits{ssvalue}.as_bytes(), (byte_array8{1, 0, 0, 0, 0, 0, 0, 64}));
    EXPECT_EQ(bits{lsvalue}.as_bytes(),
        (byte_array16{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64}));

    constexpr bool compile_time_as_bytes = [] {
        const int val = 0x03'02'01'00;
        const auto valb = bits{val}.as_bytes();
        if (!array_eq(valb, {0, 1, 2, 3})) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_as_bytes);
}

TEST_F(basic, narrow_as) {
    EXPECT_STRICT_EQ(bits{1}.narrow().as<unsigned>(), 1U);
    EXPECT_STRICT_EQ(bits{short(5)}.narrow().as<int>(), 5);
    EXPECT_STRICT_EQ(bits{10LL}.narrow().as<short>(), short(10));
    EXPECT_STRICT_EQ(bits{1.f}.narrow().as<char>(), char(0));
    EXPECT_STRICT_EQ(bits{ssvalue}.narrow().as<int>(), 1);
    EXPECT_STRICT_EQ(bits{lsvalue}.narrow().as<long long>(), 1LL);

    EXPECT_CONST_STRICT_EQ(bits{1}.narrow().as<short>(), short(1));
    EXPECT_CONST_STRICT_EQ(bits{1}.narrow().as<long long>(), 1LL);
    EXPECT_CONST_STRICT_EQ(bits{1}.narrow().as<unsigned>(), 1U);
}

TEST_F(basic, set) {
    bits{ivalue}.set();
    EXPECT_EQ(ivalue, -1);

    bits{fvalue}.set();
    EXPECT_TRUE(std::isnan(fvalue));
    EXPECT_EQ(bits{fvalue}, 0xFFFFFFFFU);

    bits{ssvalue}.set();
    EXPECT_EQ(ssvalue.ivalue, -1);
    EXPECT_TRUE(std::isnan(ssvalue.fvalue));

    bits{lsvalue}.set();
    EXPECT_EQ(lsvalue.lvalue, -1LL);
    EXPECT_TRUE(std::isnan(lsvalue.dvalue));
    EXPECT_EQ(bits{lsvalue.dvalue}, 0xFFFFFFFFFFFFFFFFULL);

    constexpr bool compile_time_set = [] {
        unsigned val = 2;
        bits{val}.set();
        if (val != unsigned(-1)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_set);

    std::array arr{1, 2, 0, 4};
    bits{arr}.set();
    EXPECT_STRICT_EQ(arr, (std::array{-1, -1, -1, -1}));
}

TEST_F(basic, reset) {
    bits{ivalue}.reset();
    EXPECT_EQ(ivalue, 0);

    bits{fvalue}.reset();
    EXPECT_EQ(fvalue, 0.f);

    bits{ssvalue}.reset();
    EXPECT_EQ(ssvalue.ivalue, 0);
    EXPECT_EQ(ssvalue.fvalue, 0.f);

    bits{lsvalue}.reset();
    EXPECT_EQ(lsvalue.lvalue, 0LL);
    EXPECT_EQ(lsvalue.dvalue, 0.);

    constexpr bool compile_time_reset = [] {
        unsigned val = 100;
        bits{val}.reset();
        if (val != 0) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_reset);

    std::array arr{0, 100, 50, 25};
    bits{arr}.reset();
    EXPECT_EQ(arr, (std::array{0, 0, 0, 0}));
}

TEST_F(basic, flip) {
    bits{ivalue}.flip();
    EXPECT_EQ(ivalue, ~1);
    bits{ivalue}.flip();
    EXPECT_EQ(ivalue, 1);

    bits{fvalue}.flip();
    EXPECT_EQ(fvalue, -3.99999976158f);

    bits{ssvalue}.flip();
    EXPECT_EQ(ssvalue.ivalue, -2);
    EXPECT_EQ(ssvalue.fvalue, -1.99999988079f);

    bits{lsvalue}.flip();
    EXPECT_EQ(lsvalue.lvalue, -2LL);
    EXPECT_EQ(lsvalue.dvalue, -1.9999999999999997779553950749686919152736663818359375);

    constexpr bool compile_time_flip = [] {
        int val = 0b1010'1010;
        bits{val}.flip();
        if (val != int(0b1111'1111'1111'1111'1111'1111'0101'0101)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_flip);

    std::array arr{2, 3, 5, 7};
    bits{arr}.flip();
    EXPECT_EQ(arr, (std::array{~2, ~3, ~5, ~7}));
}

TEST_F(basic, flip_at) {
    bits{ivalue}.flip_at(0);
    EXPECT_EQ(ivalue, 0);

    bits{ivalue}.flip_at(2);
    EXPECT_EQ(ivalue, 4);
    bits{ivalue}.flip_at(2);
    EXPECT_EQ(ivalue, 0);

    bits{fvalue}.flip_at(31);
    EXPECT_EQ(fvalue, -1.f);
    bits{fvalue}.flip_at(30);
    EXPECT_TRUE(std::isinf(fvalue));

    bits{ssvalue}.flip_at(0);
    EXPECT_EQ(ssvalue.ivalue, 0);
    bits{ssvalue}.flip_at(63);
    EXPECT_EQ(ssvalue.fvalue, -2.f);

    bits{lsvalue}.flip_at(1);
    EXPECT_EQ(lsvalue.lvalue, 3);
    bits{lsvalue}.flip_at(63 + 53);
    EXPECT_EQ(lsvalue.dvalue, 4.);

    std::array arr{0b1001, 0b0110};
    bits{arr}.flip_at(0);
    EXPECT_EQ(arr[0], 0b1000);
    bits{arr[1]}.flip_at(1);
    EXPECT_EQ(arr[1], 0b0100);
}

TEST_F(basic, all) {
    EXPECT_FALSE(bits{ivalue}.all());
    ivalue = -1;
    EXPECT_TRUE(bits{ivalue}.all());

    EXPECT_FALSE(bits{fvalue}.all());
    bits{fvalue} = 0xFFFFFFFF;
    EXPECT_TRUE(bits{fvalue}.all());

    EXPECT_FALSE(bits{ssvalue}.all());
    bits{ssvalue} = 0xFFFFFFFFFFFFFFFFULL;
    EXPECT_TRUE(bits{ssvalue}.all());

    EXPECT_FALSE(bits{lsvalue}.all());
    bits{lsvalue.lvalue} = 0xFFFFFFFFFFFFFFFFULL;
    bits{lsvalue.dvalue} = 0xFFFFFFFFFFFFFFFFULL;
    EXPECT_TRUE(bits{lsvalue}.all());

    constexpr bool compile_time_all = [] {
        int val = 1;
        if (bits{val}.all()) return false;
        val = -1;
        if (!bits{val}.all()) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_all);

    std::array<int, 4> arr{1, 2, 3, 4};
    EXPECT_FALSE(bits{arr}.all());
    arr = {-1, -1, -1, -1};
    EXPECT_TRUE(bits{arr}.all());
}

TEST_F(basic, any) {
    EXPECT_TRUE(bits{ivalue}.any());
    ivalue = 0;
    EXPECT_FALSE(bits{ivalue}.any());

    EXPECT_TRUE(bits{fvalue}.any());
    fvalue = 0.f;
    EXPECT_FALSE(bits{fvalue}.any());

    EXPECT_TRUE(bits{ssvalue}.any());
    bits{ssvalue} = 0;
    EXPECT_FALSE(bits{ssvalue}.any());

    EXPECT_TRUE(bits{lsvalue}.any());
    bits{lsvalue.lvalue} = 0;
    bits{lsvalue.dvalue} = 0;
    EXPECT_FALSE(bits{lsvalue}.any());

    constexpr bool compile_time_any = [] {
        int val = 0;
        if (bits{val}.any()) return false;
        val = 1;
        if (!bits{val}.any()) return false;
        val = -1;
        if (!bits{val}.any()) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_any);

    std::array<long long, 2> arr{123, 456};
    EXPECT_TRUE(bits{arr}.any());
    arr = {0, 0};
    EXPECT_FALSE(bits{arr}.any());
}

TEST_F(basic, none) {
    EXPECT_FALSE(bits{ivalue}.none());
    ivalue = 0;
    EXPECT_TRUE(bits{ivalue}.none());

    EXPECT_FALSE(bits{fvalue}.none());
    fvalue = 0.f;
    EXPECT_TRUE(bits{fvalue}.none());

    EXPECT_FALSE(bits{ssvalue}.none());
    ssvalue.ivalue = 0;
    ssvalue.fvalue = 0.f;
    EXPECT_TRUE(bits{ssvalue}.none());

    EXPECT_FALSE(bits{lsvalue}.none());
    lsvalue.lvalue = 0;
    lsvalue.dvalue = 0.;
    EXPECT_TRUE(bits{lsvalue}.none());

    constexpr bool compile_time_none = [] {
        int val = 5;
        if (bits{val}.none()) return false;
        val = 0;
        if (!bits{val}.none()) return false;
        val = -1;
        if (bits{val}.none()) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_none);

    std::array<unsigned, 5> arr{0, 2, 3, 4, 0};
    EXPECT_FALSE(bits{arr}.none());
    arr = {0, 0, 0, 0, 0};
    EXPECT_TRUE(bits{arr}.none());
}

TEST_F(basic, copy_to) {
    std::array<char, 4> result_to_copy{};
    bits{ivalue}.copy_to(result_to_copy);
    EXPECT_EQ(result_to_copy, (std::array<char, 4>{1, 0, 0, 0}));

    bits{fvalue}.copy_to(result_to_copy);
    EXPECT_EQ(result_to_copy, (std::array<char, 4>{0, 0, -128, 63}));

    constexpr bool compile_time_copy_to = [] {
        std::array<std::uint8_t, 4> result_arr{};
        unsigned val = 0xFF'00'0F'F0;

        bits{val}.copy_to(result_arr);
        if (!array_eq(result_arr, {240, 15, 0, 255})) return false;

        val = 0;
        bits{result_arr}.copy_to(val);
        if (val != 4278194160U) return false;

        return true;
    }();
    EXPECT_TRUE(compile_time_copy_to);
}

TEST_F(basic, as_bytes_ref) {
    auto& bytes = bits{ivalue}.as_bytes_ref();
    bytes[0] = 10;
    EXPECT_EQ(ivalue, 10);
    bytes[1] = 1;
    EXPECT_EQ(ivalue, 266);
}

TEST_F(basic, copy) {
    struct non_copyable_t {
        int value = 123;
        non_copyable_t(const non_copyable_t&) = delete;
        non_copyable_t& operator=(const non_copyable_t&) = delete;
    } non_copyable{};

    const auto copy_of_non_copyable = bits{non_copyable}.copy();
    EXPECT_EQ(copy_of_non_copyable->value, 123);
    EXPECT_NE(&copy_of_non_copyable->value, &non_copyable.value);

    EXPECT_STRICT_EQ(bits{1}.copy(), 1);
    EXPECT_STRICT_EQ(bits{1.f}.copy(), 1.f);
    EXPECT_STRICT_EQ(bits{ssvalue}.copy(), ssvalue);
    EXPECT_STRICT_EQ(bits{lsvalue}.copy(), lsvalue);

    constexpr bool compile_time_copy = [] {
        int val = 2;
        const int copy_val = bits{val}.copy();
        if (val != copy_val) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_copy);
}

TEST_F(basic, as_refw) {
    struct struct_value_t {
        int v = -2;
    };
    struct_value_t struct_value{};
    auto ref_wrapper = bits{struct_value}.as_refw<unsigned>();
    EXPECT_STRICT_EQ(ref_wrapper.get(), unsigned(-2));
    EXPECT_EQ(ref_wrapper, unsigned(-2));

    ref_wrapper = 2U;
    EXPECT_EQ(struct_value.v, 2);

    auto float_ref_wrapper = bits{fvalue}.as_refw<unsigned>();
    EXPECT_STRICT_EQ(float_ref_wrapper.get(), 0x3f800000U);
    float_ref_wrapper = 0x40400000U;
    EXPECT_EQ(fvalue, 3.f);

    bits{ssvalue}.as_refw<std::array<short, 4>>() = {1, 2, 3, 4};
    EXPECT_EQ(ssvalue.ivalue, 131073);
    EXPECT_EQ(ssvalue.fvalue, 3.67346188527e-40f);

    constexpr bool compile_time_as_refw = [] {
        int val = 12222;
        auto refw = bits{val}.as_refw<unsigned>();
        if (refw != 12222U) return false;
        refw = 2U;
        if (val != 2) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_as_refw);
}

TEST_F(basic, as_array) {
    const auto arr = bits{ivalue}.as_array<std::int16_t>();
    EXPECT_EQ(arr, (std::array<std::int16_t, 2>{1, 0}));

    const auto char_arr = bits{const_ivalue}.as_array<char>();
    EXPECT_EQ(char_arr, (std::array<char, 4>{2, 0, 0, 0}));

    const auto int_arr = bits{fvalue}.as_array<int>();
    EXPECT_EQ(int_arr, (std::array<int, 1>{0x3f800000}));

    const auto int_arr2 = bits{ssvalue}.as_array<int>();
    EXPECT_EQ(int_arr2, (std::array<int, 2>{1, 0x40000000}));

    const auto longlong_arr = bits{lsvalue}.as_array<long long>();
    EXPECT_EQ(longlong_arr, (std::array<long long, 2>{1, 0x4000000000000000}));

    constexpr bool compile_time_as_array = [] {
        int val = 1234 << 16 | 5678;
        auto valw = bits{val}.as_array<short>();
        if (!array_eq(valw, {5678, 1234})) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_as_array);
}

TEST_F(basic, emplace) {
    bits{ssvalue}.emplace<int, float>(10, 2.f);
    EXPECT_EQ(ssvalue.ivalue, 10);
    EXPECT_EQ(ssvalue.fvalue, 2.f);

    bits{ssvalue}.emplace<float, int>(5.f, 1);
    EXPECT_EQ(ssvalue.ivalue, 1084227584);
    EXPECT_EQ(ssvalue.fvalue, 1.40129846432e-45f);

    bits{lsvalue}.emplace(1234567890LL, 1e+200);
    EXPECT_EQ(lsvalue.lvalue, 1234567890LL);
    EXPECT_EQ(lsvalue.dvalue, 1e+200);

    bits{ivalue}.emplace(2);
    EXPECT_EQ(ivalue, 2);
    bits{fvalue}.emplace(10.f);
    EXPECT_EQ(fvalue, 10.f);
}

TEST_F(basic, count) {
    EXPECT_EQ(bits{ivalue}.count(), 1);
    EXPECT_EQ(bits{0b111}.count(), 3);
    EXPECT_EQ(bits{0b1010'1010}.count(), 4);

    const std::array<int, 4> arr{1, 2, 3, 4};
    EXPECT_EQ(bits{arr}.count(), 5);

    EXPECT_EQ(bits{fvalue}.count(), 7);

    EXPECT_EQ(bits{ssvalue}.count(), 2);
    EXPECT_EQ(bits{lsvalue}.count(), 2);
}

} // namespace
