#include <metahook.h>
#include "base.h"
#include "configs.h"
#include "BaseUI.h"
#include "PlayAudio.h"

#include "Input.h"

#include "DrawTargaImage.h"
#include "util.h"
#include "common.h"

#include "GameUI/ModInfo.h"
#include "GameUI/GameUI_Interface.h"
#include "MGUI/BTEPanel.h"

/*
#define CBASEPANEL_CREATEGAMEMENU_SIG "\x56\x57\x6A\x2A\x8B\xF9\xE8\x35\x7F\x02\x00\x83\xC4\x04\x85\xC0"
#define CBASEPANEL_RECURSIVELOADGAMEMENU_SIG "\x51\x55\x56\x57\x8B\xF9\x68\xF4\x00\x00\x00\x89\x7C\x24\x2A\xE8"
#define CBASEPANEL_RUNMENUCOMMAND_SIG "\x53\x8B\x5C\x24\x08\x56\x57\x68\x4C\x37\x2A\x2A\x53\x8B\xF1\xE8"
#define CBASEPANEL_ONGAMEUIACTIVATED_SIG "\x8B\x81\xb4\x00\x00\x00\xb2\x01\x85\xC0\x74"
#define VGUI_FRAME_PAINTBACKGROUND_SIG "\x83\xEC\x20\x53\x55\x56\x57\x8B\xF1\xE8\x02\xA0\xFF\xFF\x89\x44"
#define VGUI_PANEL_PAINTBACKGROUND_SIG "\x83\xEC\x2A\x8D\x44\x24\x04\x56\x8B\xF1\x8D\x4C\x24\x0C\x50\x51"


#define MAX_GAMEMENUITEMS 14
static const char g_szGameUIResourceFileData[MAX_GAMEMENUITEMS][256] =
{
	"\t\t\"label\" \"#CSBTE_About\"\n\t\t\"command\" \"OpenCSBTEAboutDialog\"\n",
	"\t\t\"label\" \"#CSBTE_ModeSelection\"\n\t\t\"command\" \"OpenCSBTEModeSelection\"\n",
	"\t\t\"label\" \"#CSBTE_MyWpnEditor\"\n\t\t\"command\" \"OpenCSBTEMyWpnEditor\"\n",
	"\t\t\"label\" \"\"\n\t\t\"command\" \"\"\n\t\t\\n",
	"\t\t\"label\" \"#GameUI_GameMenu_ResumeGame\"\n\t\t\"command\" \"ResumeGame\"\n\t\t\"OnlyInGame\" \"1\"\n",
	"\t\t\"label\" \"#GameUI_GameMenu_Disconnect\"\n\t\t\"command\" \"Disconnect\"\n\t\t\"OnlyInGame\" \"1\"\n\t\t\"notsingle\" \"1\"\n\t\t\"NotInCareerGame\" \"1\"\n",
	"\t\t\"label\" \"#Career_EndRound\"\n\t\t\"command\" \"EndRound\"\n\t\t\"OnlyInGame\" \"1\"\n\t\t\"notsingle\" \"1\"\n\t\t\"OnlyInCareerGame\" \"1\"\n",
	"\t\t\"label\" \"#Career_Surrender\"\n\t\t\"command\" \"Surrender\"\n\t\t\"OnlyInGame\" \"1\"\n\t\t\"notsingle\" \"1\"\n\t\t\"OnlyInCareerGame\" \"1\"\n",
	"\t\t\"label\" \"#GameUI_GameMenu_PlayerList\"\n\t\t\"command\" \"OpenPlayerListDialog\"\n\t\t\"OnlyInGame\" \"1\"\n\t\t\"notsingle\" \"1\"\n\t\t\"NotInCareerGame\" \"1\"\n",
	"\t\t\"label\" \"\"\n\t\t\"command\" \"\"\n\t\t\"OnlyInGame\" \"1\"\n",
	"\t\t\"label\" \"#Cstrike_NewMultiplayerGame\"\n\t\t\"command\" \"OpenCreateMultiplayerGameDialog\"\n",
	"\t\t\"label\" \"#GameUI_GameMenu_FindServers\"\n\t\t\"command\" \"OpenServerBrowser\"\n",
	"\t\t\"label\" \"#GameUI_GameMenu_Options\"\n\t\t\"command\" \"OpenOptionsDialog\"\n",
	"\t\t\"label\" \"#GameUI_GameMenu_Quit\"\n\t\t\"command\" \"Quit\"\n"
};
*/
using namespace vgui;

