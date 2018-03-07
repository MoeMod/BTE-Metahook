#ifndef DIALOGGAMEINFO_H
#define DIALOGGAMEINFO_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/ListPanel.h>
#include "ServerList.h"
#include "getchallenge.h"
#include "playerinfo.h"
#include "IServerRefreshResponse.h"
#include "IServerGetChallengeResponse.h"
#include "IServerPlayersResponse.h"

class CDialogGameInfo : public vgui::Frame, public IServerRefreshResponse, public IServerGetChallengeResponse, public IServerPlayersResponse
{
	DECLARE_CLASS_SIMPLE(CDialogGameInfo, vgui::Frame); 

public:
	CDialogGameInfo(vgui::Panel *parent, serveritem_t &server);
	~CDialogGameInfo(void);

public:
	void Run(const char *titleName);
	void ChangeGame(int serverIP, int serverPort);
	void Connect(void);
	void ConnectToServer(void);

public:
	virtual void ServerResponded(serveritem_t &server);
	virtual void ServerFailedToRespond(serveritem_t &server);
	virtual void GetChallengeFailedToRespond(void);
	virtual void GetChallengeRefreshComplete(int challenge);
	virtual void AddPlayerToList(const char *playerName, int score, float timePlayedSeconds);
	virtual void PlayersFailedToRespond(void);
	virtual void PlayersRefreshComplete(void);
	virtual void RefreshComplete(void);
	virtual void ClearPlayerList(void);
	virtual void SendPlayerQuery(void);

protected:
	MESSAGE_FUNC(OnConnect, "Connect");
	MESSAGE_FUNC(OnRefresh, "Refresh");
	MESSAGE_FUNC_PTR(OnButtonToggled, "ButtonToggled", panel);
	MESSAGE_FUNC_PTR(OnRadioButtonChecked, "RadioButtonChecked", panel)
	{
		OnButtonToggled(panel);
	}

	MESSAGE_FUNC_CHARPTR(OnJoinServerWithPassword, "JoinServerWithPassword", password);
	MESSAGE_FUNC_INT_INT(OnConnectToGame, "ConnectedToGame", ip, port);

protected:
	virtual void OnKeyCodeTyped(vgui::KeyCode code);
	virtual void OnTick(void);
	virtual void PerformLayout(void);

private:
	long m_iRequestRetry;

private:
	static int PlayerTimeColumnSortFunc(vgui::ListPanel *pPanel, const vgui::ListPanelItem &p1, const vgui::ListPanelItem &p2);

private:
	void RequestInfo(void);
	void ShowAutoRetryOptions(bool state);
	void ConstructConnectArgs(char *pchOptions, int cchOptions, const serveritem_t &server);
	void ApplyConnectCommand(const serveritem_t &server);

private:
	vgui::Button *m_pConnectButton;
	vgui::Button *m_pCloseButton;
	vgui::Button *m_pRefreshButton;
	vgui::Label *m_pInfoLabel;
	vgui::ToggleButton *m_pAutoRetry;
	vgui::RadioButton *m_pAutoRetryAlert;
	vgui::RadioButton *m_pAutoRetryJoin;
	vgui::ListPanel *m_pPlayerList;

	enum { PING_TIMES_MAX = 4 };

	unsigned int m_iServerID;

	CServerList m_Servers;
	CGetChallenge m_GetChallenge;
	CPlayerInfo m_PlayerInfo;

	bool m_bConnecting;
	char m_szPassword[64];
	int m_iChallenge;

	bool m_bServerNotResponding;
	bool m_bServerFull;
	bool m_bShowAutoRetryToggle;
	bool m_bShowingExtendedOptions;
	bool m_bQueryPlayerInfo;

	bool m_bPlayerListUpdatePending;
};

#endif