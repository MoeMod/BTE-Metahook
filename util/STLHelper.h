#pragma once

#include <string>
#include <algorithm>

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