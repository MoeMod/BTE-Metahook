#include <metahook.h>
#include <plugins.h>
#include <Interface/IFileSystem.h>

#include "iniparser.h"
#include <Windows.h>
#include <memory>

CIniParser::CIniParser(const std::string &filename, size_t iBufferSize)
{
	//char strAppNameTemp[4096], strKeyNameTemp[4096];
	auto pAppNameTemp = std::make_unique<TCHAR[]>(iBufferSize);
	auto pKeyNameTemp = std::make_unique<TCHAR[]>(iBufferSize);
	char szConfigPath[256];
	g_pFileSystem->GetLocalPath("textures.ini", szConfigPath, sizeof(szConfigPath));

	DWORD dwAppNameSize = GetPrivateProfileString(nullptr, nullptr, nullptr, pAppNameTemp.get(), iBufferSize, szConfigPath);
	DWORD dwKeyNameSize;
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
				dwKeyNameSize = GetPrivateProfileString(pAppName.get(), nullptr, nullptr, pKeyNameTemp.get(), iBufferSize, szConfigPath);
				std::unordered_map<std::string, std::string> KeyList;
				if (dwKeyNameSize > 0)
				{
					//TCHAR *pKeyName = new TCHAR[dwKeyNameSize];
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
					//delete[] pKeyName;
					m_DataMap[strAppName] = KeyList;
				}

				//pAppName[0] = '\0';
				nAppNameLen = 0;
			}
		}
		//delete[] pAppName;
	}
}