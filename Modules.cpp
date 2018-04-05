#include <metahook.h>
#include "plugins.h"
#include "configs.h"
#include "BaseUI.h"
//#include "links.h"

#include <vgui/VGUI.h>
#include <IClientVGUI.h>

HMODULE g_hClientModule = NULL;
HMODULE g_hGameUIModule = NULL;
HMODULE g_hVGUI2Module = NULL;
HMODULE g_hGameOverlayRenderer = NULL;

HMODULE (WINAPI *g_pfnLoadLibraryA)(LPCSTR lpLibFileName);
hook_t *g_phLoadLibraryA;
FARPROC (WINAPI *g_pfnGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
hook_t *g_phGetProcAddress;
BOOL (WINAPI *g_pfnFreeLibrary)(HMODULE hLibModule);
hook_t *g_phFreeLibrary;

void *(*g_pfnClient_CreateInterfaceFn)(const char *pName, int *pReturnCode);
void *(*g_pfnVGUI2_CreateInterfaceFn)(const char *pName, int *pReturnCode);

void *Hook_Client_CreateInterfaceFn(const char *pName, int *pReturnCode)
{
	/*void *p = CreateInterface(pName, pReturnCode);

	if (p)
	{
	if (!strcmp(pName, CLIENTVGUI_INTERFACE_VERSION))
	return p;
	}*/

	return g_pfnClient_CreateInterfaceFn(pName, pReturnCode);
}

void *Hook_VGUI2_CreateInterfaceFn(const char *pName, int *pReturnCode)
{
	void *p = CreateInterface(pName, pReturnCode);

	if (p)
	{
		if (!strcmp(pName, VGUI_SCHEME_INTERFACE_VERSION)) // IScheme
			return p;
	}

	return g_pfnVGUI2_CreateInterfaceFn(pName, pReturnCode);
}

int Hook_EmptyFunction(...)
{
	return 0;
}

FARPROC WINAPI Hook_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	if (g_hThisModule == hModule)
	{
		FARPROC result = g_pfnGetProcAddress(hModule, lpProcName);

		if (!result)
		{
			if (!strncmp(lpProcName, "Steam_", 6))
				return (FARPROC)Hook_EmptyFunction;
		}

		return result;
	}/*
	else if (g_hClientModule == hModule)
	{
		if (!strcmp(lpProcName, CREATEINTERFACE_PROCNAME))
		{
			g_pfnCreateInterfaceFn = (void *(*)(const char *, int *))g_pfnGetProcAddress(hModule, lpProcName);
			return (FARPROC)Hook_CreateInterfaceFn;
		}
	}*/
	if (g_hGameUIModule == hModule)
	{
		return g_pfnGetProcAddress(hModule, lpProcName);
	}

	if (g_hVGUI2Module == hModule)
	{
		if (!strcmp(lpProcName, CREATEINTERFACE_PROCNAME))
		{
			g_pfnVGUI2_CreateInterfaceFn = (void *(*)(const char *, int *))g_pfnGetProcAddress(hModule, lpProcName);
			return (FARPROC)Hook_VGUI2_CreateInterfaceFn;
		}
	}

	return g_pfnGetProcAddress(hModule, lpProcName);
}

HMODULE WINAPI Hook_LoadLibraryA(LPCSTR lpLibFileName)
{
	if (!g_bIsUseSteam)
	{
		if (strstr(lpLibFileName, "steamclient.dll"))
			return g_hThisModule;
	}
	if (strstr(lpLibFileName, "cl_dlls\\GameUI.dll"))
	{
		return g_hThisModule;
	}
	if (strstr(lpLibFileName, "vgui2.dll"))
	{
		return g_hVGUI2Module = g_pfnLoadLibraryA(lpLibFileName);
	}
	/*
	if (g_dwEngineBuildnum >= 5953)
	{
		if (strstr(lpLibFileName, "cl_dlls\\client.dll"))
		{
			char szNewFileName[MAX_PATH];
			strcpy(szNewFileName, lpLibFileName);

			char *pszResult = (char *)strrchr(szNewFileName, '\\');

			if (pszResult)
			{
				*pszResult = '\0';
				sprintf(szNewFileName, "%s\\new_client.dll", szNewFileName);
				lpLibFileName = szNewFileName;
			}

			g_hClientModule = g_pfnLoadLibraryA(lpLibFileName);

			if (g_hClientModule)
				CL_LinkNewClient((DWORD)g_hClientModule);

			return g_hClientModule;
		}

		if (strstr(lpLibFileName, "cl_dlls\\new_client.dll"))
		{
			g_hClientModule = g_pfnLoadLibraryA(lpLibFileName);

			if (g_hClientModule)
				CL_LinkNewClient((DWORD)g_hClientModule);

			return g_hClientModule;
		}
	}
	*/
	return g_pfnLoadLibraryA(lpLibFileName);
}

BOOL WINAPI Hook_FreeLibrary(HMODULE hLibModule)
{
	if (hLibModule == g_hThisModule)
		return FALSE;

	return g_pfnFreeLibrary(hLibModule);
}

void Module_InstallHook(void)
{
	g_phLoadLibraryA = g_pMetaHookAPI->InlineHook((void *)LoadLibraryA, Hook_LoadLibraryA, (void *&)g_pfnLoadLibraryA);
	g_phGetProcAddress = g_pMetaHookAPI->InlineHook((void *)GetProcAddress, Hook_GetProcAddress, (void *&)g_pfnGetProcAddress);
	g_phFreeLibrary = g_pMetaHookAPI->InlineHook((void *)FreeLibrary, Hook_FreeLibrary, (void *&)g_pfnFreeLibrary);
}

void Module_Shutdown(void)
{
	g_pMetaHookAPI->UnHook(g_phLoadLibraryA);
	g_pMetaHookAPI->UnHook(g_phGetProcAddress);
	g_pMetaHookAPI->UnHook(g_phFreeLibrary);
}

void Module_LoadClient(cl_exportfuncs_t *pExportFunc)
{
	CreateInterfaceFn fnCreateInterfaceClient = (CreateInterfaceFn)pExportFunc->ClientFactory();
	g_pMetaHookAPI->InlineHook((void *)fnCreateInterfaceClient, Hook_Client_CreateInterfaceFn, (void *&)g_pfnClient_CreateInterfaceFn);
}