extern IKeyValuesSystem *g_pKeyValuesSystem;

HINTERFACEMODULE g_hVGUI2;
//HINTERFACEMODULE g_hGameUI;
//HINTERFACEMODULE g_hServerBrowser;


IEngineVGui *enginevguifuncs;
IEngineSurface *staticSurface;
ISurface *enginesurfacefuncs;
IGameUIFuncs *gameuifuncs;
IBaseUI *baseuifuncs = NULL;

vgui::IPanel *g_pPanel;

void *staticPanel = NULL;

void(__fastcall *g_pfnCBaseUI_Initialize)(void *pthis, int, CreateInterfaceFn *factories, int count);
void(__fastcall *g_pfnCBaseUI_Start)(void *pthis, int, struct cl_enginefuncs_s *engineFuncs, int interfaceVersion);
void(__fastcall *g_pfnCBaseUI_Shutdown)(void *pthis, int);
int(__fastcall *g_pfnCBaseUI_Key_Event)(void *pthis, int, int down, int keynum, const char *pszCurrentBinding);
void(__fastcall *g_pfnCBaseUI_CallEngineSurfaceProc)(void *pthis, int, void *hwnd, unsigned int msg, unsigned int wparam, long lparam);
void(__fastcall *g_pfnCBaseUI_Paint)(void *pthis, int, int x, int y, int right, int bottom);
void(__fastcall *g_pfnCBaseUI_HideGameUI)(void *pthis, int);
void(__fastcall *g_pfnCBaseUI_ActivateGameUI)(void *pthis, int);
bool(__fastcall *g_pfnCBaseUI_IsGameUIVisible)(void *pthis, int);
void(__fastcall *g_pfnCBaseUI_HideConsole)(void *pthis, int);
void(__fastcall *g_pfnCBaseUI_ShowConsole)(void *pthis, int);

void(__fastcall *g_pfnRegisterSizeofKeyValues)(void *pthis, int edx, int size) = NULL;
void *(__fastcall *g_pfnAllocKeyValuesMemory)(void *pthis, int edx, int size) = NULL;
void(__fastcall *g_pfnFreeKeyValuesMemory)(void *pthis, int edx, void *pMem) = NULL;
HKeySymbol(__fastcall *g_pfnGetSymbolForString)(void *pthis, int edx, const char *name) = NULL;
const char *(__fastcall *g_pfnGetStringForSymbol)(void *pthis, int edx, HKeySymbol symbol) = NULL;
void(__fastcall *g_pfnGetLocalizedFromANSI)(void *pthis, int edx, const char *ansi, wchar_t *outBuf, int unicodeBufferSizeInBytes) = NULL;
void(__fastcall *g_pfnGetANSIFromLocalized)(void *pthis, int edx, const wchar_t *wchar, char *outBuf, int ansiBufferSizeInBytes) = NULL;
void(__fastcall *g_pfnAddKeyValuesToMemoryLeakList)(void *pthis, int edx, void *pMem, HKeySymbol name) = NULL;
void(_fastcall *g_pfnRemoveKeyValuesFromMemoryLeakList)(void *pthis, int edx, void *pMem) = NULL;

