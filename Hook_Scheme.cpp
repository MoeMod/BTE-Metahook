#include <metahook.h>
#include <VGUI/IScheme.h>
#include <Color.h>

#include <set>

using namespace vgui;

HScheme (__fastcall *g_pfnCSchemeManager_LoadSchemeFromFile)(void *pthis, int, const char *fileName, const char *tag);
void (__fastcall *g_pfnCSchemeManager_ReloadSchemes)(void *pthis, int);
HScheme (__fastcall *g_pfnCSchemeManager_GetDefaultScheme)(void *pthis, int);
HScheme (__fastcall *g_pfnCSchemeManager_GetScheme)(void *pthis, int, const char *tag);
IImage *(__fastcall *g_pfnCSchemeManager_GetImage)(void *pthis, int, const char *imageName, bool hardwareFiltered);
HTexture (__fastcall *g_pfnCSchemeManager_GetImageID)(void *pthis, int, const char *imageName, bool hardwareFiltered);
IScheme *(__fastcall *g_pfnCSchemeManager_GetIScheme)(void *pthis, int, HScheme scheme);
void (__fastcall *g_pfnCSchemeManager_Shutdown)(void *pthis, int, bool full);
int (__fastcall *g_pfnCSchemeManager_GetProportionalScaledValue)(void *pthis, int, int normalizedValue);
int (__fastcall *g_pfnCSchemeManager_GetProportionalNormalizedValue)(void *pthis, int, int scaledValue);

class CNewSchemeManager : public ISchemeManager
{
public:
	virtual HScheme LoadSchemeFromFile(const char *fileName, const char *tag);
	virtual void ReloadSchemes(void);
	virtual HScheme GetDefaultScheme(void);
	virtual HScheme GetScheme(const char *tag);
	virtual IImage *GetImage(const char *imageName, bool hardwareFiltered);
	virtual HTexture GetImageID(const char *imageName, bool hardwareFiltered);
	virtual IScheme *GetIScheme(HScheme scheme);
	virtual void Shutdown(bool full = true);
	virtual int GetProportionalScaledValue(int normalizedValue);
	virtual int GetProportionalNormalizedValue(int scaledValue);
};

ISchemeManager *g_pSchemeManager = NULL;

IScheme *CNewSchemeManager::GetIScheme(HScheme scheme)
{
	return g_pfnCSchemeManager_GetIScheme(this, 0, scheme);
}

void CNewSchemeManager::Shutdown(bool full)
{
	return g_pfnCSchemeManager_Shutdown(this, 0, full);
}

HScheme CNewSchemeManager::LoadSchemeFromFile(const char *fileName, const char *tag)
{
	if (tag)
	{
		char szPath[MAX_PATH];

		if (!strcmp(tag, "BaseUI"))
		{
			//sprintf(szPath, "thanatos/resource/TrackerScheme.res");
			sprintf(szPath, fileName);
			return g_pfnCSchemeManager_LoadSchemeFromFile(this, 0, szPath, tag);
		}
		/*else if (!strcmp(tag, "ClientScheme"))
		{
			//sprintf(szPath, "thanatos/resource/ClientScheme.res");
			sprintf(szPath, fileName);
			return g_pfnCSchemeManager_LoadSchemeFromFile(this, 0, szPath, tag);
		}*/
	}

	return g_pfnCSchemeManager_LoadSchemeFromFile(this, 0, fileName, tag);
}

void CNewSchemeManager::ReloadSchemes(void)
{
	return g_pfnCSchemeManager_ReloadSchemes(this, 0);
}

HScheme CNewSchemeManager::GetDefaultScheme(void)
{
	return g_pfnCSchemeManager_GetDefaultScheme(this, 0);
}

HScheme CNewSchemeManager::GetScheme(const char *tag)
{
	return g_pfnCSchemeManager_GetScheme(this, 0, tag);
}

int CNewSchemeManager::GetProportionalScaledValue(int normalizedValue)
{
	return g_pfnCSchemeManager_GetProportionalScaledValue(this, 0, normalizedValue);
}

int CNewSchemeManager::GetProportionalNormalizedValue(int scaledValue)
{
	return g_pfnCSchemeManager_GetProportionalNormalizedValue(this, 0, scaledValue);
}

IImage *CNewSchemeManager::GetImage(const char *imageName, bool hardwareFiltered)
{
	return g_pfnCSchemeManager_GetImage(this, 0, imageName, hardwareFiltered);
}

HTexture CNewSchemeManager::GetImageID(const char *imageName, bool hardwareFiltered)
{
	return g_pfnCSchemeManager_GetImageID(this, 0, imageName, hardwareFiltered);
}

void SchemeManager_InstallHook(vgui::ISchemeManager *pSchemeManager)
{
	g_pSchemeManager = pSchemeManager;

	CNewSchemeManager NewSchemeManager;
	DWORD *pVFTable = *(DWORD **)&NewSchemeManager;

	g_pSchemeManager = pSchemeManager;
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 1, (void *)pVFTable[1], (void *&)g_pfnCSchemeManager_LoadSchemeFromFile);
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 2, (void *)pVFTable[2], (void *&)g_pfnCSchemeManager_ReloadSchemes);
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 3, (void *)pVFTable[3], (void *&)g_pfnCSchemeManager_GetDefaultScheme);
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 4, (void *)pVFTable[4], (void *&)g_pfnCSchemeManager_GetScheme);
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 5, (void *)pVFTable[5], (void *&)g_pfnCSchemeManager_GetImage);
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 6, (void *)pVFTable[6], (void *&)g_pfnCSchemeManager_GetImageID);
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 7, (void *)pVFTable[7], (void *&)g_pfnCSchemeManager_GetIScheme);
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 8, (void *)pVFTable[8], (void *&)g_pfnCSchemeManager_Shutdown);
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 9, (void *)pVFTable[9], (void *&)g_pfnCSchemeManager_GetProportionalScaledValue);
	g_pMetaHookAPI->VFTHook(g_pSchemeManager, 0, 10, (void *)pVFTable[10], (void *&)g_pfnCSchemeManager_GetProportionalNormalizedValue);
}
