#ifndef LANGAMES_H
#define LANGAMES_H

#ifdef _WIN32
#pragma once
#endif

#include "BaseGamesPage.h"

#include "IGameList.h"
#include "server.h"
#include "UtlVector.h"

class CLanBroadcastMsgHandler;

class CLanGames : public CBaseGamesPage
{
	DECLARE_CLASS_SIMPLE(CLanGames, CBaseGamesPage);

public:
	CLanGames(vgui::Panel *parent, bool bAutoRefresh = true, const char *pCustomResFilename = NULL);
	~CLanGames(void);

public:
	virtual void OnPageShow(void);
	virtual void OnPageHide(void);
	virtual void OnTick(void);
	virtual void OnBeginConnect(void);
	virtual void OnViewGameInfo(void);
	virtual void OnRefreshServer(int serverID);

public:
	virtual bool SupportsItem(InterfaceItem_e item);

public:
	void ManualShowButtons(bool bShowConnect, bool bShowRefreshAll, bool bShowFilter);
	void InternalGetNewServerList(CUtlVector<netadr_t> *pSpecificAddresses);

public:
	virtual void StartRefresh(void);
	virtual void StopRefresh(void);
	virtual void GetNewServerList(void);
	virtual bool IsRefreshing(void);
	virtual void AddNewServer(serveritem_t &server);
	virtual void ListReceived(bool moreAvailable, int lastUnique);
	virtual void ServerFailedToRespond(serveritem_t &server);
	virtual void RefreshComplete(void);
	virtual void CheckRetryRequest(void);

private:
	MESSAGE_FUNC_INT(OnOpenContextMenu, "OpenContextMenu", itemID);

private:
	int m_iServerRefreshCount;
	bool m_bRequesting;
	double m_fRequestTime;
	CSocket *m_pBroadcastSocket;
	bool m_bAutoRefresh;
	CLanBroadcastMsgHandler *m_pLanBroadcastMsgHandler;
};

#endif