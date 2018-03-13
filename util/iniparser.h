#pragma once
#include <string>
#include <utility>
#include <map>
#include "STLHelper.h"

class CIniParser : public IEnumerableProxy<std::map<std::string, std::map<std::string, std::string>>>
{
public:
	CIniParser() :IEnumerableProxy(m_DataMap){}
	CIniParser(const std::string &filename) :IEnumerableProxy(m_DataMap)
	{
		OpenFile(filename);
	}
	void OpenFile2(const std::string &filename, size_t iBufferSize = 4096);
	void OpenFile(const std::string &filename);
	void CloseFile()
	{
		m_DataMap.clear();
	}

	using AppNameType = std::string;
	using KeyType = std::string;
	using ValueType = std::string;
	using DataMapType = std::map<AppNameType, std::map<KeyType, ValueType>>;
	using iterator = DataMapType::iterator;

	auto operator[](const std::string &szAppName) -> std::map<KeyType, ValueType> &
	{
		return m_DataMap[szAppName];
	}
	operator bool() const
	{
		return m_DataMap.empty();
	}

private:
	DataMapType m_DataMap;
};