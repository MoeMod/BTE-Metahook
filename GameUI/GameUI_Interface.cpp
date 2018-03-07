#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <io.h>

#include <BaseUI.h>
#include "EngineInterface.h"

#include <configs.h>

#include <tier0/dbg.h>
#include <direct.h>

#include "interface.h"
#include "vgui_controls/controls.h"
#include "vgui_controls/MessageBox.h"
//#include "vgui_internal.h"

#include "KeyValues.h"
#include <IGameUIFuncs.h>
#include <IBaseUI.h>
#include <IServerBrowser.h>
#include <IVGuiModule.h>

#include <vgui/ILocalize.h>
#include <vgui/IPanel.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/ISystem.h>
#include <vgui/ISurface.h>


#include "BasePanel.h"
#include "ModInfo.h"
#include "GameConsole.h"
#include "LoadingDialog.h"
#include "Configs.h"

#include "GameUI/CSBTEMapLoading.h"
#include "GameUI/CSBTEEscPanel.h"

#include "GameUI_Interface.h"

#include "ServerBrowser/ServerBrowser.h"

IServerBrowser *serverbrowser = NULL;
static CBasePanel *staticPanel = NULL;

static CGameUI g_GameUI;
CGameUI *g_pGameUI = NULL;

vgui::DHANDLE<CLoadingDialog> g_hLoadingDialog;
vgui::DHANDLE<CCSBTEMapLoading>g_hCSBTEMapLoading;

CGameUI &GameUI(void)
{
	return g_GameUI;
}

vgui::Panel *StaticPanel(void)
{
	return staticPanel;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameUI, IGameUI, GAMEUI_INTERFACE_VERSION, g_GameUI);

CGameUI::CGameUI(void)
{
	g_pGameUI = this;
	m_szPreviousStatusText[0] = 0;
	m_bLoadlingLevel = false;
}

CGameUI::~CGameUI(void)
{
	g_pGameUI = NULL;
}

void CGameUI::Initialize(CreateInterfaceFn *factories, int count)
{
	if (!vgui::VGui_InitInterfacesList("GameUI", factories, count))
		return;

	vgui::localize()->AddFile(g_pFullFileSystem, "Resource/gameui_%language%.txt");
	vgui::localize()->AddFile(g_pFullFileSystem, "Resource/valve_%language%.txt");
	vgui::localize()->AddFile(g_pFullFileSystem, "Resource/vgui_%language%.txt");

	ModInfo().LoadCurrentGameInfo();

	enginevguifuncs = (IEngineVGui *)factories[0](VENGINE_VGUI_VERSION, NULL);
	enginesurfacefuncs = (vgui::ISurface *)factories[0](VGUI_SURFACE_INTERFACE_VERSION, NULL);
	gameuifuncs = (IGameUIFuncs *)factories[0](VENGINE_GAMEUIFUNCS_VERSION, NULL);
	baseuifuncs = (IBaseUI *)factories[0](BASEUI_INTERFACE_VERSION, NULL);

	if (!enginesurfacefuncs || !gameuifuncs || !enginevguifuncs || !baseuifuncs)
		Error("CGameUI::Initialize() failed to get necessary interfaces\n");

	staticPanel = new CBasePanel();
	staticPanel->SetBounds(0, 0, 400, 300);
	staticPanel->SetPaintBorderEnabled(false);
	staticPanel->SetPaintBackgroundEnabled(true);
	staticPanel->SetPaintEnabled(false);
	staticPanel->SetVisible(true);
	staticPanel->SetMouseInputEnabled(false);
	staticPanel->SetKeyBoardInputEnabled(false);
	staticPanel->SetParent(enginevguifuncs->GetPanel(PANEL_GAMEUIDLL));

	/*
	HINTERFACEMODULE hServerBrowser = Sys_LoadModule("platform\\servers\\serverbrowser.dll");
	CreateInterfaceFn pfnServerBrowserFactory = Sys_GetFactory(hServerBrowser);
	
	serverbrowser = (IServerBrowser *)pfnServerBrowserFactory(SERVERBROWSER_INTERFACE_VERSION, NULL);
	serverbrowserModule = (IVGuiModule *)pfnServerBrowserFactory("VGuiModuleServerBrowser001", NULL);
	
	if (serverbrowserModule)
	{
		serverbrowserModule->Initialize(factories, count);
		serverbrowserModule->SetParent(staticPanel->GetVPanel());

		serverbrowserModule->PostInitialize(factories, count);
	}*/

	serverbrowser = (IServerBrowser *)CreateInterface(SERVERBROWSER_INTERFACE_VERSION, NULL);
	//serverbrowser = &ServerBrowser();

	if (serverbrowser)
		serverbrowser->Initialize(factories, count);

	if (serverbrowser)
		serverbrowser->SetParent(staticPanel->GetVPanel());
		
	
	vgui::surface()->SetAllowHTMLJavaScript(true);
}

void CGameUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system)
{
	if (serverbrowser)
	{
		serverbrowser->ActiveGameName(gConfigs.szGameName, gConfigs.szGameDir);
		serverbrowser->Reactivate();
	}

	// start mp3 playing
	//engine->pfnClientCmd("mp3 loop media/gamestartup.mp3\n");
	engine->pfnClientCmd("fmod loop media/gamestartup.mp3\n");
}

void CGameUI::Shutdown(void)
{
	/*if (serverbrowserModule)
	{
	//	serverbrowser->Deactivate();
	//	serverbrowser->Shutdown();
		serverbrowserModule->Shutdown();
	}*/

	if (serverbrowser)
	{
		serverbrowser->Deactivate();
		serverbrowser->Shutdown();
	}

	ModInfo().FreeModInfo();
}

int CGameUI::ActivateGameUI(void)
{
	if (!m_bLoadlingLevel && g_hLoadingDialog.Get() && IsInLevel())
	{
		g_hLoadingDialog->Close();
		g_hLoadingDialog = NULL;
	}

	if (!m_bLoadlingLevel)
	{
		if (IsGameUIActive())
			return 1;
	}

	staticPanel->OnGameUIActivated();
	staticPanel->SetVisible(true);

	engine->pfnClientCmd("setpause");

	return 1;
}

int CGameUI::ActivateDemoUI(void)
{
	return 1;
}

int CGameUI::HasExclusiveInput(void)
{
	return IsGameUIActive();
}

void CGameUI::RunFrame(void)
{
	int wide, tall;
	vgui::surface()->GetScreenSize(wide, tall);
	staticPanel->SetSize(wide, tall);

	if (staticPanel->IsVisible())
		BasePanel()->RunFrame();
}

void CGameUI::ConnectToServer(const char *game, int IP, int port)
{
	if (serverbrowser)
		serverbrowser->ConnectToGame(IP, port);

	//engine->pfnClientCmd("mp3 stop\n");
	engine->pfnClientCmd("fmod stop\n");

	baseuifuncs->HideGameUI();
	/*
	KeyValues *pKV = new KeyValues("ConnectedToGame");
	pKV->SetInt("ip", IP);
	pKV->SetInt("port", port);
	pKV->SetString("gamedir", game);

	vgui::ivgui()->PostMessageA(serverbrowserModule->GetPanel(), pKV, staticPanel->GetVPanel());*/
}

void CGameUI::DisconnectFromServer(void)
{
	if (serverbrowser)
		serverbrowser->DisconnectFromGame();

	baseuifuncs->ActivateGameUI();
	//engine->pfnClientCmd("fmod loop media/gamestartup.mp3\n");

}

void CGameUI::HideGameUI(void)
{
	if (!IsGameUIActive())
		return;

	if (!IsInLevel())
		return;

	staticPanel->SetVisible(false);

	engine->pfnClientCmd("unpause");
	engine->pfnClientCmd("hideconsole");

	if (GameConsole().IsConsoleVisible())
		GameConsole().Hide();

	if (!m_bLoadlingLevel && g_hLoadingDialog.Get())
	{
		g_hLoadingDialog->Close();
		g_hLoadingDialog = NULL;
	}
}

bool CGameUI::IsGameUIActive(void)
{
	return staticPanel->IsVisible();
}

void CGameUI::LoadingStarted(const char *resourceType, const char *resourceName)
{
	m_bLoadlingLevel = true;

	if (!g_hCSBTEMapLoading.Get())
	{
		VPANEL GameUIPanel = enginevguifuncs->GetPanel(PANEL_GAMEUIDLL);
		g_hCSBTEMapLoading = new CCSBTEMapLoading(NULL, "CSBTEMapLoading");
		g_hCSBTEMapLoading->SetParent(GameUIPanel);

	}

	if (g_hCSBTEMapLoading->SetMapName(resourceName))
	{
		engine->pfnClientCmd("unpause");
		engine->pfnClientCmd("hideconsole");
		GameConsole().Hide();

		g_hCSBTEMapLoading->SetServerName(engine->pfnGetCvarString("hostname"));
	}
	else
	{
		g_hCSBTEMapLoading->SetVisible(false);
		g_hCSBTEMapLoading->SetAutoDelete(true);
	}

	staticPanel->OnLevelLoadingStarted(resourceName);
}

void CGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
	m_bLoadlingLevel = false;

	if (g_hCSBTEMapLoading.Get())
	{
		g_hCSBTEMapLoading->SetVisible(false);
		g_hCSBTEMapLoading->SetAutoDelete(true);
	}

	staticPanel->OnLevelLoadingFinished();
	baseuifuncs->HideGameUI();
}

void CGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
	if (g_hCSBTEMapLoading.Get())
	{
		if (g_hCSBTEMapLoading->IsVisible())
		{
			g_hCSBTEMapLoading->SetProgressRange(0, progressSteps);
			g_hCSBTEMapLoading->SetProgressPoint(0.0f);

			if (!stricmp(progressType, "Server"))
			{
				g_hCSBTEMapLoading->SetProgressVisible(false);
			}
			else if (!stricmp(progressType, "Connecting"))
			{
				g_hCSBTEMapLoading->SetProgressVisible(true);
			}

			return;
		}
	}

	if (!g_hLoadingDialog.Get())
		g_hLoadingDialog = new CLoadingDialog(staticPanel);

	m_szPreviousStatusText[0] = 0;
	g_hLoadingDialog->SetProgressRange(0, progressSteps);
	g_hLoadingDialog->SetProgressPoint(0.0f);
	g_hLoadingDialog->Open();
}

int CGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	if (g_hCSBTEMapLoading.Get())
	{
		if (g_hCSBTEMapLoading->IsVisible())
		{
			return g_hCSBTEMapLoading->SetProgressPoint(progressPoint);
		}
	}

	if (!g_hLoadingDialog.Get())
	{
		g_hLoadingDialog = new CLoadingDialog(staticPanel);
		g_hLoadingDialog->SetProgressRange(0, 24);
		g_hLoadingDialog->SetProgressPoint(0.0f);
		g_hLoadingDialog->Open();
	}

	g_hLoadingDialog->Activate();
	return g_hLoadingDialog->SetProgressPoint(progressPoint);
}

void CGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
	if (g_hCSBTEMapLoading.Get())
	{
		if (g_hCSBTEMapLoading->IsVisible())
		{
			return;
		}
	}

	if (!g_hLoadingDialog.Get() && bError)
		g_hLoadingDialog = new CLoadingDialog(staticPanel);

	if (!g_hLoadingDialog.Get())
		return;

	if (bError)
	{
		g_hLoadingDialog->DisplayGenericError(failureReason, extendedReason);
	}
	else
	{
		g_hLoadingDialog->Close();
		g_hLoadingDialog = NULL;
	}
}

int CGameUI::SetProgressBarStatusText(const char *statusText)
{
	if (g_hCSBTEMapLoading.Get())
	{
		if (g_hCSBTEMapLoading->IsVisible())
		{
			g_hCSBTEMapLoading->SetStatusText(statusText);
			return false;
		}
	}

	if (!g_hLoadingDialog.Get())
		return false;

	if (!statusText)
		return false;

	if (!stricmp(statusText, m_szPreviousStatusText))
		return false;

	g_hLoadingDialog->SetStatusText(statusText);
	Q_strncpy(m_szPreviousStatusText, statusText, sizeof(m_szPreviousStatusText));
	return true;
}

void CGameUI::SetSecondaryProgressBar(float progress)
{
	if (g_hCSBTEMapLoading.Get())
	{
		if (g_hCSBTEMapLoading->IsVisible())
			return;
	}

	if (!g_hLoadingDialog.Get())
		return;

	g_hLoadingDialog->SetSecondaryProgress(progress);
}

void CGameUI::SetSecondaryProgressBarText(const char *statusText)
{
	if (g_hCSBTEMapLoading.Get())
	{
		if (g_hCSBTEMapLoading->IsVisible())
			return;
	}

	if (!g_hLoadingDialog.Get())
		return;

	g_hLoadingDialog->SetSecondaryProgressText(statusText);
}

void CGameUI::ValidateCDKey(bool force, bool inConnect)
{
	
}

bool CGameUI::IsServerBrowserValid(void)
{
	return serverbrowser != NULL;
}

void CGameUI::ActivateServerBrowser(void)
{
	if (serverbrowser)
		serverbrowser->Activate();
}

bool CGameUI::IsInLevel(void)
{
	const char *levelName = engine->pfnGetLevelName();

	if (strlen(levelName) > 0)
		return true;

	return false;
}

bool CGameUI::IsInMultiplayer(void)
{
	return (IsInLevel() && engine->GetMaxClients() > 1);
}