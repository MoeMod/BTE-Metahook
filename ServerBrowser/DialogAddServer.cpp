#include "DialogAddServer.h"
#include "ServerBrowserDialog.h"
#include "ServerListCompare.h"
#include "INetAPI.h"
#include "IGameList.h"
#include "Server.h"

#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>

#include <vgui_controls/ImageList.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/TextEntry.h>

using namespace vgui;

CAddServerGameList::CAddServerGameList(Panel *parent, const char *panelName) : BaseClass(parent, panelName)
{
}

void CAddServerGameList::OnKeyCodeTyped(vgui::KeyCode code)
{
	if (!IsInEditMode())
	{
		if (code == KEY_ESCAPE)
		{
			PostMessage(GetParent(), new KeyValues("KeyCodeTyped", "code", code));
			return;
		}

		if (code == KEY_ENTER)
		{
			PostActionSignal(new KeyValues("Command", "command", "addselected"));
			return;
		}
	}

	BaseClass::OnKeyCodeTyped(code);
}

CDialogAddServer::CDialogAddServer(vgui::Panel *parent) : Frame(parent, "DialogAddServer"), m_Servers(this)
{
	SetDeleteSelfOnClose(true);

	SetTitle("#ServerBrowser_AddServersTitle", true);
	SetSizeable(false);

	m_pTabPanel = new PropertySheet(this, "GameTabs");
	m_pTabPanel->SetTabWidth(72);

	m_pDiscoveredGames = new CAddServerGameList(this, "Servers");

	m_pDiscoveredGames->AddColumnHeader(0, "Password", "#ServerBrowser_Password", 16, ListPanel::COLUMN_FIXEDSIZE | ListPanel::COLUMN_IMAGE);
	m_pDiscoveredGames->AddColumnHeader(1, "Bots", "#ServerBrowser_Bots", 16, ListPanel::COLUMN_FIXEDSIZE | ListPanel::COLUMN_HIDDEN);
	m_pDiscoveredGames->AddColumnHeader(2, "Name", "#ServerBrowser_Servers", 30, ListPanel::COLUMN_RESIZEWITHWINDOW | ListPanel::COLUMN_UNHIDABLE);
	m_pDiscoveredGames->AddColumnHeader(3, "GameDesc", "#ServerBrowser_Game", 112, 112, 300, 0);
	m_pDiscoveredGames->AddColumnHeader(4, "Players", "#ServerBrowser_Players", 55, ListPanel::COLUMN_FIXEDSIZE);
	m_pDiscoveredGames->AddColumnHeader(5, "Map", "#ServerBrowser_Map", 75, 75, 300, 0);
	m_pDiscoveredGames->AddColumnHeader(6, "Ping", "#ServerBrowser_Latency", 55, ListPanel::COLUMN_FIXEDSIZE);

	m_pDiscoveredGames->SetColumnHeaderTooltip(0, "#ServerBrowser_PasswordColumn_Tooltip");
	m_pDiscoveredGames->SetColumnHeaderTooltip(1, "#ServerBrowser_BotColumn_Tooltip");

	m_pDiscoveredGames->SetSortFunc(0, PasswordCompare);
	m_pDiscoveredGames->SetSortFunc(1, BotsCompare);
	m_pDiscoveredGames->SetSortFunc(2, ServerNameCompare);
	m_pDiscoveredGames->SetSortFunc(3, GameCompare);
	m_pDiscoveredGames->SetSortFunc(4, PlayersCompare);
	m_pDiscoveredGames->SetSortFunc(5, MapCompare);
	m_pDiscoveredGames->SetSortFunc(6, PingCompare);

	m_pDiscoveredGames->SetSortColumn(6);

	m_pTextEntry = new vgui::TextEntry(this, "ServerNameText");
	m_pTextEntry->AddActionSignalTarget(this);

	m_pTestServersButton = new vgui::Button(this, "TestServersButton", "");
	m_pAddServerButton = new vgui::Button(this, "OKButton", "");
	m_pAddSelectedServerButton = new vgui::Button(this, "SelectedOKButton", "", this, "addselected");
	m_pTabPanel->AddPage(m_pDiscoveredGames, "#ServerBrowser_Servers");

	LoadControlSettings("Servers/DialogAddServer.res");

	m_pAddServerButton->SetEnabled(false);
	m_pTestServersButton->SetEnabled(false);
	m_pAddSelectedServerButton->SetEnabled(false);
	m_pAddSelectedServerButton->SetVisible(false);
	m_pTabPanel->SetVisible(false);

	m_pTextEntry->RequestFocus();

	int x, y;
	m_pTabPanel->GetPos(x, y);
	m_OriginalHeight = m_pTabPanel->GetTall() + y + 50;
	SetTall(y);

	ivgui()->AddTickSignal(GetVPanel());
}

