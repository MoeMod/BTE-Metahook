#include "HistoryGames.h"

#include "proto_oob.h"
#include "ServerContextMenu.h"
#include "ServerListCompare.h"
#include "Socket.h"
#include "ServerBrowserDialog.h"
#include "InternetGames.h"
#include "FileSystem.h"
#include "UtlBuffer.h"

#include <vgui/IScheme.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>
#include <KeyValues.h>

#include <vgui_controls/Button.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/MessageBox.h>

using namespace vgui;

CHistoryGames::CHistoryGames(vgui::Panel *parent) : CBaseGamesPage(parent, "HistoryGames")
{
	m_bRefreshOnListReload = false;
}

CHistoryGames::~CHistoryGames(void)
{
}

void CHistoryGames::OnPageShow(void)
{
	BaseClass::OnPageShow();
}

void CHistoryGames::OnPageHide(void)
{
	BaseClass::OnPageHide();
}

void CHistoryGames::LoadHistorysList(KeyValues *historysData)
{
	for (KeyValues *dat = historysData->GetFirstSubKey(); dat != NULL; dat = dat->GetNextKey())
	{
		serveritem_t server;
		memset(&server, 0, sizeof(server));

		const char *addr = dat->GetString("address");
		int ip1, ip2, ip3, ip4, port;
		sscanf(addr, "%d.%d.%d.%d:%d", &ip1, &ip2, &ip3, &ip4, &port);
		server.ip[0] = ip1;
		server.ip[1] = ip2;
		server.ip[2] = ip3;
		server.ip[3] = ip4;
		server.port = port;
		server.players = 0;
		Q_strncpy(server.name, dat->GetString("name"), sizeof(server.name));
		Q_strncpy(server.map, dat->GetString("map"), sizeof(server.map));
		Q_strncpy(server.gameDir, dat->GetString("gamedir"), sizeof(server.gameDir));
		server.players = dat->GetInt("players");
		server.maxPlayers = dat->GetInt("maxplayers");

		AddNewServer(server);
	}

	m_pGameList->SetEmptyListText("#ServerBrowser_NoServersPlayed");

	if (m_bRefreshOnListReload)
	{
		m_bRefreshOnListReload = false;
		StartRefresh();
	}
}

void CHistoryGames::SaveHistorysList(KeyValues *historysData)
{
	historysData->Clear();

	for (unsigned int i = 0; i < m_Servers.ServerCount(); i++)
	{
		serveritem_t &server = m_Servers.GetServer(i);

		if (server.doNotRefresh)
			continue;

		KeyValues *dat = historysData->CreateNewKey();

		dat->SetString("name", server.name);
		dat->SetString("gamedir", server.gameDir);
		dat->SetInt("players", server.players);
		dat->SetInt("maxplayers", server.maxPlayers);
		dat->SetString("map", server.map);

		char buf[64];
		sprintf(buf, "%d.%d.%d.%d:%d", server.ip[0], server.ip[1], server.ip[2], server.ip[3], server.port);
		dat->SetString("address", buf);
	}
}

bool CHistoryGames::SupportsItem(InterfaceItem_e item)
{
	switch (item)
	{
		case FILTERS: return true;
	}

	return false;
}

void CHistoryGames::StartRefresh(void)
{
	m_Servers.StopRefresh();

	for (unsigned int i = 0; i < m_Servers.ServerCount(); i++)
	{
		if (m_Servers.GetServer(i).doNotRefresh)
			continue;

		m_Servers.AddServerToRefreshList(i);
	}

	m_Servers.StartRefresh();

	SetRefreshing(IsRefreshing());
}

void CHistoryGames::GetNewServerList(void)
{
	StartRefresh();
}

void CHistoryGames::StopRefresh(void)
{
	m_Servers.StopRefresh();

	RefreshComplete();
}

bool CHistoryGames::IsRefreshing(void)
{
	return m_Servers.IsRefreshing();
}

void CHistoryGames::AddNewServer(serveritem_t &newServer)
{
	if (newServer.ip[0] == 127 && newServer.ip[1] == 0 && newServer.ip[2] == 0 && newServer.ip[3] == 1)
		return;

	for (unsigned int i = 0; i < m_Servers.ServerCount(); i++)
	{
		serveritem_t &server = m_Servers.GetServer(i);

		if (*(int *)server.ip == *(int *)newServer.ip && server.port == newServer.port)
			return;
	}

	unsigned int index = m_Servers.AddNewServer(newServer);

	serveritem_t &server = m_Servers.GetServer(index);
	server.hadSuccessfulResponse = true;
	server.doNotRefresh = false;
	server.listEntryID = GetInvalidServerListID();
	server.serverID = index;
}

void CHistoryGames::ListReceived(bool moreAvailable, int lastUnique)
{
	m_Servers.StartRefresh();
}

void CHistoryGames::OnBeginConnect(void)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

	StopRefresh();
	ServerBrowserDialog().JoinGame(this, serverID);
}

void CHistoryGames::OnViewGameInfo(void)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

	StopRefresh();
	ServerBrowserDialog().OpenGameInfoDialog(this, serverID);
}

void CHistoryGames::ServerFailedToRespond(serveritem_t &server)
{
	ServerResponded(server);
}

void CHistoryGames::RefreshComplete(void)
{
	SetRefreshing(false);

	m_pGameList->SetEmptyListText("#ServerBrowser_NoServersPlayed");
	m_pGameList->SortList();
}

void CHistoryGames::OnOpenContextMenu(int itemID)
{
	CServerContextMenu *menu = ServerBrowserDialog().GetContextMenu(m_pGameList);

	if (m_pGameList->GetSelectedItemsCount())
	{
		int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

		menu->ShowMenu(this, serverID, true, true, true, true);
		menu->AddMenuItem("RemoveServer", "#ServerBrowser_RemoveServerFromHistory", new KeyValues("RemoveFromHistory"), this);
	}
	else
		menu->ShowMenu(this, (uint32)-1, false, false, false, false);
}

void CHistoryGames::OnRefreshServer(int serverID)
{
	for (int i = 0; i < m_pGameList->GetSelectedItemsCount(); i++)
	{
		serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(i));

		m_Servers.AddServerToRefreshList(serverID);
	}

	m_Servers.StartRefresh();
	SetRefreshing(IsRefreshing());
}

void CHistoryGames::OnRemoveFromHistory(void)
{
	while (m_pGameList->GetSelectedItemsCount() > 0)
	{
		int itemID = m_pGameList->GetSelectedItem(0);
		unsigned int serverID = m_pGameList->GetItemData(itemID)->userData;

		if (serverID >= m_Servers.ServerCount())
			continue;

		serveritem_t &server = m_Servers.GetServer(serverID);

		if (m_pGameList->IsValidItemID(server.listEntryID))
		{
			m_pGameList->RemoveItem(server.listEntryID);
			server.listEntryID = GetInvalidServerListID();
		}

		server.doNotRefresh = true;
	}

	UpdateStatus();
	InvalidateLayout();
	Repaint();
}