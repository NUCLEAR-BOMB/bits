#pragma once

#include <type_traits>
#include <cstring>
#include <functional>
#include <cstdint>
#include <cstddef>
#include <array>
#include <utility>

#if __cplusplus >= 202002L
#include <version>
#endif

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

	[[nodiscard]] constexpr Value& value() const { return m_value; }

private:
	//#if defined(UINT64_MAX) && UINT64_MAX == UINTMAX_MAX
	//	template<> struct size_as_uint_t<8> { using type = std::uint64_t; };
	//#endif

	template<class T>
	struct type_identity { using type = T; };
	template<class T> using type_identity_t = typename type_identity<T>::type;

	template<std::size_t N>
	static decltype(auto) size_as_uint_impl() {
		if constexpr (N <= 1) return std::uint8_t{};
		else if constexpr (N <= 2) return std::uint16_t{};
		else if constexpr (N <= 4) return std::uint32_t{};
		else if constexpr (N <= 8) return std::uint64_t{};
		else return void();
	}

#if defined(UINT64_MAX) && UINT64_MAX <= UINTMAX_MAX
	using local_uintmax_t = std::uint64_t;
#else
	using local_uintmax_t = std::uintmax_t;
#endif

	template<std::size_t N>
	using size_as_uint = decltype(size_as_uint_impl<N>());
	template<class T>
	using type_as_uint = size_as_uint<sizeof(T)>;
	template<class T>
	using type_as_int = std::make_signed_t<type_as_uint<T>>;

	template<class T, bool = std::is_enum_v<T>>
	struct safe_underlying_type_impl { using type = std::underlying_type_t<T>; };
	template<class T>
	struct safe_underlying_type_impl<T, false> { using type = T; };

	template<class T>
	using safe_underlying_type = typename safe_underlying_type_impl<T>::type;

	template<class T>
	static constexpr bool is_like_integral = std::is_integral_v<T> || std::is_enum_v<T>;

	template<class T>
	static constexpr bool is_convertible_as_uint = (sizeof(T) <= sizeof(local_uintmax_t));

	template<class From, class To>
	static constexpr bool is_bit_convertible = is_like_integral<From> && is_like_integral<To>;

	template<class T>
	static constexpr T denominator_of_power2(const T x) {
		const auto result = T(x & (~(x - 1)));
		return result > sizeof(local_uintmax_t) ? sizeof(local_uintmax_t) : result;
	}

	template<class T>
	static constexpr bool is_integer_array = false;
	template<class T, std::size_t N>
	static constexpr bool is_integer_array<std::array<T, N>> = std::is_integral_v<T> || std::is_enum_v<T>;
	template<class T, std::size_t N>
	static constexpr bool is_integer_array<T[N]> = std::is_integral_v<T> || std::is_enum_v<T>;


	template<class T, std::size_t Size = denominator_of_power2(sizeof(T))>
	using as_max_size_array = std::array<size_as_uint<Size>, sizeof(T) / Size>;

	template<class T, class UIntMax = local_uintmax_t>
	using type_as_least_uintmax_array = std::array<UIntMax, sizeof(T) / sizeof(UIntMax)>;

	using bitsize_t = local_uintmax_t;

	template<class T>
	using array_value_type = std::decay_t<decltype(std::declval<T&>()[std::size_t(0)])>;

	template<class T>
	static constexpr decltype(auto) unwrap(const bits<T>& val) { return val.value(); }
	template<class T>
	static constexpr decltype(auto) unwrap(const T& val) { return val; }

	static constexpr bool is_constant_evaluated() {
#ifdef __cpp_lib_is_constant_evaluated
		return std::is_constant_evaluated();
#elif BITS_HAS_IS_CONSTANT_EVALUATED_INTRINSICS
		return __builtin_is_constant_evaluated();
#else
		return false;
#endif
	}

	template<class To, class From>
	static constexpr To bit_cast(const From& from) {
		static_assert(sizeof(To) == sizeof(From));
#ifdef __cpp_lib_bit_cast
		return std::bit_cast<To>(from);
#elif BITS_HAS_BIT_CAST_INTRINSICS
		return __builtin_bit_cast(To, from);
#else
		if constexpr (is_bit_convertible<From, To>) {
			return static_cast<To>(from);
		} else {
			To result{};
			std::memcpy(&result, &from, sizeof(From));
			return result;
		}
#endif
	}

	template<class To, class From>
	static constexpr void bit_cast_to(To& to, const From& from) {
		static_assert(sizeof(To) == sizeof(From));
		if constexpr (std::is_trivially_copy_assignable_v<To>) {
			to = bit_cast<To>(from);
		} else {
			std::memcpy(&to, &from, sizeof(To));
		}
	}
	template<class To, class From>
	static constexpr void bit_cast_to(const To&, const From&) = delete;

	template<class To, class From>
	static constexpr void expand_bit_cast_to(To& to, const From& from) {
		static_assert(sizeof(To) >= sizeof(From));

		if constexpr (sizeof(To) == sizeof(From)) {
			bit_cast_to(to, from);
		} else {
			std::memcpy(&to, &from, sizeof(From));
			const auto to_as_bytes = reinterpret_cast<std::byte*>(&to);
			std::memset(to_as_bytes + sizeof(From), 0, sizeof(To) - sizeof(From));
		}
	}
	template<class To, class From>
	static constexpr void expand_bit_cast_to(const To&, const From&) = delete;

	template<class To, class From>
	static constexpr void narrow_bit_cast_to(To& to, const From& from) {
		if constexpr (is_bit_convertible<From, To> && std::is_trivially_copy_assignable_v<To>) {
			to = static_cast<To>(from);
		} else if constexpr (sizeof(To) >= sizeof(From)) {
			expand_bit_cast_to(to, from);
		} else {
			std::memcpy(&to, &from, sizeof(To));
		}
	}
	template<class To, class From>
	static constexpr void narrow_bit_cast_to(const To&, const From&) = delete;

	template<class Fn, class Left, class Right>
	static constexpr bool bit_compare(const Left& left, const Right& right) {
		static_assert(sizeof(Left) == sizeof(Right));
		using T = Left;
		if constexpr (is_like_integral<Left> && is_like_integral<Right>) {
			return Fn{}(type_as_uint<Left>(left), type_as_uint<Right>(right));
		} else if constexpr (is_convertible_as_uint<T>) {
			using uint_type = type_as_uint<T>;
			return Fn{}(bit_cast<uint_type>(left), bit_cast<uint_type>(right));
		} else {
			using as_array_type = as_max_size_array<T>;
			return Fn{}(bit_cast<as_array_type>(left), bit_cast<as_array_type>(right));
		}
	}

	template<bool SetTo, class T>
	static constexpr void bit_set(T& value) {
		if constexpr (std::is_integral_v<T>) {
			value = SetTo ? T(-1) : T(0);
		} else if constexpr (is_integer_array<T>) {
			for (auto& x : value) {
				bit_set<SetTo>(x);
			}
		} else {
			if (is_constant_evaluated()) {
				using array_type = as_max_size_array<T>;
				using array_uvalue_type = typename array_type::value_type;

				auto value_as_arr = bit_cast<array_type>(value);
				bit_set<SetTo>(value_as_arr);
				value = bit_cast<T>(value_as_arr);
				return;
			}
			std::memset(&value, SetTo ? 0xFF : 0, sizeof(T));
		}
	}
	template<class T>
	static constexpr void bit_flip(T& value) {
		if constexpr (is_like_integral<T>) {
			using integer_t = safe_underlying_type<T>;
			value = T(~integer_t(value));
		} else if constexpr (is_integer_array<T>) {
			for (auto& x : value) {
				bit_flip(x);
			}
		} else {
			if constexpr (is_convertible_as_uint<T>) {
				using uint_type = type_as_uint<T>;
				const auto uint_value = bit_cast<uint_type>(value);
				bit_cast_to(value, uint_type(~uint_value));
			} else {
				using first_type = type_as_least_uintmax_array<T>;
				using rest_type = size_as_uint<sizeof(first_type) - sizeof(T)>;
				bit_flip(*reinterpret_cast<first_type*>(&value));

				std::byte* offset = reinterpret_cast<std::byte*>(&value) + sizeof(first_type);
				bit_flip(*reinterpret_cast<rest_type*>(offset));
			}
		}
	}
	template<class T>
	static constexpr void bit_flip_at(T& value, const bitsize_t index) {
		if constexpr (std::is_integral_v<T>) {
			value ^= T(1) << index;
		} else if constexpr (std::is_enum_v<T>) {
			auto buffer = static_cast<std::underlying_type_t<T>>(value);
			bit_flip_at(buffer, index);
			value = static_cast<T>(buffer);
		} else if constexpr (is_integer_array<T>) {
			using elem_type = array_value_type<T>;
			const auto array_index = index / (CHAR_BIT*sizeof(elem_type));
			const auto bit_index = index % (CHAR_BIT*sizeof(elem_type));
			bit_flip_at(value[array_index], bit_index);
		} else {
			using byte_array = std::array<std::byte, sizeof(T)>;
			bit_flip_at(reinterpret_cast<byte_array&>(value), index);
		}
	}

	template<class T>
	static constexpr void bit_assign(T& value, const bitsize_t index, const bool what_value) {
		if constexpr (std::is_integral_v<T>) {
			value &= ~(T(1) << index);
			value |= T(what_value) << index;
		} else if constexpr (std::is_enum_v<T>) {
			auto buffer = static_cast<std::underlying_type_t<T>>(value);
			bit_assign(buffer, index, what_value);
			value = static_cast<T>(buffer);
		} else if constexpr (is_integer_array<T>) {
			using elem_type = array_value_type<T>;
			const auto array_index = index / (CHAR_BIT*sizeof(elem_type));
			const auto bit_index = index % (CHAR_BIT*sizeof(elem_type));
			bit_assign(value[array_index], bit_index, what_value);
		} else {
			using byte_array = std::array<std::byte, sizeof(T)>;
			if (is_constant_evaluated() && std::is_trivially_copy_assignable_v<T>) {
				auto buffer = bit_cast<byte_array>(value);
				bit_assign(buffer, index, what_value);
				value = bit_cast<T>(buffer);
				return;
			}
			bit_assign(reinterpret_cast<byte_array&>(value), index, what_value);
		}
	}
	template<class T>
	static constexpr bool bit_get(const T& value, const bitsize_t index) {
		if constexpr (is_like_integral<T>) {
			using integer_t = safe_underlying_type<T>;
			return (integer_t(value) & (integer_t(1) << index)) > 0;
		} else if constexpr (is_integer_array<T>) {
			using elem_type = array_value_type<T>;
			const auto array_index = index / (CHAR_BIT*sizeof(elem_type));
			const auto bit_index = index % (CHAR_BIT*sizeof(elem_type));
			return bit_get(value[array_index], bit_index);
		} else {
			using byte_array = std::array<std::byte, sizeof(T)>;
			return bit_get(bit_cast<byte_array>(value), index);
		}
	}
	
	template<class T>
	static constexpr bool bit_test_all(const T& value) {
		if constexpr (is_like_integral<T>) {
			return value == T(-1);
		} else if constexpr (is_integer_array<T>) {
			for (auto& x : value) {
				if (x != T(-1)) return false;
			}
			return true;
		} else {
#if 0
			static constexpr std::array<std::byte, sizeof(T)> all_array_cmp = [] {
				std::array<std::byte, sizeof(T)> result;
				for (auto& x : result) x = std::byte{0xFF};
				return result;
			}();
			return std::memcmp(&value, all_array_cmp.data(), sizeof(T)) == 0;
#else
			if constexpr (is_convertible_as_uint<T>) {
				using as_uint_type = type_as_uint<T>;
				return bit_test_all(bit_cast<as_uint_type>(value));
			} else {
				using uintmax_array_type = std::array<local_uintmax_t, sizeof(T) / sizeof(local_uintmax_t)>;
				const bool first_result = bit_test_all(*reinterpret_cast<uintmax_array_type*>(&value));
				if (!first_result) return false;

				using padding_type = size_as_uint<sizeof(T) % sizeof(local_uintmax_t)>;
				const auto* padding_ptr = reinterpret_cast<std::byte*>(&value) + sizeof(uintmax_array_type);
				const auto padding_result = bit_test_all(*reinterpret_cast<padding_type*>(padding_ptr));
				return padding_result;
			}
#endif
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

	template<class T>
	constexpr auto as_integer_sign_of() const {
		if constexpr (std::is_unsigned_v<T>) {
			return as_uint();
		} else {
			return as_int();
		}
	}
	template<class T>
	constexpr bits& narrow_assign_integer(const T& x) {
		bit_cast_to(m_value, static_cast<type_as_uint<value_type>>(x));
		return *this;
	}

	class narrow_t {
		Value& value;
	public:
		explicit constexpr narrow_t(Value& value) : value(value) {}

		template<class T>
		constexpr void operator=(const T& x) {
			narrow_bit_cast_to(value, x);
		}

		template<class To>
		constexpr To as() const {
			static_assert(!std::is_reference_v<To>);
			To result;
			narrow_bit_cast_to(result, value);
			return result;
		}
	};

public:

	[[nodiscard]] constexpr narrow_t narrow() {
		return narrow_t(m_value);
	}

	template<class Byte = std::uint_least8_t>
	[[nodiscard]] constexpr auto as_bytes() const {
		static_assert(sizeof(Byte) == sizeof(std::uint_least8_t));
		using byte_array = std::array<Byte, sizeof(value_type) / sizeof(std::uint_least8_t)>;
		return bit_cast<byte_array>(m_value);
	}
	template<class Byte = std::uint_least8_t>
	[[nodiscard]] constexpr auto& as_bytes_ref() {
		static_assert(sizeof(Byte) == sizeof(std::uint_least8_t));
		using byte_array = std::array<Byte, sizeof(value_type) / sizeof(std::uint_least8_t)>;
		return as<byte_array&>();
	}

	[[nodiscard]] constexpr auto as_uint() const {
		static_assert(is_convertible_as_uint<value_type>, 
			"Can't be represented as an unsigned integer");
		return bit_cast<type_as_uint<value_type>>(m_value);
	}
	[[nodiscard]] constexpr auto as_int() const {
		static_assert(is_convertible_as_uint<value_type>,
			"Can't be represented as an signed integer");
		return bit_cast<type_as_int<value_type>>(m_value);
	}

	template<class To, std::enable_if_t<!std::is_reference_v<To>, int> = 0>
	[[nodiscard]] constexpr To as() const {
		static_assert(sizeof(To) == sizeof(value_type));
		return bit_cast<To>(m_value);
	}
	template<class To, std::enable_if_t<std::is_reference_v<To>, int> = 0>
	[[nodiscard]] constexpr To as() {
		static_assert(sizeof(To) == sizeof(value_type));
		return reinterpret_cast<To&>(m_value);
	}

	template<class T>
	constexpr bits& operator=(const T& other) {
		static_assert(sizeof(value_type) == sizeof(T));
		expand_bit_cast_to(m_value, other);
		return *this;
	}

	constexpr void set() {
		bit_set<true>(m_value);
	}
	constexpr void reset() {
		bit_set<false>(m_value);
	}
	constexpr void flip() {
		bit_flip(m_value);
	}
	constexpr void flip_at(const bitsize_t index) {
		bit_flip_at(m_value, index);
	}

	[[nodiscard]] constexpr bool all() const {
		return bit_test_all(m_value);
	}

	[[nodiscard]] constexpr auto copy() const {
		return bit_cast<value_type>(m_value);
	}
	template<class To>
	constexpr void copy_to(To& to) {
		static_assert(sizeof(To) == sizeof(value_type));
		bit_cast_to(to, m_value);
	}
	template<class To> constexpr void copy_to(const To&) = delete;

	template<class T>
	constexpr bits& operator+=(const T& x) {
		return narrow_assign_integer(as_integer_sign_of<T>() + x);
	}
	template<class T>
	constexpr bits& operator-=(const T& x) {
		return narrow_assign_integer(as_integer_sign_of<T>() - x);
	}
	template<class T>
	constexpr bits& operator*=(const T& x) {
		return narrow_assign_integer(as_integer_sign_of<T>() * x);
	}
	template<class T>
	constexpr bits& operator/=(const T& x) {
		return narrow_assign_integer(as_int() / x);
	}
	template<class T>
	constexpr bits& operator%=(const T& x) {
		return narrow_assign_integer(as_uint() % x);
	}
	template<class T>
	constexpr bits& operator&=(const T& x) {
		return narrow_assign_integer(as_uint() & x);
	}
	template<class T>
	constexpr bits& operator|=(const T& x) {
		return narrow_assign_integer(as_uint() | x);
	}
	template<class T>
	constexpr bits& operator^=(const T& x) {
		return narrow_assign_integer(as_uint() ^ x);
	}
	template<class T>
	constexpr bits& operator<<=(const T& x) {
		return narrow_assign_integer(as_uint() << x);
	}
	template<class T>
	constexpr bits& operator>>=(const T& x) {
		return narrow_assign_integer(as_uint() >> x);
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

	class reference {
	public:
		constexpr reference(const bitsize_t index, Value& value) : index(index), value(value) {}

		constexpr reference& operator=(const bool x) {
			bit_assign(value, index, x);
			return *this;
		}
		[[nodiscard]] constexpr bool get() const { return bit_get(value, index); }
		[[nodiscard]] constexpr operator bool() const { return get(); }

		constexpr void flip() {
			bit_flip_at(value, index);
		}

	private:
		const bitsize_t index;
		Value& value;
	};

	[[nodiscard]] constexpr reference operator[](const bitsize_t index) {
		return reference(index, m_value);
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