void(__fastcall *g_pfnCGameUI_Initialize)(void *pthis, int edx, CreateInterfaceFn *factories, int count) = 0;
void(__fastcall *g_pfnCGameUI_Start)(void *pthis, int edx, struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system) = 0;
void(__fastcall *g_pfnCGameUI_Shutdown)(void *pthis, int edx) = 0;
int(__fastcall *g_pfnCGameUI_ActivateGameUI)(void *pthis, int edx) = 0;
int(__fastcall *g_pfnCGameUI_ActivateDemoUI)(void *pthis, int edx) = 0;
int(__fastcall *g_pfnCGameUI_HasExclusiveInput)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCGameUI_RunFrame)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCGameUI_ConnectToServer)(void *pthis, int edx, const char *game, int IP, int port) = 0;
void(__fastcall *g_pfnCGameUI_DisconnectFromServer)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCGameUI_HideGameUI)(void *pthis, int edx) = 0;
bool(__fastcall *g_pfnCGameUI_IsGameUIActive)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCGameUI_LoadingStarted)(void *pthis, int edx, const char *resourceType, const char *resourceName) = 0;
void(__fastcall *g_pfnCGameUI_LoadingFinished)(void *pthis, int edx, const char *resourceType, const char *resourceName) = 0;
void(__fastcall *g_pfnCGameUI_StartProgressBar)(void *pthis, int edx, const char *progressType, int progressSteps) = 0;
int(__fastcall *g_pfnCGameUI_ContinueProgressBar)(void *pthis, int edx, int progressPoint, float progressFraction) = 0;
void(__fastcall *g_pfnCGameUI_StopProgressBar)(void *pthis, int edx, bool bError, const char *failureReason, const char *extendedReason) = 0;
int(__fastcall *g_pfnCGameUI_SetProgressBarStatusText)(void *pthis, int edx, const char *statusText) = 0;
void(__fastcall *g_pfnCGameUI_SetSecondaryProgressBar)(void *pthis, int edx, float progress) = 0;
void(__fastcall *g_pfnCGameUI_SetSecondaryProgressBarText)(void *pthis, int edx, const char *statusText) = 0;

bool g_bGameUIActive = false;
/*
void(__fastcall *g_pfnCBasePanel_RunMenuCommand)(void *, int, const char *);
void __fastcall CBasePanel_RunMenuCommand(void *pthis, int, const char *command);

int(__fastcall *g_pfnCBasePanel_OnGameUIActivated)(void *);
int __fastcall CBasePanel_OnGameUIActivated(void *pthis);
*/

void FillMultiTexturesForWindowDark(int w, int h)
{
	//surface()->DrawSetColor(10, 10, 10, 150);
	//surface()->DrawFilledRect(5, 12, w - 10, h - 15);

	GL_DrawFullPanel(g_UI_Panel_Dark, 0, 0, w + 14, h + 10, 255);
}

extern int g_UI_Button[3][3];

void FillMultiTexturesForBaseButton(int state, int w, int h)
{
	GL_DrawPanel(g_UI_Button[state], 0, 0, w, h, 255);
}

extern int g_UI_TextEntry[3][3];

void FillMultiTexturesForTextEntry(int w, int h)
{
	GL_DrawFullPanel(g_UI_TextEntry, 0, 0, w, h, 255);
}

#include <color.h>
#include <Surface.h>

