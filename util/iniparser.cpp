#include <metahook.h>
#include <plugins.h>
#include <Interface/IFileSystem.h>

#include "iniparser.h"
#include <Windows.h>
#include <memory>

#include <fstream>
#include <sstream>
#include <iterator>

void CIniParser::OpenFile2(const std::string &filename, size_t iBufferSize)
{
	CloseFile();
	//char strAppNameTemp[4096], strKeyNameTemp[4096];
	auto pAppNameTemp = std::make_unique<TCHAR[]>(iBufferSize);
	auto pKeyNameTemp = std::make_unique<TCHAR[]>(iBufferSize);
	char szConfigPath[256];
	g_pFileSystem->GetLocalPath(filename.c_str(), szConfigPath, sizeof(szConfigPath));

	DWORD dwAppNameSize = GetPrivateProfileString(nullptr, nullptr, nullptr, pAppNameTemp.get(), iBufferSize, szConfigPath);
	if (dwAppNameSize > 0)
	{
		//TCHAR *pAppName = new char[dwAppNameSize];
		auto pAppName = std::make_unique<TCHAR[]>(dwAppNameSize);
		int nAppNameLen = 0;
		for (DWORD i = 0; i < dwAppNameSize; i++)
		{
			pAppName[nAppNameLen++] = pAppNameTemp[i];
			if (!pAppNameTemp[i])
			{
				std::string strAppName(pAppName.get());
				DWORD dwKeyNameSize = GetPrivateProfileString(pAppName.get(), nullptr, nullptr, pKeyNameTemp.get(), iBufferSize, szConfigPath);
				std::map<std::string, std::string> KeyList;
				if (dwKeyNameSize > 0)
				{
					auto pKeyName = std::make_unique<TCHAR[]>(dwKeyNameSize);
					int nKeyNameLen = 0;
					for (DWORD j = 0; j < dwKeyNameSize; j++)
					{
						pKeyName[nKeyNameLen++] = pKeyNameTemp[j];
						if (!pKeyNameTemp[j])
						{
							char pValue[128]; // pKeyName = pValue
							if (GetPrivateProfileString(pAppName.get(), pKeyName.get(), nullptr, pValue, 127, szConfigPath))
							{
								std::string strKey(pKeyName.get());
								std::string strValue(pValue);

								KeyList[strKey] = strValue;
							}
							pKeyName[0] = '\0';
							nKeyNameLen = 0;
						}
					}
					m_DataMap[strAppName] = std::move(KeyList);
				}
				nAppNameLen = 0;
			}
		}
	}
}

void CIniParser::OpenFile(const std::string &filename)
{
	static char szConfigPath[256];
	g_pFileSystem->GetLocalPath(filename.c_str(), szConfigPath, sizeof(szConfigPath));

	std::ifstream fs(szConfigPath);
	std::string line, strAppName;
	std::map<std::string, std::string> KeyList;

	while (!std::getline(fs, line, '\n').eof())
	{
		if (line.empty())
			continue;
		if (line.front() == '[' && line.back() == ']')
		{
			// insert prev app
			if (!strAppName.empty())
				m_DataMap.emplace(std::move(strAppName), std::move(KeyList));
			// copy new appname
			strAppName.clear();
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