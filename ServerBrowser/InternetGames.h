#ifndef INTERNETGAMES_H
#define INTERNETGAMES_H

#ifdef _WIN32
#pragma once
#endif

#include "BaseGamesPage.h"

class CInternetGames : public CBaseGamesPage
{
	DECLARE_CLASS_SIMPLE(CInternetGames, CBaseGamesPage);

public:
	CInternetGames(vgui::Panel *parent, bool bAutoRefresh = true, const char *panelName = "InternetGames");
	~CInternetGames(void);

public:
	virtual void OnPageShow(void);
	virtual void OnPageHide(void);
	virtual void OnBeginConnect(void);
	virtual void OnViewGameInfo(void);

	MESSAGE_FUNC_INT(OnRefreshServer, "RefreshServer", serverID);

public:
	virtual bool SupportsItem(IGameList::InterfaceItem_e item);
	virtual void StartRefresh(void);
	virtual void StopRefresh(void);
	virtual bool IsRefreshing(void);
	virtual void AddNewServer(serveritem_t &server);
	virtual void ListReceived(bool moreAvailable, int lastUnique);
	virtual void ServerResponded(serveritem_t &server);
	virtual void ServerFailedToRespond(serveritem_t &server);
	virtual void RefreshComplete(void);
	virtual int GetRegionCodeToFilter(void);

	MESSAGE_FUNC(GetNewServerList, "GetNewServerList");

public:
	virtual void OnTick(void);

protected:
	virtual void PerformLayout(void);
	virtual void RequestServers(int Start, const char *filterString);

private:
	void CheckRedoSort(void);
	void CheckRetryRequest(void);
	void RemoveServer(serveritem_t &server);
	void ContinueRefresh(void);
	void OnAddToFavorites(void);

	MESSAGE_FUNC_INT(OnOpenContextMenu, "OpenContextMenu", itemID);

private:
	CSocket *m_pMaster;
	CUtlVector<CUtlSymbol> m_MasterServerNames;
	netadr_t m_MasterAddress;

	struct regions_s
	{
		CUtlSymbol name;
		unsigned char code;
	};

	CUtlVector<struct regions_s> m_Regions;

	int m_nStartPoint;
	float m_fLastSort;
	bool m_bDirty;
	int m_nRetriesRemaining;
	float m_fRequestTime;
	bool m_bRequesting;
	int m_nLastRequest;
	bool m_bRequireUpdate;
	bool m_bMoreUpdatesAvailable;
	int m_nLastUnique;
	int m_iServerRefreshCount;
	bool m_bAutoRefresh;
};

#endif