void CBaseUI::Initialize(CreateInterfaceFn *factories, int count)
{
	g_pfnCBaseUI_Initialize(this, 0, factories, count);
	g_hVGUI2 = (HINTERFACEMODULE)GetModuleHandle("vgui2.dll");
	//g_hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");

	if (g_hVGUI2/* && g_hGameUI*/)
	{
		CreateInterfaceFn fnVGUI2CreateInterface = Sys_GetFactory(g_hVGUI2);
		//CreateInterfaceFn fnGameUICreateInterface = Sys_GetFactory(g_hGameUI);
		CreateInterfaceFn fnEngineCreateInterface = g_pMetaHookAPI->GetEngineFactory();

		/* Initialize GameUI interfaces */
		//g_pGameConsole = (IGameConsole *)fnGameUICreateInterface(GAMECONSOLE_INTERFACE_VERSION, NULL);
		//g_pGameUI = (IGameUI *)fnGameUICreateInterface(GAMEUI_INTERFACE_VERSION, NULL);
		//enginevguifuncs = (IEngineVGui *)fnEngineCreateInterface(VENGINE_VGUI_VERSION, NULL);

		/* Initialize localized string table and surface interfaces */
		g_pVGuiLocalize = (ILocalize *)fnVGUI2CreateInterface(VGUI_LOCALIZE_INTERFACE_VERSION, NULL);
		g_pVGuiSurface = (ISurface *)fnEngineCreateInterface(VGUI_SURFACE_INTERFACE_VERSION, NULL);
		staticSurface = (IEngineSurface *)fnEngineCreateInterface(ENGINE_SURFACE_VERSION, NULL);
		gameuifuncs = (IGameUIFuncs *)fnEngineCreateInterface(VENGINE_GAMEUIFUNCS_VERSION, NULL);
		vgui::IInputInternal *pInput = (vgui::IInputInternal *)fnVGUI2CreateInterface(VGUI_INPUT_INTERFACE_VERSION, NULL);
		//serverbrowser = (IServerBrowser *)CreateInterface(SERVERBROWSER_INTERFACE_VERSION, NULL);

		Input_InstallHook(pInput);
		Surface_InstallHook(g_pVGuiSurface);

		g_pPanel = (vgui::IPanel *)fnVGUI2CreateInterface(VGUI_PANEL_INTERFACE_VERSION, NULL);

		/* Check if the interfaces are valid */
		//if (g_pGameConsole)
		//	LogToFile("已初始化游戏控制台接口");
		//else
		//	LogToFile("初始化游戏控制台接口失败");
		//if (g_pGameUI)
		//	LogToFile("已初始化游戏界面接口");
		//else
		//	LogToFile("初始化游戏界面接口失败");

		//Config_LoadLocalize();

		vgui::localize()->AddFile(g_pFileSystem, "Resource/csbte_%language%.txt");
		vgui::localize()->AddFile(g_pFileSystem, "Resource/mapname_%language%.txt");

		PlayAudio_Init();
		PlayAudio_Frame();
		//LogToFile(UnicodeToANSI(vgui::localize()->Find("CSBTE")));	

		//g_pfnCBasePanel_RunMenuCommand = (void(__fastcall *)(void *, int, const char *))g_pMetaHookAPI->SearchPattern((void *)g_hGameUI, g_pMetaHookAPI->GetModuleSize((HMODULE)g_hGameUI), CBASEPANEL_RUNMENUCOMMAND_SIG, sizeof(CBASEPANEL_RUNMENUCOMMAND_SIG)-1);
		//g_pMetaHookAPI->InlineHook(g_pfnCBasePanel_RunMenuCommand, CBasePanel_RunMenuCommand, (void *&)g_pfnCBasePanel_RunMenuCommand);

		//g_pfnCBasePanel_OnGameUIActivated = (int(__fastcall *)(void *))g_pMetaHookAPI->SearchPattern((void *)g_hGameUI, g_pMetaHookAPI->GetModuleSize((HMODULE)g_hGameUI), CBASEPANEL_ONGAMEUIACTIVATED_SIG, sizeof(CBASEPANEL_ONGAMEUIACTIVATED_SIG) - 1);
		//g_pMetaHookAPI->InlineHook(g_pfnCBasePanel_OnGameUIActivated, CBasePanel_OnGameUIActivated, (void *&)g_pfnCBasePanel_OnGameUIActivated);
		
	}
	else
	{
		if (!g_hVGUI2)
			LogToFile("内存中找不到VGUI2.dll");
		//if (!g_hGameUI)
		//	LogToFile("内存中找不到GameUI.dll");
	}
}

void CBaseUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion)
{
	g_pfnCBaseUI_Start(this, 0, engineFuncs, interfaceVersion);

	vgui::localize()->AddFile(g_pFileSystem, va("Resource/cstrike_%s.txt", gConfigs.szCSOLanguageName));
	vgui::localize()->AddFile(g_pFileSystem, va("Resource/cso_%s.txt", gConfigs.szCSOLanguageName));

	/*
	StringIndex_t index;
	index = localize()->FindIndex("#CSBTE_About");
	if (index != INVALID_STRING_INDEX)
	{
		if (!Q_stricmp(gConfigs.szLanguage, "schinese"))
			localize()->SetValueByIndex(index, L"Counter-Strike: BreakThrough Edition");
		else if (!Q_stricmp(gConfigs.szLanguage, "tchinese"))
			localize()->SetValueByIndex(index, L"Counter-Strike: BreakThrough Edition");
		else
			localize()->SetValueByIndex(index, L"Counter-Strike: BreakThrough Edition");
	}
	else
	{
		if (!Q_stricmp(gConfigs.szLanguage, "schinese"))
			localize()->AddString("#CSBTE_About", L"关于CSBTE", "Resource/csbte_schinese.txt");
		else if (!Q_stricmp(gConfigs.szLanguage, "tchinese"))
			localize()->AddString("#CSBTE_About", L"關於CSBTE", "Resource/csbte_tchinese.txt");
		else
			localize()->AddString("#CSBTE_About", L"About CSBTE", va("Resource/csbte_%s.txt", gConfigs.szLanguage));
	}

	index = localize()->FindIndex("#CSBTE_ModeSelection");
	if (index != INVALID_STRING_INDEX)
	{
		if (!Q_stricmp(gConfigs.szLanguage, "schinese"))
			localize()->SetValueByIndex(index, L"游戏模式选择");
		else if (!Q_stricmp(gConfigs.szLanguage, "tchinese"))
			localize()->SetValueByIndex(index, L"遊戲模式選單");
		else
			localize()->SetValueByIndex(index, L"Mode Selection");
	}
	else
	{
		if (!Q_stricmp(gConfigs.szLanguage, "schinese"))
			localize()->AddString("#CSBTE_ModeSelection", L"游戏模式选择", "Resource/csbte_schinese.txt");
		else if (!Q_stricmp(gConfigs.szLanguage, "tchinese"))
			localize()->AddString("#CSBTE_ModeSelection", L"遊戲模式選單", "Resource/csbte_tchinese.txt");
		else
			localize()->AddString("#CSBTE_ModeSelection", L"GameMode Selection", va("Resource/csbte_%s.txt", gConfigs.szLanguage));
	}

	index = localize()->FindIndex("#CSBTE_MyWpnEditor");
	if (index != INVALID_STRING_INDEX)
	{
		if (!Q_stricmp(gConfigs.szLanguage, "schinese"))
			localize()->SetValueByIndex(index, L"武器仓库编辑");
		else if (!Q_stricmp(gConfigs.szLanguage, "tchinese"))
			localize()->SetValueByIndex(index, L"武器倉庫編輯");
		else
			localize()->SetValueByIndex(index, L"MyWpn Editor");
	}
	else
	{
		if (!Q_stricmp(gConfigs.szLanguage, "schinese"))
			localize()->AddString("#CSBTE_MyWpnEditor", L"武器仓库编辑", "Resource/csbte_schinese.txt");
		else if (!Q_stricmp(gConfigs.szLanguage, "tchinese"))
			localize()->AddString("#CSBTE_MyWpnEditor", L"武器倉庫編輯", "Resource/csbte_tchinese.txt");
		else
			localize()->AddString("#CSBTE_MyWpnEditor", L"MyWpn Editor", va("Resource/csbte_%s.txt", gConfigs.szLanguage));
	}
	*/
}

void CBaseUI::Shutdown(void)
{
	g_pfnCBaseUI_Shutdown(this, 0);
}

int CBaseUI::Key_Event(int down, int keynum, const char *pszCurrentBinding)
{
	return g_pfnCBaseUI_Key_Event(this, 0, down, keynum, pszCurrentBinding);
}

