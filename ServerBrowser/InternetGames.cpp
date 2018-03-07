#include "EngineInterface.h"
#include "INetApi.h"
#include "InternetGames.h"
#include "MasterMsgHandler.h"
#include "proto_oob.h"
#include "ServerContextMenu.h"
#include "ServerListCompare.h"
#include "Socket.h"
#include "ServerBrowserDialog.h"

#include <stdio.h>

#include <vgui/ILocalize.h>
#include <vgui/IInput.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/MouseCode.h>
#include <KeyValues.h>

#include <vgui_controls/Menu.h>
#include <vgui_controls/ComboBox.h>

#define NOMASTER_TEST 0

using namespace vgui;

const int NUMBER_OF_RETRIES = 3;
const float MASTER_LIST_TIMEOUT = 3.0f;
const float MINIMUM_SORT_TIME = 1.5f;
const int MAXIMUM_SERVERS = 10000;

CInternetGames::CInternetGames(vgui::Panel *parent, bool bAutoRefresh, const char *panelName) : CBaseGamesPage(parent, panelName)
{
	KeyValues *kv;

	m_nStartPoint = 0;
	m_fLastSort = 0.0f;
	m_bDirty = false;
	m_nRetriesRemaining = NUMBER_OF_RETRIES;
	m_fRequestTime = 0.0f;
	m_bRequesting = false;
	m_nLastRequest = 0;
	m_bRequireUpdate = true;
	m_bMoreUpdatesAvailable = false;
	m_iServerRefreshCount = 0;
	m_bAutoRefresh = bAutoRefresh;
	m_pMaster = NULL;

	m_pLocationFilter->DeleteAllItems();

	kv = new KeyValues("Regions");

	if (kv->LoadFromFile(g_pFullFileSystem, "servers/Regions.vdf", NULL))
	{
		for (KeyValues *srv = kv->GetFirstSubKey(); srv != NULL; srv = srv->GetNextKey())
		{
			struct regions_s region;

			region.name = srv->GetString("text");
			region.code = srv->GetInt("code");
			KeyValues *regionKV = new KeyValues("region", "code", region.code);
			m_pLocationFilter->AddItem(region.name.String(), regionKV);
			regionKV->deleteThis();
			m_Regions.AddToTail(region);
		}
	}
	else
		Assert(!("Could not load file servers/Regions.vdf; server browser will not function."));

	kv->deleteThis();

#if NOMASTER_TEST == 0
	kv = new KeyValues("MasterServers");

	if (kv->LoadFromFile(g_pFullFileSystem, "servers/MasterServers.vdf"))
	{
		for (KeyValues *srv = kv->GetFirstSubKey(); srv != NULL; srv = srv->GetNextKey())
		{
			const char *addr = srv->GetString("addr");
			m_MasterServerNames.AddToTail(addr);
		}
	}
	else
		Assert(!("Could not load file servers/MasterServers.vdf; server browser will not function."));

	kv->deleteThis();

	if (m_MasterServerNames.Count() < 1)
		m_MasterServerNames.AddToTail(VALVE_MASTER_ADDRESS);

	int serverIndex = engine->pfnRandomLong(0, m_MasterServerNames.Count() - 1);
	net->StringToAdr(m_MasterServerNames[serverIndex].String(), &m_MasterAddress);
#endif
	LoadFilterSettings();
}

CInternetGames::~CInternetGames(void)
{
	if (m_pMaster)
		delete m_pMaster;
}

void CInternetGames::PerformLayout(void)
{
	if (m_bRequireUpdate && ServerBrowserDialog().IsVisible())
	{
		PostMessage(this, new KeyValues("GetNewServerList"), 0.1f);
		m_bRequireUpdate = false;
	}

	BaseClass::PerformLayout();

	m_pLocationFilter->SetEnabled(true);
}

void CInternetGames::OnPageShow(void)
{
	if (!ServerBrowserDialog().IsVisible())
		return;

	if (m_bAutoRefresh)
		StartRefresh();
}

void CInternetGames::OnPageHide(void)
{
	BaseClass::OnPageHide();
}

void CInternetGames::OnTick(void)
{
	BaseClass::OnTick();

	if (IsActivated())
	{
		if (m_pMaster)
			m_pMaster->Frame();

		if (m_Servers.IsRefreshing() && m_Servers.RefreshListRemaining() < 1)
		{
			if (m_bMoreUpdatesAvailable && !m_bRequesting)
				RequestServers(m_nLastUnique, GetFilterString());
		}

		CheckRedoSort();
		CheckRetryRequest();
	}
}

