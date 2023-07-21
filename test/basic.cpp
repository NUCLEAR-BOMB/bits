#include <gtest/gtest.h>
#include <tuple>

#define BITS_COMPILER_INTRINSICS 1
#include <bits.hpp>

#include <test_types.hpp>

#define TypeParam T

namespace {

template<class T>
struct basic : ::testing::Test {
	T value{1};
	const T const_value = 2;
	static constexpr T constexpr_value = 3;
};

TYPED_TEST_SUITE(basic, test_types, );

TYPED_TEST(basic, constructor) {
	(void)bits{this->value};
	(void)bits{this->const_value};

	static_assert(((void)bits{this->constexpr_value}, true));
}

TYPED_TEST(basic, value_method) {
	EXPECT_EQ(bits{this->value}.value(), this->value);
	EXPECT_EQ(bits{this->const_value}.value(), this->const_value);
	constexpr auto compile_time_value = bits{this->constexpr_value}.value();
	EXPECT_EQ(compile_time_value, this->constexpr_value);

	const auto value_value = bits{this->value}.value();
	EXPECT_EQ(value_value, this->value);
	const auto const_value_value = bits{this->const_value}.value();
	EXPECT_EQ(const_value_value, this->const_value);
}

TYPED_TEST(basic, as_uint_method) {
	using type = decltype(bits{this->value}.as_uint());
	static_assert(std::is_unsigned_v<type>);

	EXPECT_EQ(bits{this->value}.as_uint(), this->value);
	EXPECT_EQ(bits{this->const_value}.as_uint(), this->const_value);
	constexpr auto compile_time_uint = bits{this->constexpr_value}.as_uint();
	EXPECT_EQ(compile_time_uint, this->constexpr_value);
}


TYPED_TEST(basic, as_method) {
	using UInt = as_uint_t<T>;
	EXPECT_EQ(bits{this->value}.template as<UInt>(), UInt(this->value));
	EXPECT_EQ(bits{this->const_value}.template as<UInt>(), UInt(this->const_value));
	constexpr auto compile_time_as = bits{this->constexpr_value}.template as<UInt>();
	EXPECT_EQ(compile_time_as, UInt(this->constexpr_value));
	if constexpr (sizeof(UInt) == sizeof(float)) {
		(void)bits{this->value}.template as<float>();
	}
}


TYPED_TEST(basic, reference_as_method) {
	using UInt = as_uint_t<T>;
	EXPECT_EQ(bits{this->value}.template as<UInt&>(), UInt(this->value));
	EXPECT_EQ(bits{this->value}.template as<const UInt&>(), UInt(this->value));
	EXPECT_EQ(bits{this->const_value}.template as<const UInt&>(), UInt(this->const_value));
	
	UInt& value_ref = bits{this->value}.template as<UInt&>();
	value_ref = 10;
	EXPECT_EQ(bits{this->value}.template as<UInt>(), 10);

	const UInt& const_value_ref = bits{this->value}.template as<const UInt&>();
	const_cast<UInt&>(const_value_ref) = 20;
	EXPECT_EQ(bits{this->value}.template as<UInt>(), 20);
}

TYPED_TEST(basic, constructor_in_place) {
	(void)bits{this->value};
	(void)bits{1.f};
}

TYPED_TEST(basic, as_bytes) {
	constexpr struct {} skip{};
	const auto bytes = bits{this->value}.as_bytes();
	
	constexpr std::tuple table{
		std::array{1_b}, std::array{1_b, 0_b}, std::array{1_b, 0_b, 0_b, 0_b},
		std::array{1_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b}
	};
	EXPECT_EQ(bytes, std::get<bit_width(sizeof(T))>(table));
}

TYPED_TEST(basic, narrow_as) {
	EXPECT_EQ(bits{this->value}.narrow().template as<int>(), 1);
	EXPECT_EQ(bits{this->value}.narrow().template as<unsigned>(), 1);
}

TYPED_TEST(basic, set) {
	bits{this->value}.set();
	EXPECT_EQ(this->value, T(-1));
	constexpr bool compile_time_set = [] {
		T val = 2;
		bits{val}.set();
		if (val != T(-1)) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_set);

	std::array<T, 4> array{1, 2, 0, 4};
	bits{array}.set();

	constexpr std::array<T, 4> expected_array{T(-1), T(-1), T(-1), T(-1)};
	EXPECT_EQ(array, expected_array);

	T c_array[4]{1, 20, 30, 40};
	bits{c_array}.set();

	EXPECT_EQ(bits{c_array}, expected_array);
}

TYPED_TEST(basic, reset) {
	bits{this->value}.reset();
	EXPECT_EQ(this->value, 0);
	constexpr bool compile_time_reset = [] {
		T val = 100;
		bits{val}.reset();
		if (val != 0) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_reset);

	std::array<T, 4> array{0, 100, 50, 25};
	bits{array}.reset();

	constexpr std::array<T, 4> expected_array{0, 0, 0, 0};
	EXPECT_EQ(array, expected_array);
}

TYPED_TEST(basic, flip) {
	bits{this->value}.flip();
	EXPECT_EQ(this->value, T(-1) - 1);
	bits{this->value}.flip();
	EXPECT_EQ(this->value, 1);

	constexpr bool compile_time_flip = [] {
		T val = 0b1010'1010u;
		bits{val}.flip();
		if (val != 0b0101'0101 - sizeof(T) == 1 ? 0 : 1 << 8) return false;
		return true;
	}();
	EXPECT_TRUE(compile_time_flip);

	std::array<T, 4> array{2, 3, 5, 7};
	bits{array}.flip();

	constexpr std::array<T, 4> expected_array{T(-1)-2, T(-1)-3, T(-1)-5, T(-1)-7};
	EXPECT_EQ(array, expected_array);

	T c_array[4]{15, 30, 56, 96};
	bits{c_array}.flip();

	constexpr std::array<T, 4> expected_c_array{T(-1)-15, T(-1)-30, T(-1)-56, T(-1)-96};
	EXPECT_EQ(bits{c_array}, expected_c_array);
}

}
