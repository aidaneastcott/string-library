
#pragma once
#ifndef SIMPLE_STRING_HPP
#define SIMPLE_STRING_HPP


#include <algorithm>
#include <initializer_list>
#include <memory>
#include <new>
#include <iostream>
#include <utility>

#include <cassert>
#include <cstddef>


#if defined(_MSC_VER)

#define assume(expr) __assume(expr)

#elif defined(__GNUC__) || defined(__clang__)

#define assume(expr)  do { if (!(expr)) __builtin_unreachable(); } while (0)

#else

#define assume(expr)

#endif

#define assert_assume(expr)  do { assert(expr); assume(expr); } while (0)



namespace simple {


template <typename CharType>
class StringType {
public:

	// Type Aliases

	using ValueType = CharType;
	using SizeType = std::size_t;

	using DifferenceType = std::ptrdiff_t;

	using Reference = ValueType &;
	using ConstReference = const ValueType &;

	using Pointer = ValueType *;
	using ConstPointer = const ValueType *;


private:

	// Data Members

	Pointer m_data{};
	SizeType m_size{};
	SizeType m_capacity{};

	// Constants

	static constexpr ValueType NUL_TERMINATION = '\0';

	// Utility Functions

	static SizeType lookupCapacity(SizeType) noexcept;
	static SizeType cstringSize(ConstPointer) noexcept;

	// Constructors

	StringType(Pointer, SizeType, SizeType) noexcept;

public:

	// Constructors

	StringType() noexcept;
	explicit StringType(std::initializer_list<ValueType>);
	StringType(ValueType, SizeType = 1);
	StringType(ConstPointer);
	StringType(const StringType &);
	StringType(StringType &&) noexcept;

	// Destructor

	~StringType() noexcept;

	// Assignment Operations

	StringType &operator=(ValueType);
	StringType &operator=(ConstPointer);
	StringType &operator=(const StringType &);
	StringType &operator=(StringType &&) noexcept;

	// Size Functions

	SizeType size() const noexcept;
	bool empty() const noexcept;

	// Capacity Functions

	SizeType capacity() const noexcept;
	void reserve(SizeType);
	void shrink();
	void deallocate() noexcept;

	// Data Access Functions

	ConstPointer data() const noexcept;
	ConstPointer cstring() const noexcept;

	ConstReference operator[](SizeType) const noexcept;
	Reference operator[](SizeType) noexcept;

	ConstReference front() const noexcept;
	Reference front() noexcept;
	ConstReference back() const noexcept;
	Reference back() noexcept;

	// Mutation Functions

	void clear() noexcept;

	void popback(SizeType = 1) noexcept;
	void trim(SizeType = 1) noexcept;

	void erase(SizeType) noexcept;
	void erase(SizeType, SizeType) noexcept;

	void insert(ValueType, SizeType = 0);
	void insert(ConstPointer, SizeType = 0);
	void insert(const StringType &, SizeType = 0);
	void insert(StringType &&, SizeType = 0);

	StringType &operator+=(ValueType);
	StringType &operator+=(ConstPointer);
	StringType &operator+=(const StringType &);
	StringType &operator+=(StringType &&);

	StringType substring(SizeType) const &;
	StringType substring(SizeType, SizeType) const &;
	StringType substring(SizeType) && noexcept;
	StringType substring(SizeType, SizeType) && noexcept;

	// Comparison Functions

	int compare(ConstPointer) const noexcept;
	int compare(const StringType &) const noexcept;

	// Mutation Operations

	template <typename ValueType>
	friend StringType<ValueType> operator+(const StringType<ValueType> &, const StringType<ValueType> &);
	template <typename ValueType>
	friend StringType<ValueType> operator+(const StringType<ValueType> &, ConstPointer);
	template <typename ValueType>
	friend StringType<ValueType> operator+(ConstPointer, const StringType<ValueType> &);
	template <typename ValueType>
	friend StringType<ValueType> operator+(const StringType<ValueType> &, ValueType);
	template <typename ValueType>
	friend StringType<ValueType> operator+(ValueType, const StringType<ValueType> &);

