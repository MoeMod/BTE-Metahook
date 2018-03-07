#include "base.h"
#include "baseui.h"
#include "msghook.h"
#include "util.h"

char g_szLang_Left[512][256];
char g_szLang_Right[512][256];
int g_szLang_Index = 0;

void GetRegKeyValueUnderRoot(const char *pszSubKey, const char *pszElement, char *pszReturnString, int nReturnLength, const char *pszDefaultValue);

char *GetLangUtf(char *pLeft)
{
	/*for(int i=0;i<g_szLang_Index;i++)
	{
		if(!strcmp(g_szLang_Left[i],pLeft))
		{
			return g_szLang_Right[i];
		}
	}
	return "Entry Point not found";*/
	//char a[128];
	//sprintf(a,"%s",vgui::localize()->Find(pLeft));
	wchar_t *pbuf = vgui::localize()->Find(pLeft);

	if (pbuf)
		return UnicodeToUTF8(pbuf);
	else
		return pLeft;


	byte* a = (byte *)malloc(100);

}
wchar_t *GetLangUni(char *pLeft)
{
	wchar_t *pbuf = vgui::localize()->Find(pLeft);

	if (pbuf)
		return pbuf;
	else
		return ANSIToUnicode(pLeft);
}

wchar_t *GetLangUni2(char *pLeft)
{
	wchar_t *pbuf = vgui::localize()->Find(pLeft);

	if (pbuf)
		return pbuf;
	else
		return UTF8ToUnicode(pLeft);
}

void ClientPrint(int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4)
{
	MSG_BeginWrite("TextMsg");
	MSG_WriteByte(msg_dest);
	MSG_WriteString(msg_name);

	if (param1)
		MSG_WriteString(param1);

	if (param2)
		MSG_WriteString(param2);

	if (param3)
		MSG_WriteString(param3);

	if (param4)
		MSG_WriteString(param4);

	MSG_EndWrite();
}
wchar_t *ANSIToUnicode( const char* str )
{
   int textlen ;
   static wchar_t result[1024];
   textlen = MultiByteToWideChar( CP_ACP, 0, str,-1, NULL,0 ); 
   memset(result, 0, sizeof(char) * ( textlen + 1 ) );
   MultiByteToWideChar(CP_ACP, 0,str,-1,(LPWSTR)result,textlen ); 
   return result; 
}
char *UnicodeToANSI( const wchar_t* str )
{
     static char result[1024];
     int textlen;
     textlen = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL );
     memset(result, 0, sizeof(char) * ( textlen + 1 ) );
     WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL );
     return result;
}
wchar_t *UTF8ToUnicode( const char* str )
{
     int textlen ;
     static wchar_t result[1024];
     textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 ); 
     memset(result, 0, sizeof(char) * ( textlen + 1 ) );
     MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen ); 
     return result; 
}
char *UnicodeToUTF8( const wchar_t* str )
{
	static char result[1024];
	int textlen;
	textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
	return result;
}
const char *FileExtension( const char *in )
{
	const char *separator, *backslash, *colon, *dot;

	separator = strrchr( in, '/' );
	backslash = strrchr( in, '\\' );
	if( !separator || separator < backslash ) separator = backslash;
	colon = strrchr( in, ':' );
	if( !separator || separator < colon ) separator = colon;

	dot = strrchr( in, '.' );
	if( dot == NULL || ( separator && ( dot < separator )))
		return "";
	return dot + 1;
}

int FileExist(const char *szFile)
{
	DWORD dAttrib = GetFileAttributes(szFile);

	if (dAttrib == INVALID_FILE_ATTRIBUTES || dAttrib == FILE_ATTRIBUTE_DIRECTORY)
		return 0;

	return 1;
}

int LogToFile(char *szLogText, ...)
{
	FILE *fp;

	if (!(fp = fopen("metahook\\logs\\metahook.log", "a")))
		return 0;
	
	va_list vArgptr;
	char szText[1024];

	va_start(vArgptr, szLogText);
	vsprintf(szText, szLogText, vArgptr);
	va_end(vArgptr);

	SYSTEMTIME systime;
	::GetLocalTime(&systime);


	fprintf(fp, "[%02d:%02d:%02d.%03d] %s\n", systime.wHour,systime.wMinute,systime.wSecond,systime.wMilliseconds,szText);
	fclose(fp);
	return 1;
}

void *InterceptDLLCall(HMODULE hModule, char *szDLLName, char *szFunctionName, DWORD pNewFunction)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	DWORD dwOldProtect, dwOldProtect2;
	void *pOldFunction;

	#define MakePtr(cast, ptr, addValue)(cast)((DWORD)(ptr) + (DWORD)(addValue))

	if (!(pOldFunction = GetProcAddress(GetModuleHandle(szDLLName), szFunctionName)))
		return 0;

	pDosHeader = (PIMAGE_DOS_HEADER)hModule;

	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDosHeader, pDosHeader->e_lfanew);

	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE || (pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, pDosHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)) == (PIMAGE_IMPORT_DESCRIPTOR)pNTHeader)
		return NULL;

	while (pImportDesc->Name)
	{
		if (!stricmp(MakePtr(char *, pDosHeader, pImportDesc->Name), szDLLName))
			break;

		pImportDesc++;
	}

	if (pImportDesc->Name == NULL)
		return NULL;

	pThunk = MakePtr(PIMAGE_THUNK_DATA, pDosHeader, pImportDesc->FirstThunk);

	while (pThunk->u1.Function)
	{
		if (pThunk->u1.Function == (DWORD)pOldFunction)
		{
			VirtualProtect((void *)&pThunk->u1.Function, sizeof(PDWORD), PAGE_EXECUTE_READWRITE, &dwOldProtect);
			pThunk->u1.Function=(DWORD)pNewFunction;
			VirtualProtect((void *)&pThunk->u1.Function, sizeof(PDWORD), dwOldProtect, &dwOldProtect2);
			return pOldFunction;
		}

		pThunk++;
	}

	return NULL;
}

