#include "metahook.h"
#include "plugins.h"
#include "configs.h"
#include "BaseUI.h"

#include <SourceSDK\tier1\strtools.h>

void Sys_GetRegKeyValueUnderRoot(const char *pszSubKey, const char *pszElement, char *pszReturnString, int nReturnLength, const char *pszDefaultValue)
{
	LONG lResult;
	HKEY hKey;
	char szBuff[128];
	DWORD dwDisposition;
	DWORD dwType;
	DWORD dwSize;

	sprintf(pszReturnString, "%s", pszDefaultValue);
	lResult = RegCreateKeyExA(HKEY_CURRENT_USER, pszSubKey, 0, "String", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

	if (lResult != ERROR_SUCCESS)
		return;

	if (dwDisposition == REG_CREATED_NEW_KEY)
	{
		RegSetValueEx(hKey, pszElement, 0, REG_SZ, (CONST BYTE *)pszDefaultValue, strlen(pszDefaultValue) + 1);
	}
	else
	{
		dwSize = nReturnLength;
		lResult = RegQueryValueEx(hKey, pszElement, 0, &dwType, (unsigned char *)szBuff, &dwSize);

		if (lResult == ERROR_SUCCESS)
		{
			if (dwType == REG_SZ)
			{
				strncpy(pszReturnString, szBuff, nReturnLength);
				pszReturnString[nReturnLength - 1] = '\0';
			}
		}
		else
			lResult = RegSetValueEx(hKey, pszElement, 0, REG_SZ, (CONST BYTE *)pszDefaultValue, strlen(pszDefaultValue) + 1);
	}

	RegCloseKey(hKey);
}

void Sys_SetRegKeyValueUnderRoot(const char *pszSubKey, const char *pszElement, const char *pszValue)
{
	LONG lResult;
	HKEY hKey;
	DWORD dwDisposition;

	lResult = RegCreateKeyExA(HKEY_CURRENT_USER, pszSubKey, 0, "String", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

	if (lResult != ERROR_SUCCESS)
		return;

	RegSetValueEx(hKey, pszElement, 0, REG_SZ, (CONST BYTE *)pszValue, strlen(pszValue) + 1);
	RegCloseKey(hKey);
}

void Sys_ErrorEx(const char *error)
{
	if (g_pMetaSave->pEngineFuncs)
		g_pMetaSave->pEngineFuncs->pfnClientCmd("escape\n");

	MessageBoxA(NULL, error, (!strcmp(gConfigs.szLanguage, "schinese")) ? "错误" : "Error", MB_ICONERROR);
	exit(0);
}

inline bool IsSlash(char c)
{
	return (c == '\\') || (c == '/');
}

void Sys_mkdir(const char *path)
{
	char testpath[MAX_PATH];
	Q_strncpy(testpath, path, sizeof(testpath));

	int nLen = Q_strlen(testpath);

	if ((nLen > 0) && IsSlash(testpath[nLen - 1]))
		testpath[nLen - 1] = 0;

	const char *pPathID = "GAME";

	if (IsSlash(testpath[0]) && IsSlash(testpath[1]))
		pPathID = NULL;

	if (g_pFileSystem->FileExists(testpath))
	{
		if (!g_pFileSystem->IsDirectory(testpath))
			g_pFileSystem->RemoveFile(testpath, pPathID);
	}

	g_pFileSystem->CreateDirHierarchy(path, pPathID);
}