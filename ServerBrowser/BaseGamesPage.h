#ifndef BASEGAMESPAGE_H
#define BASEGAMESPAGE_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/ListPanel.h>
#include <vgui_controls/PropertyPage.h>
#include "ServerList.h"
#include "IServerRefreshResponse.h"
#include "server.h"
#include "IGameList.h"

class CBaseGamesPage;

class CGameListPanel : public vgui::ListPanel
{
public:
	DECLARE_CLASS_SIMPLE(CGameListPanel, vgui::ListPanel);

public:
	CGameListPanel(CBaseGamesPage *pOuter, const char *pName);

public:
	virtual void OnKeyCodeTyped(vgui::KeyCode code);

private:
	CBaseGamesPage *m_pOuter;
};

class CBaseGamesPage : public vgui::PropertyPage, public IServerRefreshResponse, public IGameList
{
	DECLARE_CLASS_SIMPLE(CBaseGamesPage, vgui::PropertyPage);

public:
	CBaseGamesPage(vgui::Panel *parent, const char *name, const char *pCustomResFilename = NULL);
	~CBaseGamesPage(void);

public:
	virtual void PerformLayout(void);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

public:
	virtual serveritem_t &GetServer(unsigned int serverID);
	virtual const char *GetFilterString(void);
	virtual void SetRefreshing(bool state);
	virtual void LoadFilterSettings(void);
	virtual void StartRefresh(void);
	virtual void UpdateDerivedLayouts(void);
	virtual bool OnGameListEnterPressed(void);

public:
	int GetSelectedItemsCount(void);

public:
	MESSAGE_FUNC(OnAddToFavorites, "AddToFavorites");

protected:
	virtual void OnCommand(const char *command);
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void OnSaveFilter(KeyValues *filter);
	virtual void OnLoadFilter(KeyValues *filter);
	virtual void OnPageShow(void);
	virtual void OnPageHide(void);
	virtual void OnTick(void);

protected:
	MESSAGE_FUNC(OnItemSelected, "ItemSelected");
	MESSAGE_FUNC(OnBeginConnect, "ConnectToServer");
	MESSAGE_FUNC(OnViewGameInfo, "ViewGameInfo");
	MESSAGE_FUNC_INT(OnRefreshServer, "RefreshServer", serverID);
	MESSAGE_FUNC_PTR_CHARPTR(OnTextChanged, "TextChanged", panel, text);
	MESSAGE_FUNC_PTR_INT(OnButtonToggled, "ButtonToggled", panel, state);

protected:
	virtual int GetRegionCodeToFilter(void) { return -1; }
	virtual void ServerResponded(serveritem_t &server);
	virtual bool CheckPrimaryFilters(serveritem_t &server);
	virtual bool CheckSecondaryFilters(serveritem_t &server);
	virtual int GetInvalidServerListID(void);
	virtual void UpdateFilterSettings(void);
	virtual void GetNewServerList(void);
	virtual void StopRefresh(void);
	virtual bool IsRefreshing(void);
	virtual void CreateFilters(void);
	virtual void ApplyFilters(void);
	virtual void UpdateGameFilter(void);
	virtual bool IsActivated(void);

protected:
	void ApplyGameFilters(void);
	void UpdateStatus(void);
	void ClearServerList(void);

protected:
	CGameListPanel *m_pGameList;
	CServerList m_Servers;
	vgui::ComboBox *m_pLocationFilter;

	vgui::Button *m_pConnect;
	vgui::Button *m_pRefreshAll;
	vgui::Button *m_pRefreshQuick;
	vgui::Button *m_pAddServer;
	vgui::Button *m_pAddCurrentServer;
	vgui::Button *m_pAddToFavoritesButton;
	vgui::ToggleButton *m_pFilter;

	int m_iServerRefreshCount;

private:
	void RecalculateFilterString(void);

private:
	const char *m_pCustomResFilename;

	vgui::ComboBox *m_pGameFilter;
	vgui::TextEntry *m_pMapFilter;
	vgui::ComboBox *m_pPingFilter;
	vgui::ComboBox *m_pSecureFilter;
	vgui::CheckButton *m_pNoFullServersFilterCheck;
	vgui::CheckButton *m_pNoEmptyServersFilterCheck;
	vgui::CheckButton *m_pNoPasswordFilterCheck;
	vgui::Label *m_pFilterString;
	char m_szComboAllText[64];

	KeyValues *m_pFilters;
	bool m_bFiltersVisible;
	vgui::HFont m_hFont;

	char m_szGameFilter[32];
	char m_szMapFilter[32];
	int m_iPingFilter;
	bool m_bFilterNoFullServers;
	bool m_bFilterNoEmptyServers;
	bool m_bFilterNoPasswordedServers;

	enum
	{
		MAX_FILTERSTRING = 512
	};

	char m_szMasterServerFilterString[MAX_FILTERSTRING];
};

#endif
