#include "EngineInterface.h"
#include "DialogGameInfo.h"
#include "IGameList.h"
#include "ServerBrowserDialog.h"
#include "ServerList.h"
#include "DialogServerPassword.h"

#include <vgui/ISystem.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <KeyValues.h>

#include <vgui_controls/Label.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ToggleButton.h>
#include <vgui_controls/RadioButton.h>

#include <stdio.h>

using namespace vgui;

static const long RETRY_TIME = 10000;

CDialogGameInfo::CDialogGameInfo(vgui::Panel *parent, serveritem_t &server) : Frame(parent, "DialogGameInfo"), m_Servers(this), m_GetChallenge(this), m_PlayerInfo(this)
{
	SetBounds(0, 0, 512, 512);
	SetMinimumSize(416, 340);
	SetDeleteSelfOnClose(true);

	m_bConnecting = false;
	m_bServerFull = false;
	m_bShowAutoRetryToggle = false;
	m_bServerNotResponding = false;
	m_bShowingExtendedOptions = false;
	m_bPlayerListUpdatePending = false;
	m_bQueryPlayerInfo = false;

	m_szPassword[0] = 0;
	m_iChallenge = 0;

	m_pConnectButton = new Button(this, "Connect", "#ServerBrowser_JoinGame");
	m_pCloseButton = new Button(this, "Close", "#ServerBrowser_Close");
	m_pRefreshButton = new Button(this, "Refresh", "#ServerBrowser_Refresh");
	m_pInfoLabel = new Label(this, "InfoLabel", "");
	m_pAutoRetry = new ToggleButton(this, "AutoRetry", "#ServerBrowser_AutoRetry");
	m_pAutoRetry->AddActionSignalTarget(this);

	m_pAutoRetryAlert = new RadioButton(this, "AutoRetryAlert", "#ServerBrowser_AlertMeWhenSlotOpens");
	m_pAutoRetryJoin = new RadioButton(this, "AutoRetryJoin", "#ServerBrowser_JoinWhenSlotOpens");
	m_pPlayerList = new ListPanel(this, "PlayerList");
	m_pPlayerList->AddColumnHeader(0, "PlayerName", "#ServerBrowser_PlayerName", 156);
	m_pPlayerList->AddColumnHeader(1, "Score", "#ServerBrowser_Score", 64);
	m_pPlayerList->AddColumnHeader(2, "Time", "#ServerBrowser_Time", 64);

	m_pPlayerList->SetSortFunc(2, &PlayerTimeColumnSortFunc);

	PostMessage(m_pPlayerList, new KeyValues("SetSortColumn", "column", 2));
	PostMessage(m_pPlayerList, new KeyValues("SetSortColumn", "column", 1));
	PostMessage(m_pPlayerList, new KeyValues("SetSortColumn", "column", 1));

	m_pAutoRetryAlert->SetSelected(true);

	m_pConnectButton->SetCommand(new KeyValues("Connect"));
	m_pCloseButton->SetCommand(new KeyValues("Close"));
	m_pRefreshButton->SetCommand(new KeyValues("Refresh"));

	m_iRequestRetry = 0;
	m_iServerID = m_Servers.AddNewServer(server);

	RequestInfo();

	ivgui()->AddTickSignal(GetVPanel());

	LoadControlSettings("Servers/DialogGameInfo.res");
	RegisterControlSettingsFile("Servers/DialogGameInfo_SinglePlayer.res");
	RegisterControlSettingsFile("Servers/DialogGameInfo_AutoRetry.res");
}

CDialogGameInfo::~CDialogGameInfo(void)
{
}

void CDialogGameInfo::SendPlayerQuery(void)
{
	serveritem_t server = m_Servers.GetServer(m_iServerID);

	if (m_bQueryPlayerInfo)
		return;

	m_GetChallenge.SetServer(server);
	m_GetChallenge.Query();
	m_bQueryPlayerInfo = true;
}

void CDialogGameInfo::Run(const char *titleName)
{
	if (titleName)
		SetTitle("#ServerBrowser_GameInfoWithNameTitle", true);
	else
		SetTitle("#ServerBrowser_GameInfoWithNameTitle", true);

	SetDialogVariable("game", titleName);

	RequestInfo();
	Activate();
}

