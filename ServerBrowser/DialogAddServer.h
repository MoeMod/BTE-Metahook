#ifndef DIALOGADDSERVER_H
#define DIALOGADDSERVER_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/ListPanel.h>
#include "ServerList.h"
#include "IServerRefreshResponse.h"

class CBaseGamesPage;
class IGameList;

class CAddServerGameList : public vgui::ListPanel
{
public:
	DECLARE_CLASS_SIMPLE(CAddServerGameList, vgui::ListPanel);

public:
	CAddServerGameList(vgui::Panel *parent, const char *panelName);

public:
	virtual void OnKeyCodeTyped(vgui::KeyCode code);
};

class CDialogAddServer : public vgui::Frame, public IServerRefreshResponse
{
public:
	DECLARE_CLASS_SIMPLE(CDialogAddServer, vgui::Frame);

public:
	friend class CAddServerGameList;

public:
	CDialogAddServer(vgui::Panel *parent);
	~CDialogAddServer(void);

public:
	void StartRefresh(void);
	void StopRefresh(void);
	bool IsRefreshing(void);
	void AddNewServer(netadr_t address);
	void AddNewServer(serveritem_t &server);
	void ServerResponded(serveritem_t &server);
	void ServerFailedToRespond(serveritem_t &server);
	void RefreshComplete(void);
	void TestServers(void);

public:
	void ApplySchemeSettings(vgui::IScheme *pScheme);

public:
	MESSAGE_FUNC(OnItemSelected, "ItemSelected");

private:
	void OnCommand(const char *command);
	void OnOK(void);
	void OnTick(void);
	void OnKeyCodeTyped(vgui::KeyCode code);

	MESSAGE_FUNC(OnTextChanged, "TextChanged");

private:
	vgui::Button *m_pTestServersButton;
	vgui::Button *m_pAddServerButton;
	vgui::Button *m_pAddSelectedServerButton;

	vgui::PropertySheet *m_pTabPanel;
	vgui::TextEntry *m_pTextEntry;
	CAddServerGameList *m_pDiscoveredGames;
	int m_OriginalHeight;
	CServerList m_Servers;
	CUtlVector<serveritem_t> m_DiscoveredServers;
};

#endif