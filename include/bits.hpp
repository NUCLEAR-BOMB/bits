#pragma once

#include <type_traits>
#include <cstring>
#include <functional>
#include <cstdint>
#include <cstddef>
#include <array>

#if defined(BITS_COMPILER_INTRINSICS) && BITS_COMPILER_INTRINSICS
	#define BITS_ENABLE_INTRINSICS 1
	#ifdef __has_builtin
		#define BITS_HAS_BUILTIN(x) __has_builtin(x)
	#else
		#define BITS_HAS_BUILTIN(x) 0
	#endif
#else
	#define BITS_ENABLE_INTRINSICS 0
	#define BITS_HAS_BUILTIN(x) 0
#endif

#if BITS_ENABLE_INTRINSICS && (defined(_MSC_VER) || BITS_HAS_BUILTIN(__builtin_bit_cast))
	#define BITS_HAS_BIT_CAST_INTRINSICS 1
#else
	#define BITS_HAS_BIT_CAST_INTRINSICS 0
#endif
#if BITS_ENABLE_INTRINSICS && (defined(_MSC_VER) || BITS_HAS_BUILTIN(__builtin_is_constant_evaluated))
	#define BITS_HAS_IS_CONSTANT_EVALUATED_INTRINSICS 1
#else
	#define BITS_HAS_IS_CONSTANT_EVALUATED_INTRINSICS 0
#endif

template<class Value>
class bits {
public:
	using value_type = std::remove_const_t<Value>;

	explicit constexpr bits(Value& val) noexcept : m_value(val) {}

	constexpr Value& value() { return m_value; }
	constexpr const Value& value() const { return m_value; }

private:
	template<std::size_t> struct size_as_uint_t;
	template<> struct size_as_uint_t<1> { using type = std::uint8_t; };
	template<> struct size_as_uint_t<2> { using type = std::uint16_t; };
	template<> struct size_as_uint_t<4> { using type = std::uint32_t; };
#if defined(UINT64_MAX) && UINT64_MAX == UINTMAX_MAX
	template<> struct size_as_uint_t<8> { using type = std::uint64_t; };
#endif

#if defined(UINT64_MAX) && UINT64_MAX <= UINTMAX_MAX
	using local_uintmax_t = std::uint64_t;
#else
	using local_uintmax_t = std::uintmax_t;
#endif

	template<std::size_t N>
	using size_as_uint = typename size_as_uint_t<N>::type;
	template<class T>
	using type_as_uint = size_as_uint<sizeof(T)>;

	template<class T>
	static constexpr bool is_convertible_as_uint = (sizeof(T) <= sizeof(local_uintmax_t));

	template<class T>
	static constexpr T denominator_of_power2(const T x) {
		const auto result = T(x & (~(x - 1)));
		return result > sizeof(local_uintmax_t) ? sizeof(local_uintmax_t) : result;
	}

	template<class T> 
	static constexpr decltype(auto) unwrap(const bits<T>& val) { return val.value(); }
	template<class T>
	static constexpr decltype(auto) unwrap(const T& val) { return val; }

	template<class To, class From>
	static constexpr void bit_cast_to(To& to, const From& from) {
		static_assert(sizeof(To) == sizeof(From));
		if constexpr (std::is_integral_v<To> && std::is_integral_v<From>) {
			to = static_cast<To>(from);
		} else {
#if BITS_HAS_BIT_CAST_INTRINSICS
			if constexpr (std::is_trivially_copy_constructible_v<To>) {
				to = __builtin_bit_cast(To, from);
				return;
			}
#endif
			std::memcpy(&to, &from, sizeof(To));
		}
	}
	template<class To, class From>
	static constexpr void bit_cast_to(const To&, const From&) = delete;

	template<class To, class From>
	static constexpr To bit_cast(const From& from) {
#if BITS_HAS_BIT_CAST_INTRINSICS
		return __builtin_bit_cast(To, from);
#else
		To result{};
		bit_cast_to(result, from);
		return result;
#endif
	}

	template<class Fn, class Left, class Right>
	static constexpr bool bit_compare(const Left& left, const Right& right) {
		static_assert(sizeof(Left) == sizeof(Right));
		using T = Left;
		if constexpr (std::is_integral_v<Left> && std::is_integral_v<Right>) {
			return Fn{}(type_as_uint<Left>(left), type_as_uint<Right>(right));
		} else if constexpr (is_convertible_as_uint<T>) {
			using uint_type = type_as_uint<T>;
			return Fn{}(bit_cast<uint_type>(left), bit_cast<uint_type>(right));
		} else {
			constexpr std::size_t pow2_denom = denominator_of_power2(sizeof(T));
			using as_array_type = std::array<size_as_uint<pow2_denom>, sizeof(T) / pow2_denom>;
			return Fn{}(bit_cast<as_array_type>(left), bit_cast<as_array_type>(right));
		}
	}

	template<class T>
	static constexpr bool is_bits_type = false;
	template<class T>
	static constexpr bool is_bits_type<bits<T>> = true;

	template<class T1, class T2>
	using enable_compare_operator = std::enable_if_t<
		(std::is_same_v<T1, bits> && !is_bits_type<T2>) || std::is_same_v<T2, bits>, int
	>;

public:

	constexpr auto uint() const {
		static_assert(is_convertible_as_uint<value_type>, 
			"can't be represented as an unsigned integer");
		return bit_cast<type_as_uint<value_type>>(m_value);
	}

	template<class T>
	constexpr bits& operator=(const T& other) {
		static_assert(sizeof(value_type) == sizeof(T));
		bit_cast_to(m_value, other);
		return *this;
	}
	
	template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
	friend constexpr bool operator==(const Left& left, const Right& right) {
		static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)), 
			"The size of the operands is not equal to");
		return bit_compare<std::equal_to<>>(unwrap(left), unwrap(right));
	}
	template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
	friend constexpr bool operator!=(const Left& left, const Right& right) {
		static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
			"The size of the operands is not equal to");
		return bit_compare<std::not_equal_to<>>(unwrap(left), unwrap(right));
	}
	template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
	friend constexpr bool operator>(const Left& left, const Right& right) {
		static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
			"The size of the operands is not equal to");
		return bit_compare<std::greater<>>(unwrap(left), unwrap(right));
	}
	template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
	friend constexpr bool operator>=(const Left& left, const Right& right) {
		static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
			"The size of the operands is not equal to");
		return bit_compare<std::greater_equal<>>(unwrap(left), unwrap(right));
	}
	template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
	friend constexpr bool operator<(const Left& left, const Right& right) {
		static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
			"The size of the operands is not equal to");
		return bit_compare<std::less<>>(unwrap(left), unwrap(right));
	}
	template<class Left, class Right, enable_compare_operator<Left, Right> = 0>
	friend constexpr bool operator<=(const Left& left, const Right& right) {
		static_assert(sizeof(unwrap(left)) == sizeof(unwrap(right)),
			"The size of the operands is not equal to");
		return bit_compare<std::less_equal<>>(unwrap(left), unwrap(right));
	}


private:
	Value& m_value;
};

template<class T>
bits(T&) -> bits<T>;
template<class T>
bits(const T&) -> bits<const T>;

#undef BITS_ENABLE_INTRINSICS
#undef BITS_HAS_BUILTIN
#undef BITS_HAS_BIT_CAST_INTRINSICS
#undef BITS_HAS_IS_CONSTANT_EVALUATED_INTRINSICS