	template <typename ValueType>
	friend StringType<ValueType> operator+(StringType<ValueType> &&, StringType<ValueType> &&);
	template <typename ValueType>
	friend StringType<ValueType> operator+(const StringType<ValueType> &, StringType<ValueType> &&);
	template <typename ValueType>
	friend StringType<ValueType> operator+(StringType<ValueType> &&, const StringType<ValueType> &);
	template <typename ValueType>
	friend StringType<ValueType> operator+(StringType<ValueType> &&, ConstPointer);
	template <typename ValueType>
	friend StringType<ValueType> operator+(ConstPointer, StringType<ValueType> &&);
	template <typename ValueType>
	friend StringType<ValueType> operator+(StringType<ValueType> &&, ValueType);
	template <typename ValueType>
	friend StringType<ValueType> operator+(ValueType, StringType<ValueType> &&);

	// Comparison Operations

	template <typename ValueType>
	friend bool operator==(const StringType<ValueType> &, const StringType<ValueType> &) noexcept;
	template <typename ValueType>
	friend bool operator==(const StringType<ValueType> &, ConstPointer) noexcept;
	template <typename ValueType>
	friend bool operator==(ConstPointer, const StringType<ValueType> &) noexcept;
	template <typename ValueType>
	friend bool operator!=(const StringType<ValueType> &, const StringType<ValueType> &) noexcept;
	template <typename ValueType>
	friend bool operator!=(const StringType<ValueType> &, ConstPointer) noexcept;
	template <typename ValueType>
	friend bool operator!=(ConstPointer, const StringType<ValueType> &) noexcept;

	// Output Stream Operations

