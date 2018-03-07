#include "LanGames.h"

#include "LanBroadcastMsgHandler.h"
#include "proto_oob.h"
#include "ServerContextMenu.h"
#include "ServerListCompare.h"
#include "Socket.h"
#include "ServerBrowserDialog.h"
#include "InternetGames.h"

#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>

#include <vgui_controls/Button.h>
#include <vgui_controls/ToggleButton.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/ListPanel.h>

using namespace vgui;

const float BROADCAST_LIST_TIMEOUT = 0.4f;

CLanGames::CLanGames(vgui::Panel *parent, bool bAutoRefresh, const char *pCustomResFilename) : CBaseGamesPage(parent, "LanGames", pCustomResFilename)
{
	m_iServerRefreshCount = 0;
	m_bRequesting = false;
	m_bAutoRefresh = bAutoRefresh;

	m_pBroadcastSocket = NULL;
}

CLanGames::~CLanGames(void)
{
}

void CLanGames::OnPageShow(void)
{
	if (!m_pBroadcastSocket)
	{
		m_pBroadcastSocket = new CSocket("lan broadcast", -1);
		m_pLanBroadcastMsgHandler = new CLanBroadcastMsgHandler(this, CMsgHandler::MSGHANDLER_BYTECODE, S2A_INFO_GOLDSRC);
		m_pBroadcastSocket->AddMessageHandler(m_pLanBroadcastMsgHandler);
	}

	BaseClass::OnPageShow();
}

void CLanGames::OnPageHide(void)
{
	BaseClass::OnPageHide();
}

void CLanGames::OnTick(void)
{
	BaseClass::OnTick();

	if (IsActivated())
	{
		m_pBroadcastSocket->Frame();

		CheckRetryRequest();
	}
}

bool CLanGames::SupportsItem(InterfaceItem_e item)
{
	switch (item)
	{
		case FILTERS: return true;
	}

	return false;
}

void CLanGames::StartRefresh(void)
{
	m_Servers.StopRefresh();

	GetNewServerList();
	SetRefreshing(true);
}

void CLanGames::ManualShowButtons(bool bShowConnect, bool bShowRefreshAll, bool bShowFilter)
{
	m_pConnect->SetVisible(bShowConnect);
	m_pRefreshAll->SetVisible(bShowRefreshAll);
	m_pFilter->SetVisible(bShowFilter);
}

void CLanGames::GetNewServerList(void)
{
	m_Servers.Clear();
	m_pGameList->DeleteAllItems();
	m_bRequesting = true;
	SetRefreshing(true);

	CMsgBuffer *buffer = m_pBroadcastSocket->GetSendBuffer();
	buffer->Clear();
	buffer->WriteLong(0xFFFFFFFF);
	buffer->WriteByte(A2S_INFO);
	buffer->WriteString(A2S_KEY_STRING);

	for (int i = 27015; i <= 27020; i++)
	{
		m_pBroadcastSocket->Broadcast(27015);
		m_pBroadcastSocket->Sleep(1);
	}

#ifdef _DEBUG
	m_pBroadcastSocket->Broadcast(4242);
#endif
	m_fRequestTime = CSocket::GetClock();
	m_pLanBroadcastMsgHandler->SetRequestTime(m_fRequestTime);
}

void CLanGames::StopRefresh(void)
{
	m_Servers.StopRefresh();

	m_iServerRefreshCount = 0;
	m_bRequesting = false;

	RefreshComplete();
}

bool CLanGames::IsRefreshing(void)
{
	return m_Servers.IsRefreshing() || m_bRequesting;
}

void CLanGames::AddNewServer(serveritem_t &newServer)
{
	unsigned int index = m_Servers.AddNewServer(newServer);

	serveritem_t &server = m_Servers.GetServer(index);
	server.hadSuccessfulResponse = true;
	server.doNotRefresh = false;

	server.listEntryID = GetInvalidServerListID();
	server.serverID = index;

	m_Servers.AddServerToRefreshList(server.serverID);
}

void CLanGames::ListReceived(bool moreAvailable, int lastUnique)
{
	m_Servers.StartRefresh();
	m_bRequesting = false;
	m_iServerRefreshCount = 0;

	SetRefreshing(IsRefreshing());
}

void CLanGames::OnBeginConnect(void)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

	StopRefresh();
	ServerBrowserDialog().JoinGame(this, serverID);
}

void CLanGames::OnViewGameInfo(void)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

	StopRefresh();
	ServerBrowserDialog().OpenGameInfoDialog(this, serverID);
}

void CLanGames::CheckRetryRequest(void)
{
	if (!m_bRequesting)
		return;

	float curtime = CSocket::GetClock();

	if (curtime - m_fRequestTime <= BROADCAST_LIST_TIMEOUT)
		return;

	m_bRequesting = false;
	m_pGameList->SetEmptyListText("#ServerBrowser_NoLanServers");

	ListReceived(false, 0);
}

void CLanGames::ServerFailedToRespond(serveritem_t &server)
{
	if (m_pGameList->IsValidItemID(server.listEntryID))
	{
		m_pGameList->RemoveItem(server.listEntryID);
		server.listEntryID = GetInvalidServerListID();
	}
}

void CLanGames::RefreshComplete(void)
{
	SetRefreshing(false);
	m_pGameList->SortList();
	m_iServerRefreshCount = 0;
	m_pGameList->SetEmptyListText("#ServerBrowser_NoLanServers");
}

void CLanGames::OnOpenContextMenu(int row)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

	CServerContextMenu *menu = ServerBrowserDialog().GetContextMenu(m_pGameList);
	menu->ShowMenu(this, serverID, true, true, true, false);
}

void CLanGames::OnRefreshServer(int serverID)
{
	for (int i = 0; i < m_pGameList->GetSelectedItemsCount(); i++)
	{
		int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(i));
		m_Servers.AddServerToRefreshList(serverID);
	}

	m_Servers.StartRefresh();
	SetRefreshing(IsRefreshing());
}