void CBaseUI::CallEngineSurfaceProc(void *hwnd, unsigned int msg, unsigned int wparam, long lparam)
{
	g_pfnCBaseUI_CallEngineSurfaceProc(this, 0, hwnd, msg, wparam, lparam);
}

void CBaseUI::Paint(int x, int y, int right, int bottom)
{
	g_pfnCBaseUI_Paint(this, 0, x, y, right, bottom);
}

void CBaseUI::HideGameUI(void)
{
	//ImmAssociateContextEx(g_hWnd, NULL, IACE_CHILDREN);
	//ImmSetOpenStatus(ImmGetContext(g_hWnd), TRUE);
	//engine->Con_Printf("IME Disabled.\nSwitched to other language.\n");
	g_bGameUIActive = false;
	g_pfnCBaseUI_HideGameUI(this, 0);
}

void CBaseUI::ActivateGameUI(void)
{
	//ImmAssociateContextEx(g_hWnd, NULL, IACE_DEFAULT);
	//ImmSetOpenStatus(ImmGetContext(g_hWnd), FALSE);
	//engine->Con_Printf("IME Enabled.\nSwitched to english.\n");
	g_bGameUIActive = true;
	g_pfnCBaseUI_ActivateGameUI(this, 0);
}

bool CBaseUI::IsGameUIVisible(void)
{
	return g_pfnCBaseUI_IsGameUIVisible(this, 0);
}

void CBaseUI::HideConsole(void)
{
	g_pfnCBaseUI_HideConsole(this, 0);
	//if (GameConsole().IsConsoleVisible())
	//	GameConsole().Hide();
}

void CBaseUI::ShowConsole(void)
{
	g_pfnCBaseUI_ShowConsole(this, 0);
	//GameConsole().Activate();
}

void CKeyValuesSystem::RegisterSizeofKeyValues(int size)
{
	return g_pfnRegisterSizeofKeyValues(this, 0, size);
}

void *CKeyValuesSystem::AllocKeyValuesMemory(int size)
{
	return malloc(size);
}
void CKeyValuesSystem::FreeKeyValuesMemory(void *pMem)
{
	return free(pMem);
}

HKeySymbol CKeyValuesSystem::GetSymbolForString(const char *name)
{
	return g_pfnGetSymbolForString(this, 0, name);
}

const char *CKeyValuesSystem::GetStringForSymbol(HKeySymbol symbol)
{
	return g_pfnGetStringForSymbol(this, 0, symbol);
}

void CKeyValuesSystem::GetLocalizedFromANSI(const char *ansi, wchar_t *outBuf, int unicodeBufferSizeInBytes)
{
	return g_pfnGetLocalizedFromANSI(this, 0, ansi, outBuf, unicodeBufferSizeInBytes);
}

void CKeyValuesSystem::GetANSIFromLocalized(const wchar_t *wchar, char *outBuf, int ansiBufferSizeInBytes)
{
	return g_pfnGetANSIFromLocalized(this, 0, wchar, outBuf, ansiBufferSizeInBytes);
}

void CKeyValuesSystem::AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name)
{
	return g_pfnAddKeyValuesToMemoryLeakList(this, 0, pMem, name);
}