DWORD FindPattern(char *szPattern, int iLen, DWORD dwStart, DWORD dwLen)
{ 
	char *szStart = (char *)dwStart; 
	for(; (DWORD)szStart < (dwStart + dwLen); szStart++) 
		if(!memcmp(szStart, szPattern, iLen)) 
			return (DWORD)szStart; 

	return NULL; 
}

client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount)
{
	if (!pList)
		return NULL;

	int i = iCount;
	client_sprite_t *p = pList;

	while(i--)
	{
		if ((!strcmp(psz, p->szName)) && (p->iRes == iRes))
			return p;
		p++;
	}

	return NULL;
}

void UnpackRGB(int &r, int &g, int &b, unsigned long ulRGB)
{
	r = (ulRGB & 0xFF0000) >>16;
	g = (ulRGB & 0xFF00) >> 8;
	b = ulRGB & 0xFF;
}

void ScaleColors(int &r, int &g, int &b, int a)
{
	float x = (float)a / 255;
	r = (int)(r * x);
	g = (int)(g * x);
	b = (int)(b * x);
}

void GetRegKeyValueUnderRoot(const char *pszSubKey, const char *pszElement, char *pszReturnString, int nReturnLength, const char *pszDefaultValue)
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
			RegSetValueEx(hKey, pszElement, 0, REG_SZ, (CONST BYTE *)pszDefaultValue, strlen(pszDefaultValue) + 1);
	}

	RegCloseKey(hKey);
}

static unsigned int glSeed = 0;

unsigned int seed_table[256] =
{
	28985, 27138, 26457, 9451, 17764, 10909, 28790, 8716, 6361, 4853, 17798, 21977, 19643, 20662, 10834, 20103,
	27067, 28634, 18623, 25849, 8576, 26234, 23887, 18228, 32587, 4836, 3306, 1811, 3035, 24559, 18399, 315,
	26766, 907, 24102, 12370, 9674, 2972, 10472, 16492, 22683, 11529, 27968, 30406, 13213, 2319, 23620, 16823,
	10013, 23772, 21567, 1251, 19579, 20313, 18241, 30130, 8402, 20807, 27354, 7169, 21211, 17293, 5410, 19223,
	10255, 22480, 27388, 9946, 15628, 24389, 17308, 2370, 9530, 31683, 25927, 23567, 11694, 26397, 32602, 15031,
	18255, 17582, 1422, 28835, 23607, 12597, 20602, 10138, 5212, 1252, 10074, 23166, 19823, 31667, 5902, 24630,
	18948, 14330, 14950, 8939, 23540, 21311, 22428, 22391, 3583, 29004, 30498, 18714, 4278, 2437, 22430, 3439,
	28313, 23161, 25396, 13471, 19324, 15287, 2563, 18901, 13103, 16867, 9714, 14322, 15197, 26889, 19372, 26241,
	31925, 14640, 11497, 8941, 10056, 6451, 28656, 10737, 13874, 17356, 8281, 25937, 1661, 4850, 7448, 12744,
	21826, 5477, 10167, 16705, 26897, 8839, 30947, 27978, 27283, 24685, 32298, 3525, 12398, 28726, 9475, 10208,
	617, 13467, 22287, 2376, 6097, 26312, 2974, 9114, 21787, 28010, 4725, 15387, 3274, 10762, 31695, 17320,
	18324, 12441, 16801, 27376, 22464, 7500, 5666, 18144, 15314, 31914, 31627, 6495, 5226, 31203, 2331, 4668,
	12650, 18275, 351, 7268, 31319, 30119, 7600, 2905, 13826, 11343, 13053, 15583, 30055, 31093, 5067, 761,
	9685, 11070, 21369, 27155, 3663, 26542, 20169, 12161, 15411, 30401, 7580, 31784, 8985, 29367, 20989, 14203,
	29694, 21167, 10337, 1706, 28578, 887, 3373, 19477, 14382, 675, 7033, 15111, 26138, 12252, 30996, 21409,
	25678, 18555, 13256, 23316, 22407, 16727, 991, 9236, 5373, 29402, 6117, 15241, 27715, 19291, 19888, 19847
};

unsigned int U_Random(void)
{
	glSeed *= 69069;
	glSeed += seed_table[glSeed & 0xFF];
	return (++glSeed & 0x0FFFFFFF);
}

void U_Srand(unsigned int seed)
{
	glSeed = seed_table[seed & 0xFF];
}

int UTIL_SharedRandomLong(unsigned int seed, int low, int high)
{
	U_Srand((int)seed + low + high);

	unsigned int range = high - low + 1;

	if (!(range - 1))
		return low;

	int rnum = U_Random();
	int offset = rnum % range;
	return (low + offset);
}

float UTIL_SharedRandomFloat(unsigned int seed, float low, float high)
{
	U_Srand((int)seed + *(int *)&low + *(int *)&high);
	U_Random();
	U_Random();

	unsigned range = (int)(high - low);

	if (!range)
		return low;

	int tensixrand = U_Random() & 65535;
	float offset = (float)tensixrand / 65536;
	return (low + offset * range);
}