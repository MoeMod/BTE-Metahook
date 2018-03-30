#include <metahook.h>
#include <plugins.h>
#include <exportfuncs.h>
#include <hud.h>
#include "PrecacheManager.h"

#include "Client/PlayerClassManager.h"
#include "Client/WeaponManager.h"

#include "Client/HUD/DrawTABPanel.h"

#include <algorithm>

void PrecacheManager_InstallHook();

CPrecacheManager &PrecacheManager()
{
	static CPrecacheManager x;
	return x;
}

void CPrecacheManager::OnPrecacheModel(const char *szFileName)
{
	if (strstr(szFileName, "models/v_") || strstr(szFileName, "models/p_"))
	{
		
		if (std::find(m_vecModelPrecache.begin(), m_vecModelPrecache.end(), szFileName) != m_vecModelPrecache.end())
			return;
		m_vecModelPrecache.push_back(szFileName);

		char name[64], text[64];
		strcpy(name, szFileName + 9);
		name[strlen(name) - 4] = 0;

		if (szFileName[7] == 'v' && !strstr(szFileName, "_2.mdl"))
			WeaponManager().OnPrecacheWeapon(name);
		//LoadWeaponData(name);

		if (strstr(name, "buff"))
		{
			HudTABBoard().SetHostOwnBuff(true);
		}

		return;
	}

	if (strstr(szFileName, "models/player/"))
	{
		char name[64];
		strcpy(name, szFileName + 14);
		name[strlen(name) - 4] = 0;
		name[strlen(name) / 2] = 0;

		//LoadClassData(name);
		PlayerClassManager().OnPrecacheClass(name);
	}
}

void CPrecacheManager::Init()
{
	PrecacheManager_InstallHook();
}

void CPrecacheManager::VidInit()
{
	m_vecModelPrecache.clear();
}

// hooks below

#define CL_GETMODELBYINDEX_SIG "\x83\xEC\x10\x56\x57\x8B\x7C\x24\x1C\x8B\x34\xBD\x2A\x2A\x2A\x2A\x85\xF6"
#define CL_ADDTORESOURCELIST_SIG "\x8B\x44\x24\x04\x8B\x88\x84\x00\x00\x00\x85\xC9"
struct model_s *(*g_pfnCL_GetModelByIndex)(int index);
struct model_s *CL_GetModelByIndex(int index);
hook_t *g_phCL_GetModelByIndex;
void(*g_pfnCL_AddToResourceList)(resource_t *pResource, resource_t *pList);
hook_t *g_phCL_AddToResourceList;
void CL_AddToResourceList(resource_t *pResource, resource_t *pList);

struct model_s *CL_GetModelByIndex(int index)
{
	if (index == -1)
		return NULL;

	if (index >= 512) // WTF??
		return IEngineStudio.Mod_ForName(PrecacheManager().m_vecModelPrecache[index - 512].c_str(), false);

	return g_pfnCL_GetModelByIndex(index);
}

void CL_AddToResourceList(resource_t *pResource, resource_t *pList)
{
	if (pResource->type == t_model)
	{
		PrecacheManager().OnPrecacheModel(pResource->szFileName);
	}
	g_pfnCL_AddToResourceList(pResource, pList);
}

void PrecacheManager_InstallHook()
{
	g_pfnCL_GetModelByIndex = (struct model_s *(*)(int))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, CL_GETMODELBYINDEX_SIG, sizeof(CL_GETMODELBYINDEX_SIG) - 1);
	g_pfnCL_AddToResourceList = (void(*)(resource_t *, resource_t *))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, CL_ADDTORESOURCELIST_SIG, sizeof(CL_ADDTORESOURCELIST_SIG) - 1);

	g_phCL_AddToResourceList = g_pMetaHookAPI->InlineHook(g_pfnCL_AddToResourceList, CL_AddToResourceList, (void *&)g_pfnCL_AddToResourceList);
	g_phCL_GetModelByIndex = g_pMetaHookAPI->InlineHook(g_pfnCL_GetModelByIndex, CL_GetModelByIndex, (void *&)g_pfnCL_GetModelByIndex);
}

