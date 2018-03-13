#include <metahook.h>
#include <plugins.h>
#include <Interface/IFileSystem.h>

#include "iniparser.h"
#include <memory>

#include <fstream>
#include <sstream>
#include <iterator>

void CIniParser::OpenFile(const std::string &filename)
{
	if (m_pszConfigPath)
		m_pszConfigPath = std::make_unique<char[]>(128);
	g_pFileSystem->GetLocalPath(filename.c_str(), m_pszConfigPath.get(), 127);
	std::ifstream fs(m_pszConfigPath.get());
	std::string line, strAppName;
	std::unordered_map<std::string, std::string> KeyList;

	while (!std::getline(fs, line, '\n').eof())
	{
		if (line.empty())
			continue;
		if (line.front() == '[' && line.back() == ']')
		{
			// insert prev app
			m_DataMap.emplace(std::move(strAppName), std::move(KeyList));
			// copy new appname
			KeyList.clear();
			strAppName = line.substr(1, line.size() - 2);
		}
		else if (line.front() != ';')
		{
			std::string::size_type n = line.find('=');
			if (n != std::string::npos)
			{
				KeyList.emplace(line.substr(0, n), line.substr(n + 1));
			}
		}
	}
}

void CIniParser::SaveFile() const
{
	std::ofstream fs(m_pszConfigPath.get());
	for (auto &app : m_DataMap)
	{
		fs << '[' << app.first << ']' << std::endl;
		for (auto &kv : app.second)
		{
			fs << kv.first << " = " << kv.second << std::endl;
		}
	}
}