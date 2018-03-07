#ifndef FAVORITEGAMES_H
#define FAVORITEGAMES_H

#ifdef _WIN32
#pragma once
#endif

#include "BaseGamesPage.h"

#include "IGameList.h"
#include "IServerRefreshResponse.h"
#include "server.h"

class CFavoriteGames : public CBaseGamesPage
{
	DECLARE_CLASS_SIMPLE(CFavoriteGames, CBaseGamesPage);

public:
	CFavoriteGames(vgui::Panel *parent);
	~CFavoriteGames(void);

public:
	void LoadFavoritesList(KeyValues *favoritesData);
	void SaveFavoritesList(KeyValues *favoritesData);

public:
	virtual void OnBeginConnect(void);
	virtual void OnViewGameInfo(void);

public:
	void OnConnectToGame(void);
	void OnDisconnectFromGame(void);
	void OnPageShow(void);
	void OnPageHide(void);

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
	MESSAGE_FUNC_INT(OnOpenContextMenu, "OpenContextMenu", itemID);
	MESSAGE_FUNC(OnRemoveFromFavorites, "RemoveFromFavorites");
	MESSAGE_FUNC(OnAddServerByName, "AddServerByName");

private:
	void OnRefreshServer(int serverID);
	void OnAddCurrentServer(void);
	void OnCommand(const char *command);

private:
	bool m_bRefreshOnListReload;
};

#endif