void CDialogGameInfo::PerformLayout(void)
{
	BaseClass::PerformLayout();

	serveritem_t &server = m_Servers.GetServer(m_iServerID);

	SetControlString("ServerText", server.name);
	SetControlString("GameText", server.gameDescription);
	SetControlString("MapText", server.map);

	if (!server.hadSuccessfulResponse)
		SetControlString("SecureText", "");
	else
		SetControlString("SecureText", "#ServerBrowser_NotSecure");

	char buf[128];

	if (server.maxPlayers > 0)
	{
		Q_snprintf(buf, sizeof(buf), "%d / %d", server.players, server.maxPlayers);
	}
	else
		buf[0] = 0;

	SetControlString("PlayersText", buf);

	if (server.ip[0] && server.port)
	{
		char buf[64];
		sprintf(buf, "%d.%d.%d.%d:%d", server.ip[0], server.ip[1], server.ip[2], server.ip[3], server.port);
		SetControlString("ServerIPText", buf);
		m_pConnectButton->SetEnabled(true);

		if (m_pAutoRetry->IsSelected())
		{
			m_pAutoRetryAlert->SetVisible(true);
			m_pAutoRetryJoin->SetVisible(true);
		}
		else
		{
			m_pAutoRetryAlert->SetVisible(false);
			m_pAutoRetryJoin->SetVisible(false);
		}
	}
	else
	{
		SetControlString("ServerIPText", "");
		m_pConnectButton->SetEnabled(false);
	}

	if (server.hadSuccessfulResponse && server.ping < 1200)
	{
		Q_snprintf(buf, sizeof(buf), "%d", server.ping);
		SetControlString("PingText", buf);
	}
	else
	{
		SetControlString("PingText", "");
	}

	if (m_pAutoRetry->IsSelected())
	{
		if (server.players < server.maxPlayers)
			m_pInfoLabel->SetText("#ServerBrowser_PressJoinToConnect");
		else if (m_pAutoRetryJoin->IsSelected())
			m_pInfoLabel->SetText("#ServerBrowser_JoinWhenSlotIsFree");
		else
			m_pInfoLabel->SetText("#ServerBrowser_AlertWhenSlotIsFree");
	}
	else if (m_bServerFull)
	{
		m_pInfoLabel->SetText("#ServerBrowser_CouldNotConnectServerFull");
	}
	else if (m_bServerNotResponding)
	{
		m_pInfoLabel->SetText("#ServerBrowser_ServerNotResponding");
	}
	else
	{
		m_pInfoLabel->SetText("");
	}

	if (server.hadSuccessfulResponse && !(server.players + server.botPlayers))
		m_pPlayerList->SetEmptyListText("#ServerBrowser_ServerHasNoPlayers");
	else
		m_pPlayerList->SetEmptyListText("#ServerBrowser_ServerNotResponding");

	m_pAutoRetry->SetVisible(m_bShowAutoRetryToggle);

	Repaint();
}

void CDialogGameInfo::Connect(void)
{
	OnConnect();
}

void CDialogGameInfo::OnConnect(void)
{
	m_bConnecting = true;

	m_bServerFull = false;
	m_bServerNotResponding = false;
	m_iRequestRetry = 0;

	InvalidateLayout();
	RequestInfo();
}

void CDialogGameInfo::OnConnectToGame(int ip, int port)
{
	serveritem_t server = m_Servers.GetServer(m_iServerID);

	if (*(int *)server.ip == ip && server.port == port)
		Close();
}

void CDialogGameInfo::OnRefresh(void)
{
	m_iRequestRetry = 0;

	RequestInfo();
}

void CDialogGameInfo::OnButtonToggled(Panel *panel)
{
	if (panel == m_pAutoRetry)
		ShowAutoRetryOptions(m_pAutoRetry->IsSelected());

	InvalidateLayout();
}

void CDialogGameInfo::OnKeyCodeTyped(KeyCode code)
{
	if (code == KEY_ESCAPE)
	{
		SetAlpha(0);
		Close();
	}
	else
		BaseClass::OnKeyCodeTyped(code);
}

void CDialogGameInfo::ShowAutoRetryOptions(bool state)
{
	int growSize = 60;

	if (!state)
		growSize = -growSize;

	int x, y, wide, tall;
	GetBounds(x, y, wide, tall);
	SetMinimumSize(416, 340);

	if (state)
		LoadControlSettings("Servers/DialogGameInfo_AutoRetry.res");
	else
		LoadControlSettings("Servers/DialogGameInfo.res");

	m_pAutoRetryAlert->SetSelected(true);

	SetBounds(x, y, wide, tall + growSize);
	InvalidateLayout();
}

void CDialogGameInfo::RequestInfo(void)
{
	m_iRequestRetry = system()->GetTimeMillis() + RETRY_TIME;

	if (!m_Servers.IsRefreshing())
	{
		m_Servers.AddServerToRefreshList(m_iServerID);
		m_Servers.StartRefresh();
	}
}

void CDialogGameInfo::OnTick(void)
{
	if (m_iRequestRetry && m_iRequestRetry < system()->GetTimeMillis())
	{
		m_iRequestRetry = 0;
		RequestInfo();
	}

	m_Servers.RunFrame();
	m_GetChallenge.RunFrame();
	m_PlayerInfo.RunFrame();
}

void CDialogGameInfo::ServerResponded(serveritem_t &server)
{
	if (m_bConnecting)
	{
		ConnectToServer();
	}
	else if (m_pAutoRetry->IsSelected() && server.players < server.maxPlayers)
	{
		surface()->PlaySound("servers/game_ready.wav");

		FlashWindow();

		if (m_pAutoRetryJoin->IsSelected())
			ConnectToServer();
	}
	else
		SendPlayerQuery();

	m_bServerNotResponding = false;

	InvalidateLayout();
	Repaint();
}

