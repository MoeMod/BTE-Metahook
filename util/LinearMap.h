#pragma once

#include <vector>
#include <functional>
#include "STLHelper.h"

template<
	class Key,
	class T,
	class Allocator = std::allocator<std::pair<Key, T>>
> class LinearMap : public std::vector<std::pair<Key, T>, Allocator>
{
	
public:
	using key_type = Key;
	using mapped_type = T;

	iterator find(const Key& key)
	{
		return std::find_if(begin(), end(), std::bind(IsKeyEqualsToTupleElement<0, value_type>, std::cref(key), std::placeholders::_1));
	}
	const_iterator find(const Key& key) const
	{
		return std::find_if(cbegin(), cend(), std::bind(IsKeyEqualsToTupleElement<0, value_type>, std::cref(key), std::placeholders::_1));
	}

	T& operator[](const Key& key)
	{
		auto iter = find(key);
		if (iter == end())
		{
			push_back(value_type(key, mapped_type()));
			return back().second;
		}
		return iter->second;
	}

	template< class... Args >
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		auto iter = find(std::get<0>(std::tie(std::forward<Args>(args)...)));
		if (iter != this->end())
		{
			return std::make_pair(iter, false);
		}
		push_back({ std::forward<Args>(args)... });
		iter = std::prev(this->end());
		return std::make_pair(iter, true);
	}
};