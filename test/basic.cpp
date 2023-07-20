#include <gtest/gtest.h>
#include <tuple>

#define BITS_COMPILER_INTRINSICS 1
#include <bits.hpp>

#include <test_types.hpp>

namespace {

template<class T>
struct basic : ::testing::Test {
	T value{1};
	const T const_value = 2;
	static constexpr T constexpr_value = 3;
};

TYPED_TEST_SUITE(basic, test_types, );

TYPED_TEST(basic, constructor) {
	(void)bits {
	this->value
};
	(void)bits {
		this->const_value
	};
	static_assert(((void)bits { this->constexpr_value }, true));
}

TYPED_TEST(basic, value_method) {
	EXPECT_EQ(bits{this->value}.value(), this->value);
	EXPECT_EQ(bits{this->const_value}.value(), this->const_value);
	static_assert(bits{this->constexpr_value}.value() == this->constexpr_value);

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
	static_assert(bits{this->constexpr_value}.as_uint() == this->constexpr_value);
}


TYPED_TEST(basic, as_method) {
	using T = as_uint_t<TypeParam>;
	EXPECT_EQ(bits{this->value}.template as<T>(), T(this->value));
	EXPECT_EQ(bits{this->const_value}.template as<T>(), T(this->const_value));
	static_assert(bits{this->constexpr_value}.template as<T>() == T(this->constexpr_value));
	if constexpr (sizeof(T) == sizeof(float)) {
		(void)bits {
		this->value
	}.template as<float>();
	}
}


TYPED_TEST(basic, reference_as_method) {
	using T = as_uint_t<TypeParam>;
	EXPECT_EQ(bits{this->value}.template as<T&>(), T(this->value));
	EXPECT_EQ(bits{this->value}.template as<const T&>(), T(this->value));
	EXPECT_EQ(bits{this->const_value}.template as<const T&>(), T(this->const_value));
	
	T& value_ref = bits{this->value}.template as<T&>();
	value_ref = 10;
	EXPECT_EQ(bits{this->value}.template as<T>(), 10);

	const T& const_value_ref = bits{this->value}.template as<const T&>();
	const_cast<T&>(const_value_ref) = 20;
	EXPECT_EQ(bits{this->value}.template as<T>(), 20);
}

TYPED_TEST(basic, constructor_in_place) {
	(void)bits{this->value};
	(void)bits{1.f};
}

TYPED_TEST(basic, as_bytes) {
	using T = TypeParam;
	constexpr struct {} skip{};
	const auto bytes = bits{this->value}.as_bytes();
	constexpr auto make = [](auto... x) { return make_array(x...); };
	constexpr std::tuple byte_arrays{
		make(1_b), make(1_b, 0_b), skip, make(1_b, 0_b, 0_b, 0_b), skip, skip, skip,
		make(1_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b)
	};
	EXPECT_EQ(bytes, std::get<sizeof(T)-1>(byte_arrays));
}

TYPED_TEST(basic, narrow_as) {
	EXPECT_EQ(bits{this->value}.narrow().template as<int>(), 1);
	EXPECT_EQ(bits{this->value}.narrow().template as<unsigned>(), 1);
}

}
