#include <gtest/gtest.h>
#include <tuple>

#define BITS_COMPILER_INTRINSICS 1
#include <bits.hpp>
#include <test_types.hpp>

#define TypeParam T

namespace {

template<class T>
struct basic : ::testing::Test {
    T value = T(1);
    const T const_value = T(2);
    static constexpr T constexpr_value = T(3);
};

struct custom_struct {
    int ivalue = 1;
    float fvalue = 2.f;
};

struct custom_basic : ::testing::Test {
    custom_struct value{};
};

using int_basic = basic<int>;

TYPED_TEST_SUITE(basic, test_types, );

TEST_F(int_basic, constructor) {
    (void)bits{value};
    (void)bits{const_value};
    static_assert(((void)bits{constexpr_value}, true));

    (void)bits{1};
    (void)bits{1.f};

    static_assert(((void)bits{1}, true));
}

TEST_F(int_basic, value) {
    EXPECT_EQ(bits{value}.value(), value);
    EXPECT_EQ(bits{const_value}.value(), const_value);
    constexpr auto compile_time_value = bits{constexpr_value}.value();
    EXPECT_EQ(compile_time_value, constexpr_value);

    EXPECT_EQ(&bits{value}.value(), &value);
}

TYPED_TEST(basic, as_uint) {
    using type = decltype(bits{this->value}.as_uint());
    static_assert(std::is_unsigned_v<type>);

    EXPECT_EQ(T(bits{this->value}.as_uint()), this->value);
    EXPECT_EQ(T(bits{this->const_value}.as_uint()), this->const_value);
    constexpr auto compile_time_uint = bits{this->constexpr_value}.as_uint();
    EXPECT_EQ(T(compile_time_uint), this->constexpr_value);
}

TYPED_TEST(basic, as) {
    using UInt = as_uint_t<T>;
    EXPECT_EQ(bits{this->value}.template as<UInt>(), UInt(this->value));
    EXPECT_EQ(bits{this->const_value}.template as<UInt>(), UInt(this->const_value));
    constexpr auto compile_time_as = bits{this->constexpr_value}.template as<UInt>();
    EXPECT_EQ(compile_time_as, UInt(this->constexpr_value));
    if constexpr (sizeof(UInt) == sizeof(float)) {
        (void)bits{this->value}.template as<float>();
    }
}

TYPED_TEST(basic, as_ref) {
    using UInt = as_uint_t<T>;
    EXPECT_EQ(bits{this->value}.template as_ref<UInt>(), UInt(this->value));
    EXPECT_EQ(bits{this->value}.template as_ref<const UInt>(), UInt(this->value));
    EXPECT_EQ(bits{this->const_value}.template as_ref<const UInt>(), UInt(this->const_value));

    UInt& value_ref = bits{this->value}.template as_ref<UInt>();
    value_ref = 10;
    EXPECT_EQ(bits{this->value}.template as<UInt>(), 10);

    const UInt& const_value_ref = bits{this->value}.template as_ref<const UInt>();
    const_cast<UInt&>(const_value_ref) = 20;
    EXPECT_EQ(bits{this->value}.template as<UInt>(), 20);
}

TYPED_TEST(basic, as_bytes) {
    const auto bytes = bits{this->value}.as_bytes();

    const std::array<std::uint8_t, sizeof(T)> cmp_to{1};
    EXPECT_EQ(bytes, cmp_to);
}

TYPED_TEST(basic, narrow_as) {
    EXPECT_EQ(bits{this->value}.narrow().template as<int>(), 1);
    EXPECT_EQ(bits{this->value}.narrow().template as<unsigned>(), 1);
}

TYPED_TEST(basic, set) {
    bits{this->value}.set();
    EXPECT_EQ(this->value, T(-1));
    constexpr bool compile_time_set = [] {
        T val = T(2);
        bits{val}.set();
        if (val != T(-1)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_set);

    auto array = weak_make_array<T, 4>(1, 2, 0, 4);
    bits{array}.set();

    constexpr auto expected_array = weak_make_array<T, 4>(-1, -1, -1, -1);
    EXPECT_EQ(array, expected_array);

    T c_array[4]{T(1), T(20), T(30), T(40)};
    bits{c_array}.set();

    EXPECT_EQ(bits{c_array}, expected_array);
}

TYPED_TEST(basic, reset) {
    bits{this->value}.reset();
    EXPECT_EQ(this->value, T(0));
    constexpr bool compile_time_reset = [] {
        T val = T(100);
        bits{val}.reset();
        if (val != T(0)) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_reset);

    auto array = weak_make_array<T, 4>(0, 100, 50, 25);
    bits{array}.reset();

    constexpr auto expected_array = weak_make_array<T, 4>(0, 0, 0, 0);
    EXPECT_EQ(array, expected_array);
}

TYPED_TEST(basic, flip) {
    using u_t = safe_underlying_type<T>;

    bits{this->value}.flip();
    EXPECT_EQ(this->value, T(u_t(-1) - 1));
    bits{this->value}.flip();
    EXPECT_EQ(this->value, T(1));

    constexpr bool compile_time_flip = [] {
        T val = T(0b1010'1010u);
        bits{val}.flip();
        if (val != T(u_t(0b0101'0101) - u_t(sizeof(T) == 1 ? 0u : 1u << 8))) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_flip);

    std::array<T, 4> array{T(2), T(3), T(5), T(7)};
    bits{array}.flip();

    using u_t = safe_underlying_type<T>;

    constexpr auto expected_array
        = weak_make_array<T, 4>(u_t(-1) - 2, u_t(-1) - 3, u_t(-1) - 5, u_t(-1) - 7);
    EXPECT_EQ(array, expected_array);

    T c_array[4]{T(15), T(30), T(56), T(96)};
    bits{c_array}.flip();

    constexpr auto expected_c_array = weak_make_array<T, 4>(T(u_t(-1) - 15), T(u_t(-1) - 30),
        T(u_t(-1) - 56), T(u_t(-1) - 96));
    EXPECT_EQ(bits{c_array}, expected_c_array);
}

TYPED_TEST(basic, flip_at) {
    bits{this->value}.flip_at(0);
    EXPECT_EQ(this->value, T(0));

    bits{this->value}.flip_at(2);
    EXPECT_EQ(this->value, T(4));
    bits{this->value}.flip_at(2);
    EXPECT_EQ(this->value, T(0));

    auto array = weak_make_array<T, 2>(0b1001, 0b0110);
    bits{array}.flip_at(0);
    EXPECT_EQ(array[0], T(0b1000));
    bits{array[1]}.flip_at(1);
    EXPECT_EQ(array[1], T(0b0100));
}

TYPED_TEST(basic, all) {
    EXPECT_FALSE(bits{this->value}.all());
    EXPECT_FALSE(bits{this->const_value}.all());

    this->value = T(-1);
    EXPECT_TRUE(bits{this->value}.all());

    constexpr bool compile_time_all = [] {
        T val = T(1);
        if (bits{val}.all()) return false;

        val = T(-1);
        if (!bits{val}.all()) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_all);
}

TYPED_TEST(basic, any) {
    EXPECT_TRUE(bits{this->value}.any());
    EXPECT_TRUE(bits{this->const_value}.any());

    this->value = T(0);
    EXPECT_FALSE(bits{this->value}.any());

    constexpr bool compile_time_any = [] {
        T val = T(0);
        if (bits{val}.any()) return false;

        val = T(1);
        if (!bits{val}.any()) return false;

        val = T(-1);
        if (!bits{val}.any()) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_any);
}

TYPED_TEST(basic, none) {
    EXPECT_FALSE(bits{this->value}.none());
    EXPECT_FALSE(bits{this->const_value}.none());

    this->value = T(0);
    EXPECT_TRUE(bits{this->value}.none());

    constexpr bool compile_time_none = [] {
        T val = T(5);
        if (bits{val}.none()) return false;

        val = T(0);
        if (!bits{val}.none()) return false;

        val = T(-1);
        if (bits{val}.none()) return false;

        return true;
    }();
    EXPECT_TRUE(compile_time_none);
}

TYPED_TEST(basic, copy_to) {
    std::array<char, sizeof(T)> result_to_copy;
    bits{this->value}.copy_to(result_to_copy);
    EXPECT_EQ(result_to_copy[0], 1);
}

TYPED_TEST(basic, as_bytes_ref) {
    auto& bytes = bits{this->value}.as_bytes_ref();
    bytes[0] = 10;
    EXPECT_EQ(this->value, T(10));
}

TEST_F(int_basic, copy) {
    struct non_copyable_t {
        int value = 123;
        non_copyable_t(const non_copyable_t&) = delete;
        non_copyable_t& operator=(const non_copyable_t&) = delete;
    } non_copyable{};

    const non_copyable_t copy_of_non_copyable = bits{non_copyable}.copy();
    EXPECT_EQ(copy_of_non_copyable.value, 123);
    EXPECT_NE(&copy_of_non_copyable.value, &non_copyable.value);
}

TEST_F(int_basic, as_refw) {
    struct struct_value_t {
        int v = -2;
    };
    struct_value_t struct_value;
    auto ref_wrapper = bits{struct_value}.as_refw<unsigned>();
    EXPECT_EQ(ref_wrapper, unsigned(-2));

    ref_wrapper = 2u;
    EXPECT_EQ(struct_value.v, 2);

    constexpr bool compile_time_as_refw = [] {
        int value = 12222;
        auto refw = bits{value}.as_refw<unsigned>();
        if (refw != 12222) return false;
        refw = 2u;
        if (value != 2) return false;
        return true;
    }();
    EXPECT_TRUE(compile_time_as_refw);
}

TEST_F(int_basic, as_array) {
    const auto arr = bits{value}.as_array<short>();
    constexpr std::array<short, 2> cmp_arr{1, 0};
    EXPECT_EQ(arr, cmp_arr);

    const auto char_arr = bits{const_value}.as_array<char>();
    constexpr std::array<char, 4> cmp_char_arr{2, 0, 0, 0};
    EXPECT_EQ(char_arr, cmp_char_arr);
}

TEST_F(custom_basic, emplace) {
    bits{value}.template emplace<int, float>(10, 2.f);
    EXPECT_EQ(value.ivalue, 10);
    EXPECT_EQ(value.fvalue, 2.f);

    bits{value}.template emplace<float, int>(5.f, 1);
    EXPECT_EQ(value.ivalue, 1084227584);
    EXPECT_EQ(value.fvalue, 1.40129846432e-45f);
}

} // namespace