void CDialogGameInfo::ServerFailedToRespond(serveritem_t &server)
{
	if (!server.hadSuccessfulResponse)
		m_bServerNotResponding = true;

	InvalidateLayout();
	Repaint();
}

void CDialogGameInfo::ApplyConnectCommand(const serveritem_t &server)
{
	char command[256];

	if (m_szPassword[0])
	{
		Q_snprintf(command, Q_ARRAYSIZE(command), "password \"%s\"\n", m_szPassword);
		engine->pfnClientCmd(command);
	}

	char buf[64];
	Q_snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%d", server.ip[0], server.ip[1], server.ip[2], server.ip[3], server.port);
	Q_snprintf(command, Q_ARRAYSIZE(command), "connect %s\n", buf);

	engine->pfnClientCmd(command);
}

void CDialogGameInfo::ConstructConnectArgs(char *pchOptions, int cchOptions, const serveritem_t &server)
{
	char buf[64];
	Q_snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%d", server.ip[0], server.ip[1], server.ip[2], server.ip[3], server.port);
	Q_snprintf(pchOptions, cchOptions, " +connect %s", buf);

	if (m_szPassword[0])
	{
		Q_strncat(pchOptions, " +password \"", cchOptions);
		Q_strncat(pchOptions, m_szPassword, cchOptions);
		Q_strncat(pchOptions, "\"", cchOptions);
	}
}

void CDialogGameInfo::ConnectToServer(void)
{
	m_bConnecting = false;

	serveritem_t &server = m_Servers.GetServer(m_iServerID);

	if (server.password && !m_szPassword[0])
	{
		CDialogServerPassword *box = new CDialogServerPassword(this);
		box->AddActionSignalTarget(this);
		box->Activate(server.name, server.serverID);
		return;
	}

	if (server.players >= server.maxPlayers)
	{
		m_bServerFull = true;
		m_bShowAutoRetryToggle = true;

		InvalidateLayout();
		return;
	}

	ApplyConnectCommand(server);

	PostMessage(this, new KeyValues("Close"));
}

void CDialogGameInfo::RefreshComplete(void)
{
}

void CDialogGameInfo::OnJoinServerWithPassword(const char *password)
{
	Q_strncpy(m_szPassword, password, sizeof(m_szPassword));

	OnConnect();
}

void CDialogGameInfo::ClearPlayerList(void)
{
	m_pPlayerList->DeleteAllItems();

	Repaint();
}

void CDialogGameInfo::GetChallengeFailedToRespond(void)
{
}

void CDialogGameInfo::GetChallengeRefreshComplete(int challenge)
{
	serveritem_t server = m_Servers.GetServer(m_iServerID);

	if (m_bQueryPlayerInfo)
	{
		m_PlayerInfo.SetChallenge(challenge);
		m_PlayerInfo.SetServer(server);
		m_PlayerInfo.Query();
		m_bPlayerListUpdatePending = true;
		m_bQueryPlayerInfo = false;
		m_iChallenge = challenge;
	}
}

void CDialogGameInfo::AddPlayerToList(const char *playerName, int score, float timePlayedSeconds)
{
	if (m_bPlayerListUpdatePending)
	{
		m_bPlayerListUpdatePending = false;
		m_pPlayerList->RemoveAll();
	}

	KeyValues *player = new KeyValues("player");
	player->SetString("PlayerName", playerName);
	player->SetInt("Score", score);
	player->SetInt("TimeSec", (int)timePlayedSeconds);

	int seconds = (int)timePlayedSeconds;
	int minutes = seconds / 60;
	int hours = minutes / 60;

	seconds %= 60;
	minutes %= 60;

	char buf[64];
	buf[0] = 0;

	if (hours)
		Q_snprintf(buf, sizeof(buf), "%dh %dm %ds", hours, minutes, seconds);
	else if (minutes)
		Q_snprintf(buf, sizeof(buf), "%dm %ds", minutes, seconds);
	else
		Q_snprintf(buf, sizeof(buf), "%ds", seconds);

	player->SetString("Time", buf);

	m_pPlayerList->AddItem(player, 0, false, true);
	player->deleteThis();
}

void CDialogGameInfo::PlayersFailedToRespond(void)
{
}

void CDialogGameInfo::PlayersRefreshComplete(void)
{
}

int CDialogGameInfo::PlayerTimeColumnSortFunc(ListPanel *pPanel, const ListPanelItem &p1, const ListPanelItem &p2)
{
	int p1time = p1.kv->GetInt("TimeSec");
	int p2time = p2.kv->GetInt("TimeSec");

	if (p1time > p2time)
		return -1;
	if (p1time < p2time)
		return 1;

	return 0;
}