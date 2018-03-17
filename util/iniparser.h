#pragma once
#include <string>
#include <utility>
#include <map>
#include <unordered_map>
#include <memory>

#include <fstream>
#include <sstream>
#include <iterator>
#include <Interface/IFileSystem.h>
#include "STLHelper.h"

template<class MapType>
class CBasicIniParser : public IEnumerableProxy<MapType>
{
public:
	CBasicIniParser() :IEnumerableProxy(m_DataMap){}
	CBasicIniParser(const std::string &filename) :IEnumerableProxy(m_DataMap)
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

	using iterator = typename MapType::iterator;
	using KeyListType = typename Template_GetArg<1, MapType>::type;
	auto operator[](const std::string &szAppName) -> KeyListType &
	{
		return m_DataMap[szAppName];
	}
	operator bool() const
	{
		return m_DataMap.empty();
	}
	size_t size() const
	{
		return m_DataMap.size();
	}

private:
	MapType m_DataMap;
	std::unique_ptr<char[]> m_pszConfigPath;
};

template<class MapType>
void CBasicIniParser<MapType>::OpenFile(const std::string &filename)
{
	CloseFile();
	if (!m_pszConfigPath)
		m_pszConfigPath = std::make_unique<char[]>(128);
	g_pFileSystem->GetLocalPath(filename.c_str(), m_pszConfigPath.get(), 127);

	std::ifstream fs(m_pszConfigPath.get());
	std::string line, strAppName;
	KeyListType KeyList;

	while (!std::getline(fs, line, '\n').eof())
	{
		if (line.empty())
			continue;
		if (line.front() == '[' && line.back() == ']')
		{
			// insert prev app
			if (!strAppName.empty())
				m_DataMap.emplace(std::move(strAppName), KeyList);
			// copy new appname
			KeyList.clear();
			strAppName = line.substr(1, line.size() - 2);
		}
		else if (line.front() != ';')
		{
			std::string::size_type n = line.find('=');
			if (n != std::string::npos)
			{
				std::string &&left = line.substr(0, n);
				std::string &&right = line.substr(n + 1);
				KeyList.emplace(Trim(left), Trim(right));
			}
		}
	}
}

template<class MapType>
void CBasicIniParser<MapType>::SaveFile() const
{
	std::ofstream fs(m_pszConfigPath.get());
	for (auto &app : m_DataMap)
	{
		fs << '[' << app.first << ']' << std::endl;
		for (auto &kv : app.second)
		{
			fs << kv.first << " = " << kv.second << std::endl;
		}
		fs << std::endl;
	}
}

using CIniParser = CBasicIniParser<std::unordered_map<std::string, std::unordered_map<std::string, std::string>>>;

