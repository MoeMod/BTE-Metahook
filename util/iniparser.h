#pragma once
#include <string>
#include <utility>
#include <unordered_map>

class CIniParser
{
public:
	CIniParser(){}
	CIniParser(const std::string &filename, size_t iBufferSize = 4096);

	using AppNameType = std::string;
	using KeyType = std::string;
	using ValueType = std::string;
	using DataMapType = std::unordered_map<AppNameType, std::unordered_map<KeyType, ValueType>>;
	using iterator = DataMapType::iterator;

	auto operator[](const std::string &szAppName) -> std::unordered_map<KeyType, ValueType> &
	{
		return m_DataMap[szAppName];
	}
	auto begin() -> DataMapType::iterator
	{
		return m_DataMap.begin();
	}
	auto end() -> DataMapType::iterator
	{
		return m_DataMap.end();
	}
	operator bool() const
	{
		return m_DataMap.empty();
	}

private:
	DataMapType m_DataMap;
};