CDialogAddServer::~CDialogAddServer(void)
{
	m_Servers.StopRefresh();
}

void CDialogAddServer::OnTextChanged(void)
{
	bool bAnyText = (m_pTextEntry->GetTextLength() > 0);

	m_pAddServerButton->SetEnabled(bAnyText);
	m_pTestServersButton->SetEnabled(bAnyText);
}

void CDialogAddServer::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "OK"))
	{
		OnOK();
	}
	else if (!Q_stricmp(command, "TestServers"))
	{
		SetTall(m_OriginalHeight);

		m_pTabPanel->SetVisible(true);
		m_pAddSelectedServerButton->SetVisible(true);

		TestServers();
	}
	else if (!Q_stricmp(command, "addselected"))
	{
		if (m_pDiscoveredGames->GetSelectedItemsCount())
		{
			while (m_pDiscoveredGames->GetSelectedItemsCount() > 0)
			{
				int itemID = m_pDiscoveredGames->GetSelectedItem(0);
				int serverID = m_pDiscoveredGames->GetItemUserData(itemID);

				m_pDiscoveredGames->RemoveItem(itemID);
				ServerBrowserDialog().AddServerToFavorites(m_DiscoveredServers[serverID]);
			}

			m_pDiscoveredGames->SetEmptyListText("");
		}
	}
	else
		BaseClass::OnCommand(command);
}

void CDialogAddServer::OnOK(void)
{
	const char *address = GetControlString("ServerNameText", "");
	netadr_t netaddr;

	if (net->StringToAdr(address, &netaddr))
	{
		serveritem_t server;
		memset(&server, 0, sizeof(server));

		for (int i = 0; i < 4; i++)
			server.ip[i] = netaddr.ip[i];

		server.port = (netaddr.port & 0xff) << 8 | (netaddr.port & 0xff00) >> 8;

		if (!server.port)
			server.port = 27015;

		ServerBrowserDialog().AddServerToFavorites(server);
	}
	else
	{
		MessageBox *dlg = new MessageBox("#ServerBrowser_AddServerErrorTitle", "#ServerBrowser_AddServerError");
		dlg->DoModal();
	}

	PostMessage(this, new KeyValues("Close"));
}

void CDialogAddServer::OnKeyCodeTyped(KeyCode code)
{
	if (code == KEY_ESCAPE)
	{
		SetAlpha(0);
		Close();
	}
	else
		BaseClass::OnKeyCodeTyped(code);
}

void CDialogAddServer::TestServers(void)
{
	CUtlVector<netadr_t> vecAdress;

	m_Servers.Clear();
	m_DiscoveredServers.RemoveAll();
	m_pDiscoveredGames->RemoveAll();
	m_pDiscoveredGames->SetEmptyListText("");

	const char *address = GetControlString("ServerNameText", "");

	netadr_t netaddr;
	net->StringToAdr(address, &netaddr);

	if (netaddr.port == 0)
	{
		CUtlVector<uint16> portsToTry;
		GetMostCommonQueryPorts(portsToTry);

		for (int i = 0; i < portsToTry.Count(); i++)
		{
			netadr_t newAddr = netaddr;
			newAddr.port = portsToTry[i];
			vecAdress.AddToTail(newAddr);
		}
	}
	else
	{
		netaddr.port = (netaddr.port & 0xff) << 8 | (netaddr.port & 0xff00) >> 8;
		vecAdress.AddToTail(netaddr);
	}

	m_pTabPanel->RemoveAllPages();

	wchar_t wstr[512];

	if (address[0] == 0)
	{
		Q_wcsncpy(wstr, g_pVGuiLocalize->Find("#ServerBrowser_ServersRespondingLocal"), sizeof(wstr));
	}
	else
	{
		wchar_t waddress[512];
		Q_UTF8ToUnicode(address, waddress, sizeof(waddress));
		g_pVGuiLocalize->ConstructString(wstr, sizeof(wstr), g_pVGuiLocalize->Find("#ServerBrowser_ServersResponding"), 1, waddress);
	}

	char str[512];
	Q_UnicodeToUTF8(wstr, str, sizeof(str));
	m_pTabPanel->AddPage(m_pDiscoveredGames, str);
	m_pTabPanel->InvalidateLayout();

	for (int i = 0; i < vecAdress.Size(); i++)
		AddNewServer(vecAdress[i]);

	StartRefresh();
}