void CInternetGames::ServerResponded(serveritem_t &server)
{
	m_bDirty = true;
	BaseClass::ServerResponded(server);

	m_iServerRefreshCount++;

	if (!m_bMoreUpdatesAvailable && m_pGameList->GetItemCount() > 0)
	{
		wchar_t unicode[128], unicodePercent[6];
		char tempPercent[6];
		int percentDone = (m_iServerRefreshCount * 100) / m_pGameList->GetItemCount();

		if (percentDone < 0)
			percentDone = 0;
		else if (percentDone > 99)
			percentDone = 99;

		itoa(percentDone, tempPercent, 10);
		localize()->ConvertANSIToUnicode(tempPercent, unicodePercent, sizeof(unicodePercent));
		localize()->ConstructString(unicode, sizeof(unicode), localize()->Find("#ServerBrowser_RefreshingPercentDone"), 1, unicodePercent);
		ServerBrowserDialog().UpdateStatusText(unicode);
	}
}

void CInternetGames::ServerFailedToRespond(serveritem_t &server)
{
	m_bDirty = true;

	if (server.hadSuccessfulResponse)
	{
		ServerResponded(server);
	}
	else
	{
		RemoveServer(server);
		m_iServerRefreshCount++;
	}
}

void CInternetGames::RefreshComplete(void)
{
	if (!m_bMoreUpdatesAvailable)
		SetRefreshing(false);

	UpdateFilterSettings();

	if (!m_bRequesting)
	{
		if (m_bMoreUpdatesAvailable)
		{
			RequestServers(m_nLastUnique, GetFilterString());
		}
		else
		{
			m_nStartPoint = 0;
			m_iServerRefreshCount = 0;

			if (m_Servers.RefreshListRemaining() == 0)
			{
#if 0
				m_pGameList->SetEmptyListText("#ServerBrowser_MasterServerHasNoServersListed");
#else
				m_pGameList->SetEmptyListText("#ServerBrowser_NoInternetGamesResponded");
#endif
			}
			else if (m_pGameList->GetItemCount() == 0)
				m_pGameList->SetEmptyListText("#ServerBrowser_NoInternetGamesResponded");
			else
				m_pGameList->SetEmptyListText("#ServerBrowser_NoInternetGames");

			m_bDirty = false;
			m_fLastSort = m_pMaster->GetClock();

			if (IsVisible())
				m_pGameList->SortList();
		}
	}

	UpdateStatus();
}

void CInternetGames::GetNewServerList(void)
{
	if (!m_pMaster)
	{
		m_pMaster = new CSocket("internet dialog master", -1);
		m_pMaster->AddMessageHandler(new CMasterMsgHandler(this, CMsgHandler::MSGHANDLER_BYTECODE, M2A_SERVER_BATCH));
	}

	m_Servers.Clear();

	UpdateStatus();

	m_pGameList->DeleteAllItems();

	m_bRequireUpdate = false;
	m_iServerRefreshCount = 0;

	RequestServers(0, GetFilterString());
}

void CInternetGames::AddNewServer(serveritem_t &server)
{
	unsigned int index = m_Servers.AddNewServer(server);

	m_Servers.AddServerToRefreshList(index);
}

bool CInternetGames::SupportsItem(IGameList::InterfaceItem_e item)
{
	switch (item)
	{
		case FILTERS:
		case GETNEWLIST: return true;
	}

	return false;
}

void CInternetGames::CheckRedoSort(void)
{
	float curtime;

	if (!m_bDirty)
		return;

	curtime = m_pMaster->GetClock();

	if (curtime - m_fLastSort < MINIMUM_SORT_TIME)
		return;

	if (input()->IsMouseDown(MOUSE_LEFT) || input()->IsMouseDown(MOUSE_RIGHT))
	{
		m_fLastSort = curtime - MINIMUM_SORT_TIME + 1.0f;
		return;
	}

	m_bDirty = false;
	m_fLastSort = curtime;
	m_pGameList->SortList();
}

void CInternetGames::CheckRetryRequest(void)
{
	if (!m_bRequesting)
		return;

	float curtime = m_pMaster->GetClock();

	if (curtime - m_fRequestTime <= MASTER_LIST_TIMEOUT)
		return;

	if (--m_nRetriesRemaining <= 0)
	{
		StopRefresh();

		m_pGameList->SetEmptyListText("#ServerBrowser_MasterServerNotResponsive");
		return;
	}

	int serverIndex = engine->pfnRandomLong(0, m_MasterServerNames.Count() - 1);
	net->StringToAdr(m_MasterServerNames[serverIndex].String(), &m_MasterAddress);

	RequestServers(m_nLastRequest, GetFilterString());
}

