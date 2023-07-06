#pragma once

#include <type_traits>
#include <cstring>
#include <functional>

template<class Value>
class bits {
public:
	using value_type = std::remove_const_t<Value>;

	explicit constexpr bits(Value& val) noexcept : m_value(val) {}

	constexpr Value& value() { return m_value; }
	constexpr const Value& value() const { return m_value; }

private:
	template<class T> 
	static constexpr decltype(auto) unwrap(const bits<T>& val) { return val.value(); }
	template<class T>
	static constexpr decltype(auto) unwrap(const T& val) { return val; }

	template<class To, class From>
	static constexpr void bit_cast_to(To& to, const From& from) {
		static_assert(sizeof(To) == sizeof(From));
		if constexpr (std::is_trivially_assignable_v<To, From>) {
			to = from;
		} else {
			std::memcpy(&to, &from, sizeof(To));
		}
	}
	template<class To, class From>
	static constexpr void bit_cast_to(const To&, const From&) = delete;

	template<class Fn, class Left, class Right>
	static constexpr bool bit_compare(const Left& left, const Right& right) {
		static_assert(sizeof(Left) == sizeof(Right));
		if constexpr (std::is_same_v<Left, Right> && std::is_trivial_v<Left>) {
			return Fn{}(left, right);
		} else {
			return Fn{}(std::memcmp(&left, &right, sizeof(Left)), 0);
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