void CKeyValuesSystem::RemoveKeyValuesFromMemoryLeakList(void *pMem)
{
	return g_pfnRemoveKeyValuesFromMemoryLeakList(this, 0, pMem);
}
/*
void RewriteGameMenuResourceFile(void)
{
	char szPath[MAX_PATH];
	g_pFileSystem->GetLocalPath("resource/GameMenu.res", szPath, sizeof(szPath));
	DeleteFile(szPath);

	FILE *fp = fopen(szPath, "a");

	if (!fp)
		return;

	fprintf(fp, "// This file has been locked, any change you make is invalid because it will be rewritten when loading the engine.\n\n");
	fprintf(fp, "\"GameMenu\"\n{\n");

	for (int i = 0; i < MAX_GAMEMENUITEMS; i++)
	{
		fprintf(fp, "\t\"%d\"\n\t{\n", i + 1);
		fprintf(fp, g_szGameUIResourceFileData[i]);
		fprintf(fp, "\t}\n");
	}

	fprintf(fp, "}");

	fclose(fp);
}

void CBasePanel_PositionDialog(vgui::PHandle dlg)
{
	if (!dlg.Get())
		return;

	int x, y, ww, wt, wide, tall;
	vgui::surface()->GetWorkspaceBounds(x, y, ww, wt);
	dlg->GetSize(wide, tall);
	dlg->SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2));
}

void OnOpenQuitConfirmationDialog()
{
	VPANEL GameUIPanel = enginevguifuncs->GetPanel(PANEL_GAMEUIDLL);

	QueryBox *box = new QueryBox("#GameUI_QuitConfirmationTitle", "#GameUI_QuitConfirmationText", NULL);
	box->SetParent(GameUIPanel);
	box->SetOKButtonText("#GameUI_Quit");
	box->SetOKCommand(new KeyValues("Command", "command", "QuitNoConfirm"));
	box->SetCancelCommand(new KeyValues("Command", "command", "ReleaseModalWindow"));
	box->AddActionSignalTarget((Panel *)staticPanel);
	box->DoModal();
	vgui::surface()->RestrictPaintToSinglePanel(box->GetVPanel());
}
*/
void BaseUI_InstallHook(void)
{
	CreateInterfaceFn fnCreateInterface = g_pMetaHookAPI->GetEngineFactory();
	baseuifuncs = (IBaseUI *)fnCreateInterface(BASEUI_INTERFACE_VERSION, NULL);
	
	CBaseUI BaseUI;
	DWORD *pVFTable = *(DWORD **)&BaseUI;

	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 1, (void *)pVFTable[1], (void *&)g_pfnCBaseUI_Initialize);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 2, (void *)pVFTable[2], (void *&)g_pfnCBaseUI_Start);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 3, (void *)pVFTable[3], (void *&)g_pfnCBaseUI_Shutdown);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 4, (void *)pVFTable[4], (void *&)g_pfnCBaseUI_Key_Event);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 5, (void *)pVFTable[5], (void *&)g_pfnCBaseUI_CallEngineSurfaceProc);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 6, (void *)pVFTable[6], (void *&)g_pfnCBaseUI_Paint);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 7, (void *)pVFTable[7], (void *&)g_pfnCBaseUI_HideGameUI);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 8, (void *)pVFTable[8], (void *&)g_pfnCBaseUI_ActivateGameUI);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 9, (void *)pVFTable[9], (void *&)g_pfnCBaseUI_IsGameUIVisible);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 10, (void *)pVFTable[10], (void *&)g_pfnCBaseUI_HideConsole);
	g_pMetaHookAPI->VFTHook(baseuifuncs, 0, 11, (void *)pVFTable[11], (void *&)g_pfnCBaseUI_ShowConsole);

}

void KeyValuesSystem_InstallHook(void)
{
	CKeyValuesSystem KeyValuesSystem;
	DWORD *pVFTable = *(DWORD **)&KeyValuesSystem;

	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 1, (void *)pVFTable[1], (void *&)g_pfnRegisterSizeofKeyValues);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 2, (void *)pVFTable[2], (void *&)g_pfnAllocKeyValuesMemory);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 3, (void *)pVFTable[3], (void *&)g_pfnFreeKeyValuesMemory);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 4, (void *)pVFTable[4], (void *&)g_pfnGetSymbolForString);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 5, (void *)pVFTable[5], (void *&)g_pfnGetStringForSymbol);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 6, (void *)pVFTable[6], (void *&)g_pfnGetLocalizedFromANSI);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 7, (void *)pVFTable[7], (void *&)g_pfnGetANSIFromLocalized);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 8, (void *)pVFTable[8], (void *&)g_pfnAddKeyValuesToMemoryLeakList);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 9, (void *)pVFTable[9], (void *&)g_pfnRemoveKeyValuesFromMemoryLeakList);
}