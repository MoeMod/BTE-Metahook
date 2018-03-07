#include "ServerBrowser.h"
#include "ServerBrowserDialog.h"
#include "DialogGameInfo.h"

#include <vgui/ILocalize.h>
#include <vgui/IPanel.h>
#include <vgui/IVGui.h>
#include <KeyValues.h>

CServerBrowser g_ServerBrowserSingleton;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CServerBrowser, IServerBrowser, SERVERBROWSER_INTERFACE_VERSION, g_ServerBrowserSingleton);

CServerBrowser &ServerBrowser(void)
{
	return g_ServerBrowserSingleton;
}

CServerBrowser::CServerBrowser(void)
{
}

CServerBrowser::~CServerBrowser(void)
{
}

void CServerBrowser::CreateDialog(void)
{
	if (!m_hInternetDlg.Get())
	{
		m_hInternetDlg = new CServerBrowserDialog(NULL);
		m_hInternetDlg->Initialize();
	}
}

bool CServerBrowser::Initialize(CreateInterfaceFn *factorylist, int factoryCount)
{
	//if (!vgui::VGui_InitInterfacesList("GameUI", factorylist, factoryCount))
	//	return false;

	vgui::localize()->AddFile(g_pFullFileSystem, "servers/serverbrowser_%language%.txt");

	CreateDialog();
	return true;
}

void CServerBrowser::ActiveGameName(const char *szGameName, const char *szGameDir)
{
	vgui::VPANEL panel = ServerBrowserDialog().GetVPanel();
	vgui::ivgui()->PostMessage(panel, new KeyValues("ActiveGameName", "game", szGameName, "name", szGameDir), panel, 0.0);
}

void CServerBrowser::ConnectToGame(int ip, int connectionport)
{
	vgui::VPANEL panel = ServerBrowserDialog().GetVPanel();
	vgui::ivgui()->PostMessage(panel, new KeyValues("ConnectedToGame", "ip", ip, "connectionport", connectionport), panel, 0.0);
}

void CServerBrowser::DisconnectFromGame(void)
{
	vgui::VPANEL panel = ServerBrowserDialog().GetVPanel();
	vgui::ivgui()->PostMessage(panel, new KeyValues("DisconnectedFromGame"), panel, 0.0);
}

bool CServerBrowser::Activate(void)
{
	m_hInternetDlg->Open();
	return true;
}

void CServerBrowser::Deactivate(void)
{
	if (m_hInternetDlg.Get())
		m_hInternetDlg->SaveUserData();
}

void CServerBrowser::Reactivate(void)
{
	if (m_hInternetDlg.Get())
	{
		m_hInternetDlg->LoadUserData();

		if (m_hInternetDlg->IsVisible())
			m_hInternetDlg->RefreshCurrentPage();
	}
}

vgui::VPANEL CServerBrowser::GetPanel(void)
{
	return m_hInternetDlg.Get() ? m_hInternetDlg->GetVPanel() : NULL;
}

void CServerBrowser::SetParent(vgui::VPANEL parent)
{
	if (m_hInternetDlg.Get())
		m_hInternetDlg->SetParent(parent);
}

void CServerBrowser::Shutdown(void)
{
	if (m_hInternetDlg.Get())
	{
		m_hInternetDlg->Close();
		m_hInternetDlg->MarkForDeletion();
	}
}

bool CServerBrowser::JoinGame(unsigned int gameIP, unsigned int gamePort, const char *userName)
{
	m_hInternetDlg->JoinGame(gameIP, gamePort, userName);
	return true;
}

void CServerBrowser::CloseAllGameInfoDialogs(void)
{
	if (m_hInternetDlg.Get())
		m_hInternetDlg->CloseAllGameInfoDialogs();
}