	template <typename ValueType>
	friend std::ostream &operator<<(std::ostream &, const StringType<ValueType> &);
};


// Utility Functions

/*
*/
template <typename ValueType>
typename StringType<ValueType>::SizeType StringType<ValueType>::lookupCapacity(SizeType size) noexcept {

	assert_assume(size < std::numeric_limits<SizeType>::max());

	if (size == 0) {
		return 0;
	}

	++size;

	static constexpr SizeType SEED = 15;
	SizeType capacity = SEED;

	while (size > capacity) {
		capacity *= 2;
	}

	return capacity;
}

/*
*/
template <typename ValueType>
typename StringType<ValueType>::SizeType StringType<ValueType>::cstringSize(ConstPointer cstring) noexcept {

	SizeType size = 0;

	while (cstring[size] != NUL_TERMINATION) {
		++size;
	}

	return size;
}


// Constructors

/*
*/
template <typename ValueType>
StringType<ValueType>::StringType(Pointer data, SizeType size, SizeType capacity) noexcept :
	m_data{data}, m_size{size}, m_capacity{capacity} {

	assert_assume(data != nullptr || (size == 0 && capacity == 0));
	assert_assume(size < capacity || (size == 0 && capacity == 0));
}

/*
*/
template <typename ValueType>
StringType<ValueType>::StringType() noexcept = default;

/*
*/
template <typename ValueType>
StringType<ValueType>::StringType(std::initializer_list<ValueType> list) {

	if (list.size() == 0) {
		return;
	}

	SizeType capacity = lookupCapacity(list.size());
	assume(list.size() < capacity);

	m_data = new ValueType[capacity];
	m_size = list.size();
	m_capacity = capacity;

	std::copy(list.begin(), list.end(), m_data);
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
StringType<ValueType>::StringType(ValueType character, SizeType size) {

	if (size == 0) {
		return;
	}

	SizeType capacity = lookupCapacity(size);
	assume(size < capacity);

	m_data = new ValueType[capacity];
	m_size = size;
	m_capacity = capacity;

	std::fill(m_data, m_data + m_size, character);

	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
StringType<ValueType>::StringType(ConstPointer cstring) {

	assert_assume(cstring != nullptr);

	SizeType size = cstringSize(cstring);

	if (size == 0) {
		return;
	}

	SizeType capacity = lookupCapacity(size);
	assume(size < capacity);

	m_data = new ValueType[capacity];
	m_size = size;
	m_capacity = capacity;

	std::copy(cstring, cstring + m_size, m_data);
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
StringType<ValueType>::StringType(const StringType &object) {

	if (object.m_size == 0) {
		return;
	}

	SizeType capacity = lookupCapacity(object.m_size);
	assume(object.m_size < capacity);

	m_data = new ValueType[capacity];
	m_size = object.m_size;
	m_capacity = capacity;

	std::copy(object.m_data, object.m_data + m_size, m_data);
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
StringType<ValueType>::StringType(StringType &&object) noexcept :
	m_data{object.m_data}, m_size{object.m_size}, m_capacity{object.m_capacity} {

	object.m_data = nullptr;
	object.m_size = 0;
	object.m_capacity = 0;
}


// Destructor

/*
*/
template <typename ValueType>
StringType<ValueType>::~StringType() noexcept {

	delete[] m_data;
	m_data = nullptr;
	m_size = 0;
	m_capacity = 0;
}


// Assignment Operations

/*
*/
template <typename ValueType>
StringType<ValueType> &StringType<ValueType>::operator=(ValueType character) {

	SizeType size = 1;
	SizeType capacity = lookupCapacity(size);
	assume(size < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}

	m_size = size;

	m_data[0] = character;
	m_data[m_size] = NUL_TERMINATION;

	return *this;
}

/*
*/
template <typename ValueType>
StringType<ValueType> &StringType<ValueType>::operator=(ConstPointer cstring) {

	assert_assume(cstring != nullptr);

	SizeType size = cstringSize(cstring);

	if (size == 0) {
		if (m_capacity > 0) {
			m_data[0] = NUL_TERMINATION;
			m_size = 0;
		}
		return *this;
	}

	SizeType capacity = lookupCapacity(size);
	assume(size < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}

	m_size = size;

	std::copy(cstring, cstring + m_size, m_data);
	m_data[m_size] = NUL_TERMINATION;

	return *this;
}

/*
*/
template <typename ValueType>
StringType<ValueType> &StringType<ValueType>::operator=(const StringType &object) {

	if (object.m_size == 0) {
		if (m_capacity > 0) {
			m_data[0] = NUL_TERMINATION;
			m_size = object.m_size;
		}
		return *this;
	}

	SizeType capacity = lookupCapacity(object.m_size);
	assume(object.m_size < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}

	m_size = object.m_size;

	std::copy(object.m_data, object.m_data + m_size, m_data);
	m_data[m_size] = NUL_TERMINATION;

	return *this;
}

/*
*/
template <typename ValueType>
StringType<ValueType> &StringType<ValueType>::operator=(StringType &&object) noexcept {

	Pointer data = object.m_data;
	SizeType size = object.m_size;
	SizeType capacity = object.m_capacity;

	object.m_data = nullptr;
	object.m_size = 0;
	object.m_capacity = 0;

	delete[] m_data;
	m_data = data;
	m_size = size;
	m_capacity = capacity;

	return *this;
}


// Size Functions

/*
*/
template <typename ValueType>
bool StringType<ValueType>::empty() const noexcept {
	return m_size == 0;
}

/*
*/
template <typename ValueType>
typename StringType<ValueType>::SizeType StringType<ValueType>::size() const noexcept {
	return m_size;
}


// Capacity Functions

/*
*/
template <typename ValueType>
typename StringType<ValueType>::SizeType StringType<ValueType>::capacity() const noexcept {
	return m_capacity;
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::reserve(SizeType size) {

	SizeType capacity = lookupCapacity(size);
	assume(size < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		std::copy(m_data, m_data + m_size, data);
		data[m_size] = NUL_TERMINATION;

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::shrink() {

	if (m_size == 0) {

		delete[] m_data;
		m_data = nullptr;
		m_size = 0;
		m_capacity = 0;

		return;
	}

	SizeType capacity = lookupCapacity(m_size);
	assume(m_size < capacity);

	if (m_capacity > capacity) {
		Pointer data = new ValueType[capacity];

		std::copy(m_data, m_data + m_size, data);
		data[m_size] = NUL_TERMINATION;

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::deallocate() noexcept {

	delete[] m_data;
	m_data = nullptr;
	m_size = 0;
	m_capacity = 0;
}


// Data Access Functions

/*
*/
template <typename ValueType>
typename StringType<ValueType>::ConstPointer StringType<ValueType>::data() const noexcept {
	return m_data;
}

/*
*/
template <typename ValueType>
typename StringType<ValueType>::ConstPointer StringType<ValueType>::cstring() const noexcept {
	return m_capacity > 0 ? m_data : std::addressof(NUL_TERMINATION);
}

/*
*/
template <typename ValueType>
typename StringType<ValueType>::ConstReference StringType<ValueType>::operator[](SizeType index) const noexcept {

	assert_assume(index < m_size);

	return m_data[index];
}

/*
*/
template <typename ValueType>
typename StringType<ValueType>::Reference StringType<ValueType>::operator[](SizeType index) noexcept {

	assert_assume(index < m_size);

	return m_data[index];
}

/*
*/
template <typename ValueType>
typename StringType<ValueType>::ConstReference StringType<ValueType>::front() const noexcept {

	assert_assume(m_size > 0);

	return m_data[0];
}

/*
*/
template <typename ValueType>
typename StringType<ValueType>::Reference StringType<ValueType>::front() noexcept {

	assert_assume(m_size > 0);

	return m_data[0];
}

/*
*/
template <typename ValueType>
typename StringType<ValueType>::ConstReference StringType<ValueType>::back() const noexcept {

	assert_assume(m_size > 0);

	return m_data[m_size - 1];
}

/*
*/
template <typename ValueType>
typename StringType<ValueType>::Reference StringType<ValueType>::back() noexcept {

	assert_assume(m_size > 0);

	return m_data[m_size - 1];
}

// Mutations Functions

/*
*/
template <typename ValueType>
void StringType<ValueType>::clear() noexcept {

	m_size = 0;
	if (m_capacity > 0) {
		m_data[0] = NUL_TERMINATION;
	}
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::popback(SizeType count) noexcept {

	assert_assume(count <= m_size);

	m_size -= count;
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::trim(SizeType count) noexcept {

	assert_assume(count <= m_size);

	std::copy(m_data + count, m_data + m_size, m_data);

	m_size -= count;
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::erase(SizeType index) noexcept {

	assert_assume(index < m_size);

	std::copy(m_data + index + 1, m_data + m_size, m_data + index);

	--m_size;
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::erase(SizeType first, SizeType last) noexcept {

	assert_assume(first < last);
	assert_assume(last <= m_size);

	std::copy(m_data + last, m_data + m_size, m_data + first);

	m_size -= last - first;
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::insert(ValueType character, SizeType index) {

	assert_assume(index < m_size);

	SizeType capacity = lookupCapacity(m_size + 1);
	assume(m_size + 1 < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		std::copy(m_data, m_data + index, data);
		std::copy(m_data + index, m_data + m_size, data + index + 1);
		data[index] = character;

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}
	else {
		std::copy_backward(m_data + index, m_data + m_size, m_data + m_size + 1);
		m_data[index] = character;
	}

	++m_size;
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::insert(ConstPointer cstring, SizeType index) {

	assert_assume(cstring != nullptr);
	assert_assume(index < m_size);

	SizeType size = cstringSize(cstring);

	if (size == 0) {
		return;
	}

	SizeType capacity = lookupCapacity(m_size + size);
	assume(m_size + size < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		std::copy(m_data, m_data + index, data);
		std::copy(m_data + index, m_data + m_size, data + index + size);
		std::copy(cstring, cstring + size, data + index);

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}
	else {
		std::copy_backward(m_data + index, m_data + m_size, m_data + m_size + size);
		std::copy(cstring, cstring + size, m_data + index);
	}

	m_size += size;
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::insert(const StringType &object, SizeType index) {

	assert_assume(index < m_size);

	if (object.m_size == 0) {
		return;
	}

	SizeType capacity = lookupCapacity(m_size + object.m_size);
	assume(m_size + object.m_size < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		std::copy(m_data, m_data + index, data);
		std::copy(m_data + index, m_data + m_size, data + index + object.m_size);
		std::copy(object.m_data, object.m_data + object.m_size, data + index);

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}
	else {
		std::copy_backward(m_data + index, m_data + m_size, m_data + m_size + object.m_size);
		std::copy(object.m_data, object.m_data + object.m_size, m_data + index);
	}

	m_size += object.m_size;
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
void StringType<ValueType>::insert(StringType &&object, SizeType index) {

	assert_assume(index < m_size);

	if (object.m_size == 0) {
		return;
	}

	SizeType capacity = lookupCapacity(m_size + object.m_size);
	assume(m_size + object.m_size < capacity);

	if (m_capacity < capacity) {

		if (object.m_capacity >= capacity) {

			std::copy_backward(object.m_data, object.m_data + object.m_size, object.m_data + object.m_size + index);
			std::copy(m_data, m_data + index, object.m_data);
			std::copy(m_data + index, m_data + m_size, object.m_data + object.m_size + index);

			delete[] m_data;
			m_data = object.m_data;
			m_size += object.m_size;
			m_capacity = object.m_capacity;

			object.m_data = nullptr;
			object.m_size = 0;
			object.m_capacity = 0;
		}
		else {
			Pointer data = new ValueType[capacity];

			std::copy(m_data, m_data + index, data);
			std::copy(m_data + index, m_data + m_size, data + index + object.m_size);
			std::copy(object.m_data, object.m_data + object.m_size, data + index);

			delete[] m_data;
			m_data = data;
			m_capacity = capacity;
		}
	}
	else {
		std::copy_backward(m_data + index, m_data + m_size, m_data + m_size + object.m_size);
		std::copy(object.m_data, object.m_data + object.m_size, m_data + index);
	}

	m_size += object.m_size;
	m_data[m_size] = NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
StringType<ValueType> &StringType<ValueType>::operator+=(ValueType character) {

	SizeType capacity = lookupCapacity(m_size + 1);
	assume(m_size + 1 < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		std::copy(m_data, m_data + m_size, data);

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}

	m_data[m_size] = character;

	++m_size;
	m_data[m_size] = NUL_TERMINATION;

	return *this;
}

/*
*/
template <typename ValueType>
StringType<ValueType> &StringType<ValueType>::operator+=(ConstPointer cstring) {

	assert_assume(cstring != nullptr);

	SizeType size = cstringSize(cstring);

	if (size == 0) {
		return *this;
	}

	SizeType capacity = lookupCapacity(m_size + size);
	assume(m_size + size < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		std::copy(m_data, m_data + m_size, data);

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}

	std::copy(cstring, cstring + size, m_data + m_size);

	m_size += size;
	m_data[m_size] = NUL_TERMINATION;

	return *this;
}

/*
*/
template <typename ValueType>
StringType<ValueType> &StringType<ValueType>::operator+=(const StringType &object) {

	if (object.m_size == 0) {
		return *this;
	}

	SizeType capacity = lookupCapacity(m_size + object.m_size);
	assume(m_size + object.m_size < capacity);

	if (m_capacity < capacity) {
		Pointer data = new ValueType[capacity];

		std::copy(m_data, m_data + m_size, data);

		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}

	std::copy(object.m_data, object.m_data + object.m_size, m_data + m_size);

	m_size += object.m_size;
	m_data[m_size] = NUL_TERMINATION;

	return *this;
}

/*
*/
template <typename ValueType>
StringType<ValueType> &StringType<ValueType>::operator+=(StringType &&object) {

	if (object.m_size == 0) {
		return *this;
	}

	SizeType capacity = lookupCapacity(m_size + object.m_size);
	assume(m_size + object.m_size < capacity);

	if (m_capacity < capacity) {

		if (object.m_capacity >= capacity) {

			std::copy_backward(object.m_data, object.m_data + object.m_size, object.m_data + object.m_size + m_size);
			std::copy(m_data, m_data + m_size, object.m_data);

			delete[] m_data;
			m_data = object.m_data;
			m_size += object.m_size;
			m_capacity = object.m_capacity;

			object.m_data = nullptr;
			object.m_size = 0;
			object.m_capacity = 0;
		}
		else {
			Pointer data = new ValueType[capacity];

			std::copy(m_data, m_data + m_size, data);

			delete[] m_data;
			m_data = data;
			m_capacity = capacity;
		}
	}

	std::copy(object.m_data, object.m_data + object.m_size, m_data + m_size);

	m_size += object.m_size;
	m_data[m_size] = NUL_TERMINATION;

	return *this;
}

/*
*/
template <typename ValueType>
StringType<ValueType> StringType<ValueType>::substring(SizeType last) const &{

	assert_assume(last <= m_size);

	SizeType size = last;
	SizeType capacity = lookupCapacity(size);
	assume(size < capacity);

	Pointer data = new ValueType[capacity];

	std::copy(m_data, m_data + last, data);
	data[size] = NUL_TERMINATION;

	return StringType{data, size, capacity};
}

/*
*/
template <typename ValueType>
StringType<ValueType> StringType<ValueType>::substring(SizeType first, SizeType last) const &{

	assert_assume(first < last);
	assert_assume(last <= m_size);

	SizeType size = last - first;
	SizeType capacity = lookupCapacity(size);
	assume(size < capacity);

	Pointer data = new ValueType[capacity];

	std::copy(m_data + first, m_data + last, data);
	data[size] = NUL_TERMINATION;

	return StringType{data, size, capacity};
}

/*
*/
template <typename ValueType>
StringType<ValueType> StringType<ValueType>::substring(SizeType last) && noexcept {

	assert_assume(last <= m_size);

	SizeType size = last;
	SizeType capacity = m_capacity;
	Pointer data = m_data;

	m_data = nullptr;
	m_size = 0;
	m_capacity = 0;

	data[size] = NUL_TERMINATION;

	return StringType{data, size, capacity};
}

/*
*/
template <typename ValueType>
StringType<ValueType> StringType<ValueType>::substring(SizeType first, SizeType last) && noexcept {

	assert_assume(first < last);
	assert_assume(last <= m_size);

	SizeType size = last - first;
	SizeType capacity = m_capacity;
	Pointer data = m_data;

	m_data = nullptr;
	m_size = 0;
	m_capacity = 0;

	std::copy(data + first, data + last, data);
	data[size] = NUL_TERMINATION;

	return StringType{data, size, capacity};
}

// Comparison Functions

/*
*/
template <typename ValueType>
int StringType<ValueType>::compare(ConstPointer cstring) const noexcept {

	assert_assume(cstring != nullptr);

	SizeType i = 0;
	for (; i < m_size; ++i) {

		if (cstring[i] == NUL_TERMINATION) {
			break;
		}
		else if (m_data[i] < cstring[i]) {
			return -1;
		}
		else if (m_data[i] > cstring[i]) {
			return 1;
		}
	}

	if (cstring[i] != NUL_TERMINATION) {
		return -1;
	}
	else if (m_size > i) {
		return 1;
	}
	else {
		return 0;
	}
}

/*
*/
template <typename ValueType>
int StringType<ValueType>::compare(const StringType &object) const noexcept {

	for (SizeType i = 0; i < m_size && i < object.m_size; ++i) {

		if (m_data[i] < object.m_data[i]) {
			return -1;
		}
		else if (m_data[i] > object.m_data[i]) {
			return 1;
		}
	}

	if (m_size < object.m_size) {
		return -1;
	}
	else if (m_size > object.m_size) {
		return 1;
	}
	else {
		return 0;
	}
}

// Mutation Operations

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(const StringType<ValueType> &left, const StringType<ValueType> &right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	if (left.m_size == 0 && right.m_size == 0) {
		return StringType<ValueType>{};
	}
	else if (left.m_size == 0) {
		return StringType<ValueType>{ right };
	}
	else if (right.m_size == 0) {
		return StringType<ValueType>{ left };
	}

	SizeType size = left.m_size + right.m_size;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = new ValueType[capacity];

	std::copy(left.m_data, left.m_data + left.m_size, data);
	std::copy(right.m_data, right.m_data + right.m_size, data + left.m_size);
	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(const StringType<ValueType> &left, typename StringType<ValueType>::ConstPointer right) {

	assert_assume(right != nullptr);

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	SizeType rightSize = StringType<ValueType>::cstringSize(right);

	if (left.m_size == 0 && rightSize == 0) {
		return StringType<ValueType>{};
	}
	else if (left.m_size == 0) {
		return StringType<ValueType>{ right };
	}
	else if (rightSize == 0) {
		return StringType<ValueType>{ left };
	}

	SizeType size = left.m_size + rightSize;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = new ValueType[capacity];

	std::copy(left.m_data, left.m_data + left.m_size, data);
	std::copy(right, right + rightSize, data + left.m_size);
	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(typename StringType<ValueType>::ConstPointer left, const StringType<ValueType> &right) {

	assert_assume(left != nullptr);

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	SizeType leftSize = StringType<ValueType>::cstringSize(left);

	if (leftSize == 0 && right.m_size == 0) {
		return StringType<ValueType>{};
	}
	else if (leftSize == 0) {
		return StringType<ValueType>{ right };
	}
	else if (right.m_size == 0) {
		return StringType<ValueType>{ left };
	}

	SizeType size = leftSize + right.m_size;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = new ValueType[capacity];

	std::copy(left, left + leftSize, data);
	std::copy(right.m_data, right.m_data + right.m_size, data + leftSize);
	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(const StringType<ValueType> &left, ValueType right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	if (left.m_size == 0) {
		return StringType<ValueType>{ right };
	}

	SizeType size = left.m_size + 1;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = new ValueType[capacity];

	std::copy(left.m_data, left.m_data + left.m_size, data);
	data[left.m_size] = right;
	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(ValueType left, const StringType<ValueType> &right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	if (right.m_size == 0) {
		return StringType<ValueType>{ left };
	}

	SizeType size = 1 + right.m_size;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = new ValueType[capacity];

	data[0] = left;
	std::copy(right.m_data, right.m_data + right.m_size, data + 1);
	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(StringType<ValueType> &&left, StringType<ValueType> &&right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	if (left.m_size == 0 && right.m_size == 0) {
		return StringType<ValueType>{};
	}
	else if (left.m_size == 0) {
		return StringType<ValueType>{ std::move(right) };
	}
	else if (right.m_size == 0) {
		return StringType<ValueType>{ std::move(left) };
	}

	SizeType size = left.m_size + right.m_size;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = nullptr;

	if (left.m_capacity >= capacity) {
		std::copy(right.m_data, right.m_data + right.m_size, left.m_data + left.m_size);

		data = left.m_data;
		capacity = left.m_capacity;

		left.m_data = nullptr;
		left.m_size = 0;
		left.m_capacity = 0;
	}
	else if (right.m_capacity >= capacity) {
		std::copy_backward(right.m_data, right.m_data + right.m_size, right.m_data + size);
		std::copy(left.m_data, left.m_data + left.m_size, right.m_data);

		data = right.m_data;
		capacity = right.m_capacity;

		right.m_data = nullptr;
		right.m_size = 0;
		right.m_capacity = 0;
	}
	else {
		data = new ValueType[capacity];

		std::copy(left.m_data, left.m_data + left.m_size, data);
		std::copy(right.m_data, right.m_data + right.m_size, data + left.m_size);
	}

	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(StringType<ValueType> &&left, const StringType<ValueType> &right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	if (left.m_size == 0 && right.m_size == 0) {
		return StringType<ValueType>{};
	}
	else if (left.m_size == 0) {
		return StringType<ValueType>{ right };
	}
	else if (right.m_size == 0) {
		return StringType<ValueType>{ std::move(left) };
	}

	SizeType size = left.m_size + right.m_size;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = nullptr;

	if (left.m_capacity >= capacity) {
		std::copy(right.m_data, right.m_data + right.m_size, left.m_data + left.m_size);

		data = left.m_data;
		capacity = left.m_capacity;

		left.m_data = nullptr;
		left.m_size = 0;
		left.m_capacity = 0;
	}
	else {
		data = new ValueType[capacity];

		std::copy(left.m_data, left.m_data + left.m_size, data);
		std::copy(right.m_data, right.m_data + right.m_size, data + left.m_size);
	}

	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(const StringType<ValueType> &left, StringType<ValueType> &&right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	if (left.m_size == 0 && right.m_size == 0) {
		return StringType<ValueType>{};
	}
	else if (left.m_size == 0) {
		return StringType<ValueType>{ std::move(right) };
	}
	else if (right.m_size == 0) {
		return StringType<ValueType>{ left };
	}

	SizeType size = left.m_size + right.m_size;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = nullptr;

	if (right.m_capacity >= capacity) {
		std::copy_backward(right.m_data, right.m_data + right.m_size, right.m_data + size);
		std::copy(left.m_data, left.m_data + left.m_size, right.m_data);

		data = right.m_data;
		capacity = right.m_capacity;

		right.m_data = nullptr;
		right.m_size = 0;
		right.m_capacity = 0;
	}
	else {
		data = new ValueType[capacity];

		std::copy(left.m_data, left.m_data + left.m_size, data);
		std::copy(right.m_data, right.m_data + right.m_size, data + left.m_size);
	}

	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(StringType<ValueType> &&left, typename StringType<ValueType>::ConstPointer right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	assert_assume(right != nullptr);

	SizeType rightSize = StringType<ValueType>::cstringSize(right);

	if (left.m_size == 0 && rightSize == 0) {
		return StringType<ValueType>{};
	}
	else if (left.m_size == 0) {
		return StringType<ValueType>{ right };
	}
	else if (rightSize == 0) {
		return StringType<ValueType>{ std::move(left) };
	}

	SizeType size = left.m_size + rightSize;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = nullptr;

	if (left.m_capacity >= capacity) {
		std::copy(right, right + rightSize, left.m_data + left.m_size);

		data = left.m_data;
		capacity = left.m_capacity;

		left.m_data = nullptr;
		left.m_size = 0;
		left.m_capacity = 0;
	}
	else {
		data = new ValueType[capacity];

		std::copy(left.m_data, left.m_data + left.m_size, data);
		std::copy(right, right + rightSize, data + left.m_size);
	}

	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(typename StringType<ValueType>::ConstPointer left, StringType<ValueType> &&right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	assert_assume(left != nullptr);

	SizeType leftSize = StringType<ValueType>::cstringSize(left);

	if (leftSize == 0 && right.m_size == 0) {
		return StringType<ValueType>{};
	}
	else if (leftSize == 0) {
		return StringType<ValueType>{ std::move(right) };
	}
	else if (right.m_size == 0) {
		return StringType<ValueType>{ left };
	}

	SizeType size = leftSize + right.m_size;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = nullptr;

	if (right.m_capacity >= capacity) {
		std::copy_backward(right.m_data, right.m_data + right.m_size, right.m_data + size);
		std::copy(left, left + leftSize, right.m_data);

		data = right.m_data;
		capacity = right.m_capacity;

		right.m_data = nullptr;
		right.m_size = 0;
		right.m_capacity = 0;
	}
	else {
		data = new ValueType[capacity];

		std::copy(left, left + leftSize, data);
		std::copy(right.m_data, right.m_data + right.m_size, data + leftSize);
	}

	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(StringType<ValueType> &&left, ValueType right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	if (left.m_size == 0) {
		return StringType<ValueType>{ right };
	}

	SizeType size = left.m_size + 1;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = nullptr;

	if (left.m_capacity >= capacity) {
		left.m_data[left.m_size] = right;

		data = left.m_data;
		capacity = left.m_capacity;

		left.m_data = nullptr;
		left.m_size = 0;
		left.m_capacity = 0;
	}
	else {
		data = new ValueType[capacity];

		std::copy(left.m_data, left.m_data + left.m_size, data);
		data[left.m_size] = right;
	}

	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

/*
*/
template <typename ValueType>
StringType<ValueType> operator+(ValueType left, StringType<ValueType> &&right) {

	using SizeType = typename StringType<ValueType>::SizeType;
	using Pointer = typename StringType<ValueType>::Pointer;

	if (right.m_size == 0) {
		return StringType<ValueType>{ left };
	}

	SizeType size = 1 + right.m_size;
	SizeType capacity = StringType<ValueType>::lookupCapacity(size);
	assume(size < capacity);
	Pointer data = nullptr;

	if (right.m_capacity >= capacity) {
		std::copy_backward(right.m_data, right.m_data + right.m_size, right.m_data + size);
		right.m_data[0] = left;

		data = right.m_data;
		capacity = right.m_capacity;

		right.m_data = nullptr;
		right.m_size = 0;
		right.m_capacity = 0;
	}
	else {
		data = new ValueType[capacity];

		data[0] = left;
		std::copy(right.m_data, right.m_data + right.m_size, data + 1);
	}

	data[size] = StringType<ValueType>::NUL_TERMINATION;

	return StringType<ValueType>{ data, size, capacity };
}

// Comparison Operations

/*
*/
template <typename ValueType>
bool operator==(const StringType<ValueType> &left, const StringType<ValueType> &right) noexcept {

	using SizeType = typename StringType<ValueType>::SizeType;

	if (left.m_size != right.m_size) {
		return false;
	}

	if (left.m_size == 0) {
		return true;
	}

	for (SizeType i = 0; i < left.m_size; ++i) {

		if (left.m_data[i] != right.m_data[i]) {
			return false;
		}
	}

	return true;
}

/*
*/
template <typename ValueType>
bool operator==(const StringType<ValueType> &left, typename StringType<ValueType>::ConstPointer right) noexcept {

	using SizeType = typename StringType<ValueType>::SizeType;

	assert_assume(right != nullptr);

	for (SizeType i = 0; i < left.m_size; ++i) {

		if (right[i] == StringType<ValueType>::NUL_TERMINATION || left.m_data[i] != right[i]) {
			return false;
		}
	}

	return right[left.m_size] == StringType<ValueType>::NUL_TERMINATION;
}

/*
*/
template <typename ValueType>
bool operator==(typename StringType<ValueType>::ConstPointer left, const StringType<ValueType> &right) noexcept {
	return right == left;
}

/*
*/
template <typename ValueType>
bool operator!=(const StringType<ValueType> &left, const StringType<ValueType> &right) noexcept {
	return !(left == right);
}

/*
*/
template <typename ValueType>
bool operator!=(const StringType<ValueType> &left, typename StringType<ValueType>::ConstPointer right) noexcept {
	return !(left == right);
}

/*
*/
template <typename ValueType>
bool operator!=(typename StringType<ValueType>::ConstPointer left, const StringType<ValueType> &right) noexcept {
	return !(left == right);
}

// Output Stream Operations

/*
*/
template <typename ValueType>
std::ostream &operator<<(std::ostream &os, const StringType<ValueType> &object) {

	using SizeType = typename StringType<ValueType>::SizeType;

	for (SizeType i = 0; i < object.m_size; ++i) {
		os << object.m_data[i];
	}

	return os;
}


// Default Alias

using String = StringType<char>;

}


#undef assume
#undef assert_assume

#endif // SIMPLE_STRING_HPP
