#pragma once
#include <string>
#include <utility>
#include <unordered_map>
#include <memory>
#include "STLHelper.h"

class CIniParser : public IEnumerableProxy<std::unordered_map<std::string, std::unordered_map<std::string, std::string>>>
{
public:
	CIniParser() :IEnumerableProxy(m_DataMap){}
	CIniParser(const std::string &filename) :IEnumerableProxy(m_DataMap)
	{
		OpenFile(filename);
	}
	void OpenFile(const std::string &filename);
	void CloseFile()
	{
		m_DataMap.clear();
		m_pszConfigPath.reset();
	}
	void SaveFile() const;

	using AppNameType = std::string;
	using KeyType = std::string;
	using ValueType = std::string;
	using DataMapType = std::unordered_map<AppNameType, std::unordered_map<KeyType, ValueType>>;
	using iterator = DataMapType::iterator;

	auto operator[](const std::string &szAppName) -> std::unordered_map<KeyType, ValueType> &
	{
		return m_DataMap[szAppName];
	}
	operator bool() const
	{
		return m_DataMap.empty();
	}

private:
	DataMapType m_DataMap;
	std::unique_ptr<char[]> m_pszConfigPath;
};