#include "metahook.h"
#include "bte_const.h"
#include "vgui_controls/Controls.h"
#include "PlayerClassManager.h"
#include "util.h"
#include "common.h"

CPlayerClassManager &PlayerClassManager()
{
	static CPlayerClassManager x;
	return x;
}

char *AliasClassName(char *name)
{

	if (!stricmp(name, "spet"))
		return "Spetsnaz";
	else if (!stricmp(name, "ara"))
		return "jra";
	else if (!stricmp(name, "rbc"))
		return "rb";
	else if (!stricmp(name, "rbc"))
		return "rb";
	else if (!stricmp(name, "vc"))
		return "ira";
	else if (!stricmp(name, "nlc"))
		return "eta";
	return name;
}

wchar_t *GetClassNameFormat(char *name)
{
	char *pTemp = AliasClassName(name);

	wchar_t *wstrResult = vgui::localize()->Find(va("#Cstrike_%s", pTemp));

	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSO_Item_Name_%s", pTemp));

	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSBTE_%s", pTemp));

	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSO_%s", pTemp));

	return wstrResult ? wstrResult : ANSIToUnicode(va("#Cstrike_%s", pTemp));
}

wchar_t *GetClassInfoFormat(char *name)
{
	//return L" ";
	char *pTemp = AliasClassName(name);

	//return ANSIToUnicode(va("#Cstrike_%s_Label", pTemp));

	wchar_t *wstrResult = GetLangUni(va("#Cstrike_%s_Label", pTemp));

	return wstrResult;// ? wstrResult : ANSIToUnicode(va("#Cstrike_%s_Label", pTemp));
}

auto CPlayerClassManager::Find(const char *szName) -> ClassData &
{
	auto it = m_DataMap.find(szName);
	if (it != m_DataMap.end())
		return *(it->second);

	return m_NullClass;
}

auto CPlayerClassManager::OnPrecacheClass(const char *szName) -> ClassData &
{
	auto it = m_DataMap.find(szName);
	if (it != m_DataMap.end())
		return *(it->second);

	if (szName)
	{
		ClassData Data = LoadClassData(szName);
		ClassData *pResult = new ClassData(Data);
		m_DataMap[szName] = pResult;
		return *pResult;
	}
	return m_NullClass;
}

auto CPlayerClassManager::LoadClassData(const char *szName) -> ClassData
{
	ClassData Result;

	char szValue[128];

	sprintf(Result.model, "%s", szName);

	GetPrivateProfileStringA(szName, "Sex", "1", szValue, sizeof(szValue), "cstrike/class.ini");
	if (!stricmp(szValue, "Male") || atoi(szValue) == 1)
		Result.sex = SEX_MALE;
	else if (!stricmp(szValue, "Female") || atoi(szValue) == 2)
		Result.sex = SEX_FEMALE;
	else 
		Result.sex = SEX_MALE;

	GetPrivateProfileStringA(szName, "Team", "0", szValue, sizeof(szValue), "cstrike/class.ini");
	if (szValue[0]=='T' || atoi(szValue) == 1)
		Result.team = TEAM_TERRORIST;
	else if (szValue[0] == 'C' || atoi(szValue) == 2)
		Result.team = TEAM_CT;
	else
		Result.team = TEAM_UNASSIGNED;

	GetPrivateProfileStringA(szName, "TextureReplaceKey", "HandDefault", szValue, sizeof(szValue), "cstrike/class.ini");
	sprintf(Result.szTextureReplaceKey, "%s", szValue);

	//return (ClassData &&)Result;
	return std::move(Result);
}