void CInternetGames::RemoveServer(serveritem_t &server)
{
	if (m_pGameList->IsValidItemID(server.listEntryID))
		m_pGameList->SetItemVisible(server.listEntryID, false);

	UpdateStatus();
}

void CInternetGames::StartRefresh(void)
{
	if (m_pGameList->GetItemCount() < 1 || m_bRequireUpdate)
	{
		GetNewServerList();
	}
	else
	{
		m_Servers.StopRefresh();

		for (int i = 0; i < m_pGameList->GetItemCount(); i++)
		{
			int serverID = m_pGameList->GetItemUserData(m_pGameList->GetItemIDFromRow(i));
			m_Servers.AddServerToRefreshList(serverID);
		}

		ContinueRefresh();
	}

	m_iServerRefreshCount = 0;
}

void CInternetGames::ContinueRefresh(void)
{
	m_Servers.StartRefresh();
	SetRefreshing(true);

	if (m_bMoreUpdatesAvailable)
		ServerBrowserDialog().UpdateStatusText("#ServerBrowser_GettingNewServerList");
}

void CInternetGames::ListReceived(bool moreAvailable, int lastUnique)
{
	if (!m_bRequesting)
		return;

	m_bRequesting = false;
	m_nRetriesRemaining = NUMBER_OF_RETRIES;

	if (m_Servers.ServerCount() < MAXIMUM_SERVERS)
	{
		m_bMoreUpdatesAvailable = moreAvailable;
		m_nLastUnique = lastUnique;
	}
	else
	{
		m_bMoreUpdatesAvailable = false;
		m_nLastUnique = 0;
	}

	ContinueRefresh();
}

bool CInternetGames::IsRefreshing(void)
{
	return (m_Servers.IsRefreshing() || m_bRequesting);
}

void CInternetGames::StopRefresh(void)
{
	m_Servers.StopRefresh();

	m_nStartPoint = 0;
	m_bRequesting = false;
	m_bRequireUpdate = false;
	m_bMoreUpdatesAvailable = false;
	m_nLastUnique = 0;
	m_iServerRefreshCount = 0;

	RefreshComplete();
}

void CInternetGames::OnBeginConnect(void)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

	StopRefresh();
	ServerBrowserDialog().JoinGame(this, serverID);
}

void CInternetGames::OnViewGameInfo(void)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

	StopRefresh();
	ServerBrowserDialog().OpenGameInfoDialog(this, serverID);
}

void CInternetGames::RequestServers(int Start, const char *filterString)
{
	CMsgBuffer *buffer = m_pMaster->GetSendBuffer();
	Assert(buffer);

	if (!buffer)
		return;

	m_bRequesting = true;

	SetRefreshing(true);
	ServerBrowserDialog().UpdateStatusText("#ServerBrowser_GettingNewServerList");

	m_fRequestTime = m_pMaster->GetClock();
	m_nLastRequest = Start;

	buffer->Clear();
	buffer->WriteByte(A2M_GET_SERVERS_BATCH2);
	buffer->WriteLong(Start);
	buffer->WriteString(filterString);

	m_pMaster->SendMessage(&m_MasterAddress);
}

void CInternetGames::OnOpenContextMenu(int itemID)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemData(m_pGameList->GetSelectedItem(0))->userData;

	CServerContextMenu *menu = ServerBrowserDialog().GetContextMenu(m_pGameList);
	menu->ShowMenu(this, serverID, true, true, true, true);
}

void CInternetGames::OnRefreshServer(int serverID)
{
	for (int i = 0; i < m_pGameList->GetSelectedItemsCount(); i++)
	{
		serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(i));
		m_Servers.AddServerToRefreshList(serverID);
	}

	ContinueRefresh();

	ServerBrowserDialog().UpdateStatusText("#ServerBrowser_GettingNewServerList");
}

void CInternetGames::OnAddToFavorites(void)
{
	for (int i = 0; i < m_pGameList->GetSelectedItemsCount(); i++)
	{
		unsigned int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(i));

		if (serverID >= m_Servers.ServerCount())
			continue;

		serveritem_t &server = m_Servers.GetServer(serverID);
		ServerBrowserDialog().AddServerToFavorites(server);
	}
}

int CInternetGames::GetRegionCodeToFilter(void)
{
	KeyValues *kv = m_pLocationFilter->GetActiveItemUserData();

	if (kv)
		return kv->GetInt("code");
	else
		return 255;
}