void CDialogAddServer::StartRefresh(void)
{
	if (m_Servers.IsRefreshing())
		m_Servers.StopRefresh();

	m_Servers.StartRefresh();
}

void CDialogAddServer::StopRefresh(void)
{
	if (m_Servers.IsRefreshing())
		m_Servers.StopRefresh();
}

bool CDialogAddServer::IsRefreshing(void)
{
	return m_Servers.IsRefreshing();
}

void CDialogAddServer::AddNewServer(netadr_t address)
{
	serveritem_t server;
	memset(&server, 0, sizeof(server));

	for (int i = 0; i < 4; i++)
		server.ip[i] = address.ip[i];

	server.port = address.port;

	server.hadSuccessfulResponse = false;
	server.doNotRefresh = false;

	AddNewServer(server);
}

void CDialogAddServer::AddNewServer(serveritem_t &server)
{
	unsigned int index = m_Servers.AddNewServer(server);

	m_Servers.AddServerToRefreshList(index);
}

void CDialogAddServer::ServerResponded(serveritem_t &server)
{
	KeyValues *kv = new KeyValues("Server");

	kv->SetString("name", server.name);
	kv->SetString("map", server.map);
	kv->SetString("GameDir", server.gameDir);
	kv->SetString("GameDesc", server.gameDescription);
	kv->SetInt("password", server.password ? 1 : 0);
	kv->SetInt("bots", server.botPlayers ? 2 : 0);

	char buf[32];
	Q_snprintf(buf, sizeof(buf), "%d / %d", server.players, server.maxPlayers);
	kv->SetString("Players", buf);

	if (server.ping < 1200)
		kv->SetInt("Ping", server.ping);
	else
		kv->SetString("Ping", "");

	int iServer = m_DiscoveredServers.AddToTail(server);
	int iListID = m_pDiscoveredGames->AddItem(kv, iServer, false, false);

	if (m_pDiscoveredGames->GetItemCount() == 1)
		m_pDiscoveredGames->AddSelectedItem(iListID);

	kv->deleteThis();

	m_pDiscoveredGames->InvalidateLayout();
}

void CDialogAddServer::ServerFailedToRespond(serveritem_t &server)
{
	m_pDiscoveredGames->SetEmptyListText("#ServerBrowser_ServerNotResponding");
}

void CDialogAddServer::RefreshComplete(void)
{
}

void CDialogAddServer::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	ImageList *imageList = new ImageList(false);
	imageList->AddImage(scheme()->GetImage("servers/icon_password", false));
	imageList->AddImage(scheme()->GetImage("servers/icon_bots", false));
	imageList->AddImage(scheme()->GetImage("servers/icon_robotron", false));
	imageList->AddImage(scheme()->GetImage("servers/icon_secure_deny", false));

	int passwordColumnImage = imageList->AddImage(scheme()->GetImage("servers/icon_password_column", false));
	int botColumnImage = imageList->AddImage(scheme()->GetImage("servers/icon_bots_column", false));
	int secureColumnImage = imageList->AddImage(scheme()->GetImage("servers/icon_robotron_column", false));

	vgui::HFont hFont = pScheme->GetFont("ListSmall", IsProportional());

	if (!hFont)
		hFont = pScheme->GetFont("DefaultSmall", IsProportional());

	m_pDiscoveredGames->SetFont(hFont);
	m_pDiscoveredGames->SetImageList(imageList, true);
	m_pDiscoveredGames->SetColumnHeaderImage(0, passwordColumnImage);
	m_pDiscoveredGames->SetColumnHeaderImage(1, botColumnImage);
}

void CDialogAddServer::OnTick(void)
{
	BaseClass::OnTick();

	m_Servers.RunFrame();
}

void CDialogAddServer::OnItemSelected(void)
{
	int nSelectedItem = m_pDiscoveredGames->GetSelectedItem(0);

	if (nSelectedItem != -1)
		m_pAddSelectedServerButton->SetEnabled(true);
	else
		m_pAddSelectedServerButton->SetEnabled(false);
}