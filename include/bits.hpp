#pragma once

#include <type_traits>
#include <cstring>
#include <functional>

template<class Value>
class bits {
public:
	using value_type = std::remove_const_t<Value>;

	explicit constexpr bits(Value& val) noexcept : m_value(val) {}

	bits(Value&&) = delete;

	constexpr Value& value() { return m_value; }
	constexpr const Value& value() const { return m_value; }

private:
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

public:

	template<class T>
	constexpr bits& operator=(const T& other) {
		static_assert(sizeof(value_type) == sizeof(T));
		bit_cast_to(m_value, other);
		return *this;
	}
	
	template<class T>
	friend constexpr bool operator==(const bits& left, const T& right) {
		static_assert(sizeof(value_type) == sizeof(T));
		return bit_compare<std::equal_to<>>(left.m_value, right);
	}
	template<class T>
	friend constexpr bool operator==(const T& left, const bits& right) {
		static_assert(sizeof(value_type) == sizeof(T));
		return bit_compare<std::equal_to<>>(left, right.m_value);
	}

private:
	Value& m_value;
};

template<class Value>
bits(Value&) -> bits<Value>;
