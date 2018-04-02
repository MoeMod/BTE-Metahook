#pragma once

#include <string>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <iterator>

inline std::string ToLower(std::string sz)
{
	std::transform(sz.cbegin(), sz.cend(), sz.begin(), ::tolower);
	return sz;
}

inline std::string ToUpper(std::string sz)
{
	std::transform(sz.cbegin(), sz.cend(), sz.begin(), ::toupper);
	return sz;
}

inline std::string Trim(std::string sz)
{
	const char *delim = " \t";
	sz.erase(0, sz.find_first_not_of(delim));
	sz.erase(sz.find_last_not_of(delim) + 1);
	return sz;
}

template<class T>
class IEnumerableProxy
{
public:
	IEnumerableProxy(T &x) : m_Iterable(x){}

	auto begin() -> decltype(std::declval<T>().begin()) { return m_Iterable.begin();}
	auto end() -> decltype(std::declval<T>().end()) { return m_Iterable.end(); }
	auto rbegin() -> decltype(std::declval<T>().rbegin()) { return m_Iterable.rbegin(); }
	auto rend() -> decltype(std::declval<T>().rend()) { return m_Iterable.rend(); }
	auto cbegin() -> decltype(std::declval<T>().cbegin()) { return m_Iterable.cbegin(); }
	auto cend() -> decltype(std::declval<T>().cend()) { return m_Iterable.cend(); }
	auto crbegin() -> decltype(std::declval<T>().crbegin()) { return m_Iterable.crbegin(); }
	auto crend() -> decltype(std::declval<T>().crend()) { return m_Iterable.crend(); }
	
private:
	T &m_Iterable;
};

template<size_t n, class First, class...Args>
struct VA_GetArg
{
	using type = typename VA_GetArg<n - 1, Args...>::type;
};
template<class First, class...Args>
struct VA_GetArg<0, First, Args...>
{
	using type = First;
};
template<size_t n, class T>
struct Template_GetArg{};
template<template<class...> class T, size_t n, class...Args>
struct Template_GetArg<n, T<Args...>>
{
	using type = typename VA_GetArg<n, Args...>::type;
};

template<size_t n, class Tuple>
inline bool IsKeyEqualsToTupleElement(const std::string &sz, const Tuple &pair)
{
	return sz == std::get<n>(pair);
}

struct range_iterator
{
	int n, step, end;
	using difference_type = int;
	using iterator_category = std::input_iterator_tag;
	using pointer = range_iterator;
	using reference = int &;
	using value_type = int;
	range_iterator &operator++() { return  n += step, *this; }
	bool operator!=(range_iterator &other) { return is_end() != other.is_end() && n != other.n; }
	int operator*() { return n; }
	bool is_end() { return n >= end; }
};

struct range_iterator_one
{
	int n;
	using difference_type = int;
	using iterator_category = std::input_iterator_tag;
	using pointer = range_iterator_one;
	using reference = int &;
	using value_type = int;
	range_iterator_one &operator++() { return  ++n, *this; }
	bool operator!=(range_iterator_one &other) { return n != other.n; }
	int operator*() { return n; }
};

struct range_generator
{
	int start, stop, step;
	range_iterator begin() { return { start, step, stop }; }
	range_iterator end() { return { stop, step, stop }; }
};

struct range_generator_one
{
	int start, stop;
	range_iterator_one begin() { return { start }; }
	range_iterator_one end() { return { stop }; }
};

inline range_generator xrange(int start, int stop, int step)
{
	return { start ,stop , step };
}

inline range_generator_one xrange(int start, int stop)
{
	return { start ,stop };
}

template<class...Args>
inline std::vector<int> range(Args&&...args)
{
	auto &&ran = xrange(args...);
	return std::vector<int>(ran.begin(), ran.end());
}