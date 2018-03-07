#ifndef HISTORYGAMES_H
#define HISTORYGAMES_H

#ifdef _WIN32
#pragma once
#endif

#include "BaseGamesPage.h"

#include "IGameList.h"
#include "IServerRefreshResponse.h"
#include "server.h"

class CHistoryGames : public CBaseGamesPage
{
	DECLARE_CLASS_SIMPLE(CHistoryGames, CBaseGamesPage);

public:
	CHistoryGames(vgui::Panel *parent);
	~CHistoryGames(void);

public:
	void LoadHistorysList(KeyValues *historysData);
	void SaveHistorysList(KeyValues *historysData);

public:
	virtual void OnPageShow(void);
	virtual void OnPageHide(void);
	virtual void OnBeginConnect(void);
	virtual void OnViewGameInfo(void);

public:
	virtual bool SupportsItem(InterfaceItem_e item);
	virtual void StartRefresh(void);
	virtual void GetNewServerList(void);
	virtual void StopRefresh(void);
	virtual bool IsRefreshing(void);
	virtual void AddNewServer(serveritem_t &server);
	virtual void ListReceived(bool moreAvailable, int lastUnique);
	virtual void ServerFailedToRespond(serveritem_t &server);
	virtual void RefreshComplete(void);

public:
	void SetRefreshOnReload(void) { m_bRefreshOnListReload = true; }

private:
	void OnRefreshServer(int serverID);

private:
	MESSAGE_FUNC_INT(OnOpenContextMenu, "OpenContextMenu", itemID);
	MESSAGE_FUNC(OnRemoveFromHistory, "RemoveFromHistory");

private:
	bool m_bRefreshOnListReload;
};

#endif