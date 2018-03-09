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