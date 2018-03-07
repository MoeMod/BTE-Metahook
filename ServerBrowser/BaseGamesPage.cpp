#include "BaseGamesPage.h"
#include "ServerListCompare.h"
#include "ServerBrowserDialog.h"

#include <vgui/ILocalize.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/KeyCode.h>
#include <KeyValues.h>

#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ImageList.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/ListPanel.h>

#include <stdio.h>

using namespace vgui;

CGameListPanel::CGameListPanel(CBaseGamesPage *pOuter, const char *pName) : BaseClass(pOuter, pName)
{
	m_pOuter = pOuter;
}

void CGameListPanel::OnKeyCodeTyped(vgui::KeyCode code)
{
	if (code == KEY_ENTER && m_pOuter->OnGameListEnterPressed())
		return;

	BaseClass::OnKeyCodeTyped(code);
}

CBaseGamesPage::CBaseGamesPage(vgui::Panel *parent, const char *name, const char *pCustomResFilename) : PropertyPage(parent, name), m_pCustomResFilename(pCustomResFilename), m_Servers(this)
{
	SetSize(624, 278);

	m_szGameFilter[0] = 0;
	m_szMapFilter[0] = 0;
	m_iPingFilter = 0;
	m_iServerRefreshCount = 0;
	m_bFilterNoFullServers = false;
	m_bFilterNoEmptyServers = false;
	m_bFilterNoPasswordedServers = false;
	m_hFont = NULL;

	wchar_t *all = g_pVGuiLocalize->Find("ServerBrowser_All");
	if (all)
		Q_UnicodeToUTF8(all, m_szComboAllText, sizeof(m_szComboAllText));

	m_pConnect = new Button(this, "ConnectButton", "#ServerBrowser_Connect");
	m_pConnect->SetEnabled(false);
	m_pRefreshAll = new Button(this, "RefreshButton", "#ServerBrowser_Refresh");
	m_pRefreshQuick = new Button(this, "RefreshQuickButton", "#ServerBrowser_RefreshQuick");
	m_pAddServer = new Button(this, "AddServerButton", "#ServerBrowser_AddServer");
	m_pAddCurrentServer = new Button(this, "AddCurrentServerButton", "#ServerBrowser_AddCurrentServer");
	m_pGameList = new CGameListPanel(this, "gamelist");
	m_pGameList->SetAllowUserModificationOfColumns(true);

	m_pAddToFavoritesButton = new vgui::Button(this, "AddToFavoritesButton", "");
	m_pAddToFavoritesButton->SetEnabled(false);
	m_pAddToFavoritesButton->SetVisible(false);

	m_pGameList->AddColumnHeader(0, "Password", "#ServerBrowser_Password", 16, ListPanel::COLUMN_FIXEDSIZE | ListPanel::COLUMN_IMAGE);
	m_pGameList->AddColumnHeader(1, "Bots", "#ServerBrowser_Bots", 16, ListPanel::COLUMN_FIXEDSIZE | ListPanel::COLUMN_HIDDEN);
	m_pGameList->AddColumnHeader(2, "Name", "#ServerBrowser_Servers", 50, ListPanel::COLUMN_RESIZEWITHWINDOW | ListPanel::COLUMN_UNHIDABLE);
	m_pGameList->AddColumnHeader(3, "GameDesc", "#ServerBrowser_Game", 112, 112, 300, 0);
	m_pGameList->AddColumnHeader(4, "Players", "#ServerBrowser_Players", 55, ListPanel::COLUMN_FIXEDSIZE);
	m_pGameList->AddColumnHeader(5, "Map", "#ServerBrowser_Map", 90, 90, 300, 0);
	m_pGameList->AddColumnHeader(6, "Ping", "#ServerBrowser_Latency", 55, ListPanel::COLUMN_FIXEDSIZE);

	m_pGameList->SetColumnHeaderTooltip(0, "#ServerBrowser_PasswordColumn_Tooltip");
	m_pGameList->SetColumnHeaderTooltip(1, "#ServerBrowser_BotColumn_Tooltip");

	m_pGameList->SetSortFunc(0, PasswordCompare);
	m_pGameList->SetSortFunc(1, BotsCompare);
	m_pGameList->SetSortFunc(2, ServerNameCompare);
	m_pGameList->SetSortFunc(3, GameCompare);
	m_pGameList->SetSortFunc(4, PlayersCompare);
	m_pGameList->SetSortFunc(5, MapCompare);
	m_pGameList->SetSortFunc(6, PingCompare);
	m_pGameList->SetSortColumn(6);

	ivgui()->AddTickSignal(GetVPanel());

	CreateFilters();
	LoadFilterSettings();
}

CBaseGamesPage::~CBaseGamesPage(void)
{
}

int CBaseGamesPage::GetInvalidServerListID(void)
{
	return m_pGameList->InvalidItemID();
}

void CBaseGamesPage::PerformLayout(void)
{
	BaseClass::PerformLayout();

	if (m_pGameList->GetSelectedItemsCount() < 1)
		m_pConnect->SetEnabled(false);
	else
		m_pConnect->SetEnabled(true);

	if (SupportsItem(IGameList::GETNEWLIST))
	{
		m_pRefreshQuick->SetVisible(true);
		m_pRefreshAll->SetText("#ServerBrowser_RefreshAll");
	}
	else
	{
		m_pRefreshQuick->SetVisible(false);
		m_pRefreshAll->SetText("#ServerBrowser_Refresh");
	}

	if (SupportsItem(IGameList::ADDSERVER))
	{
		m_pFilterString->SetWide(90);
		m_pAddServer->SetVisible(true);
	}
	else
	{
		m_pAddServer->SetVisible(false);
	}

	if (SupportsItem(IGameList::ADDCURRENTSERVER))
		m_pAddCurrentServer->SetVisible(true);
	else
		m_pAddCurrentServer->SetVisible(false);

	if (IsRefreshing())
		m_pRefreshAll->SetText("#ServerBrowser_StopRefreshingList");

	if (m_pGameList->GetItemCount() > 0)
		m_pRefreshQuick->SetEnabled(true);
	else
		m_pRefreshQuick->SetEnabled(false);

	Repaint();
}

bool CBaseGamesPage::IsActivated(void)
{
	if (!ServerBrowserDialog().IsVisible() || !ipanel()->Render_GetPopupVisible(ServerBrowserDialog().GetVPanel()))
		return false;

	if (ServerBrowserDialog().GetActivePage() != this)
		return false;

	return true;
}

void CBaseGamesPage::OnTick(void)
{
	BaseClass::OnTick();

	if (IsActivated())
	{
		m_Servers.RunFrame();
	}
}

void CBaseGamesPage::ApplySchemeSettings(IScheme *pScheme)
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

	m_pGameList->SetImageList(imageList, true);
	m_hFont = pScheme->GetFont("ListSmall", IsProportional());

	if (!m_hFont)
		m_hFont = pScheme->GetFont("DefaultSmall", IsProportional());

	m_pGameList->SetFont(m_hFont);
	m_pGameList->SetColumnHeaderImage(0, passwordColumnImage);
	m_pGameList->SetColumnHeaderImage(1, botColumnImage);

	OnButtonToggled(m_pFilter, false);
}

serveritem_t &CBaseGamesPage::GetServer(unsigned int serverID)
{
	return m_Servers.GetServer(serverID);
}

void CBaseGamesPage::CreateFilters(void)
{
	m_bFiltersVisible = false;
	m_szMasterServerFilterString[0] = 0;
	m_pFilter = new ToggleButton(this, "Filter", "#ServerBrowser_Filter");
	m_pFilterString = new Label(this, "FilterString", "");
	m_pGameFilter = new ComboBox(this, "GameFilter", 6, false);

	KeyValues *pkv = new KeyValues("mod", "gamedir", "");
	m_pGameFilter->AddItem("#ServerBrowser_All", pkv);
	pkv->deleteThis();

	m_pLocationFilter = new ComboBox(this, "LocationFilter", 6, false);
	m_pLocationFilter->AddItem("", NULL);

	m_pMapFilter = new TextEntry(this, "MapFilter");
	m_pPingFilter = new ComboBox(this, "PingFilter", 6, false);
	m_pPingFilter->AddItem("#ServerBrowser_All", NULL);
	m_pPingFilter->AddItem("#ServerBrowser_LessThan50", NULL);
	m_pPingFilter->AddItem("#ServerBrowser_LessThan100", NULL);
	m_pPingFilter->AddItem("#ServerBrowser_LessThan150", NULL);
	m_pPingFilter->AddItem("#ServerBrowser_LessThan250", NULL);
	m_pPingFilter->AddItem("#ServerBrowser_LessThan350", NULL);
	m_pPingFilter->AddItem("#ServerBrowser_LessThan600", NULL);

	m_pSecureFilter = new ComboBox(this, "SecureFilter", 3, false);
	m_pSecureFilter->AddItem("#ServerBrowser_All", NULL);
	m_pSecureFilter->AddItem("#ServerBrowser_SecureOnly", NULL);
	m_pSecureFilter->AddItem("#ServerBrowser_InsecureOnly", NULL);

	m_pNoEmptyServersFilterCheck = new CheckButton(this, "ServerEmptyFilterCheck", "");
	m_pNoFullServersFilterCheck = new CheckButton(this, "ServerFullFilterCheck", "");
	m_pNoPasswordFilterCheck = new CheckButton(this, "NoPasswordFilterCheck", "");
}

void CBaseGamesPage::LoadFilterSettings(void)
{
	KeyValues *filter = ServerBrowserDialog().GetFilterSaveData(GetName());

	if (ServerBrowserDialog().GetActiveModName())
		Q_strncpy(m_szGameFilter, ServerBrowserDialog().GetActiveModName(), sizeof(m_szGameFilter));
	else
		Q_strncpy(m_szGameFilter, filter->GetString("game"), sizeof(m_szGameFilter));

	Q_strncpy(m_szMapFilter, filter->GetString("map"), sizeof(m_szMapFilter));
	m_iPingFilter = filter->GetInt("ping");
	m_bFilterNoFullServers = filter->GetInt("NoFull");
	m_bFilterNoEmptyServers = filter->GetInt("NoEmpty");
	m_bFilterNoPasswordedServers = filter->GetInt("NoPassword");

	UpdateGameFilter();

	m_pMapFilter->SetText(m_szMapFilter);
	m_pLocationFilter->ActivateItem(filter->GetInt("location"));

	if (m_iPingFilter)
	{
		char buf[32];
		Q_snprintf(buf, sizeof(buf), "< %d", m_iPingFilter);
		m_pPingFilter->SetText(buf);
	}

	m_pNoFullServersFilterCheck->SetSelected(m_bFilterNoFullServers);
	m_pNoEmptyServersFilterCheck->SetSelected(m_bFilterNoEmptyServers);
	m_pNoPasswordFilterCheck->SetSelected(m_bFilterNoPasswordedServers);

	OnLoadFilter(filter);
	UpdateFilterSettings();
}

void CBaseGamesPage::UpdateGameFilter(void)
{
	bool bFound = false;

	for (int i = 0; i < m_pGameFilter->GetItemCount(); i++)
	{
		KeyValues *kv = m_pGameFilter->GetItemUserData(i);
		const char *pchGameDir = kv->GetString("gamedir");

		if (!m_szGameFilter[0] || !Q_strncmp(pchGameDir, m_szGameFilter, Q_strlen(pchGameDir)))
		{
			if (i != m_pGameFilter->GetActiveItem())
				m_pGameFilter->ActivateItem(i);

			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		if (m_pGameFilter->GetActiveItem() != 0)
			m_pGameFilter->ActivateItem(0);
	}

	if (ServerBrowserDialog().GetActiveModName())
	{
		m_pGameFilter->SetEnabled(false);
		m_pGameFilter->SetText(ServerBrowserDialog().GetActiveGameName());
	}

	m_pSecureFilter->SetVisible(false);
}

void CBaseGamesPage::ServerResponded(serveritem_t &server)
{
	bool removeItem = false;

	if (!CheckPrimaryFilters(server))
	{
		server.doNotRefresh = true;
		removeItem = true;
	}
	else if (!CheckSecondaryFilters(server))
	{
		removeItem = true;
	}

	if (removeItem)
		return;

	KeyValues *kv;

	if (m_pGameList->IsValidItemID(server.listEntryID))
	{
		kv = m_pGameList->GetItem(server.listEntryID);
		m_pGameList->SetUserData(server.listEntryID, server.serverID);
	}
	else
		kv = new KeyValues("Server");

	kv->SetString("name", server.name);
	kv->SetString("map", server.map);
	kv->SetString("GameDir", server.gameDir);
	kv->SetString("GameDesc", server.gameDescription);
	kv->SetInt("password", server.password ? 1 : 0);

	char buf[256];
	sprintf(buf, "%d / %d", server.players, server.maxPlayers);
	kv->SetString("Players", buf);

	if (server.ping < 1200)
		kv->SetInt("Ping", server.ping);
	else
		kv->SetString("Ping", "");

	if (!m_pGameList->IsValidItemID(server.listEntryID))
		server.listEntryID = m_pGameList->AddItem(kv, server.serverID, false, false);
	else
		m_pGameList->ApplyItemChanges(server.listEntryID);

	UpdateStatus();

	m_pGameList->InvalidateLayout();
	m_pGameList->Repaint();
}

void CBaseGamesPage::OnButtonToggled(Panel *panel, int state)
{
	if (panel == m_pFilter)
	{
		int wide, tall;
		GetSize(wide, tall);
		SetSize(624, 278);

		if (m_pCustomResFilename)
		{
			m_bFiltersVisible = false;
		}
		else
		{
			if (m_pFilter->IsSelected())
				m_bFiltersVisible = true;
			else
				m_bFiltersVisible = false;
		}

		UpdateDerivedLayouts();

		m_pFilter->SetSelected(m_bFiltersVisible);

		UpdateGameFilter();

		if (m_hFont)
		{
			SETUP_PANEL(m_pGameList);
			m_pGameList->SetFont(m_hFont);
		}

		SetSize(wide, tall);
		InvalidateLayout();
	}
	else if (panel == m_pNoFullServersFilterCheck || panel == m_pNoEmptyServersFilterCheck || panel == m_pNoPasswordFilterCheck)
		OnTextChanged(panel, "");
}

void CBaseGamesPage::UpdateDerivedLayouts(void)
{
	char rgchControlSettings[MAX_PATH];

	if (m_pCustomResFilename)
	{
		Q_snprintf(rgchControlSettings, sizeof(rgchControlSettings), "%s", m_pCustomResFilename);
	}
	else
	{
		if (m_pFilter->IsSelected())
			Q_snprintf(rgchControlSettings, sizeof(rgchControlSettings), "Servers/%sPage_Filters.res", "InternetGames");
		else
			Q_snprintf(rgchControlSettings, sizeof(rgchControlSettings), "Servers/%sPage.res", "InternetGames");
	}

	LoadControlSettings(rgchControlSettings);
}

void CBaseGamesPage::OnTextChanged(Panel *panel, const char *text)
{
	if (!Q_stricmp(text, m_szComboAllText))
	{
		ComboBox *box = dynamic_cast<ComboBox *>(panel);

		if (box)
		{
			box->SetText("");
			text = "";
		}
	}

	UpdateFilterSettings();
	ApplyFilters();

	if (m_bFiltersVisible && (panel == m_pGameFilter || panel == m_pLocationFilter))
	{
		StopRefresh(); 
		GetNewServerList(); 
	}
}

void CBaseGamesPage::ApplyFilters(void)
{
	ApplyGameFilters();
}

void CBaseGamesPage::ApplyGameFilters(void)
{
	for (unsigned int i = 0; i < m_Servers.ServerCount(); i++)
	{
		serveritem_t &server = m_Servers.GetServer(i);

		if (!CheckPrimaryFilters(server) || !CheckSecondaryFilters(server))
		{
			server.doNotRefresh = true;

			if (m_pGameList->IsValidItemID(server.listEntryID))
				m_pGameList->SetItemVisible(server.listEntryID, false);
		}
		else if (server.hadSuccessfulResponse)
		{
			server.doNotRefresh = false;

			if (!m_pGameList->IsValidItemID(server.listEntryID))
			{
				KeyValues *kv = new KeyValues("Server");
				kv->SetString("name", server.name);
				kv->SetString("map", server.map);
				kv->SetString("GameDir", server.gameDir);
				kv->SetString("GameDesc", server.gameDescription);

				char buf[256];
				sprintf(buf, "%d / %d", server.players, server.maxPlayers);
				kv->SetString("Players", buf);
				kv->SetInt("Ping", server.ping);
				kv->SetInt("password", server.password ? 1 : 0);

				server.listEntryID = m_pGameList->AddItem(kv, i, false, false);
			}

			m_pGameList->SetItemVisible(server.listEntryID, true);
		}
	}

	UpdateStatus();

	m_pGameList->SortList();

	InvalidateLayout();
	Repaint();
}

void CBaseGamesPage::UpdateStatus(void)
{
	if (m_pGameList->GetItemCount() > 1)
	{
		wchar_t header[256];
		wchar_t count[128];

		_itow(m_pGameList->GetItemCount(), count, 10);
		g_pVGuiLocalize->ConstructString(header, sizeof(header), g_pVGuiLocalize->Find("#ServerBrowser_ServersCount"), 1, count);
		m_pGameList->SetColumnHeaderText(2, header);
	}
	else
	{
		m_pGameList->SetColumnHeaderText(2, g_pVGuiLocalize->Find("#ServerBrowser_Servers"));
	}
}

void CBaseGamesPage::UpdateFilterSettings(void)
{
	if (ServerBrowserDialog().GetActiveModName())
	{
		Q_strncpy(m_szGameFilter, ServerBrowserDialog().GetActiveModName(), sizeof(m_szGameFilter));
		RecalculateFilterString();
		UpdateGameFilter();
	}
	else
	{
		KeyValues *data = m_pGameFilter->GetActiveItemUserData();

		if (data)
			Q_strncpy(m_szGameFilter, data->GetString("gamedir"), sizeof(m_szGameFilter));

		m_pGameFilter->SetEnabled(true);
	}

	Q_strlower(m_szGameFilter);
	m_pMapFilter->GetText(m_szMapFilter, sizeof(m_szMapFilter) - 1);
	Q_strlower(m_szMapFilter);

	char buf[256];
	m_pPingFilter->GetText(buf, sizeof(buf));

	if (buf[0])
		m_iPingFilter = atoi(buf + 2);
	else
		m_iPingFilter = 0;

	m_bFilterNoFullServers = m_pNoFullServersFilterCheck->IsSelected();
	m_bFilterNoEmptyServers = m_pNoEmptyServersFilterCheck->IsSelected();
	m_bFilterNoPasswordedServers = m_pNoPasswordFilterCheck->IsSelected();

	buf[0] = 0;

	if (m_szGameFilter[0])
	{
		strcat(buf, "\\gamedir\\");
		strcat(buf, m_szGameFilter);
	}

	if (m_bFilterNoEmptyServers)
	{
		strcat(buf, "\\empty\\1");
	}

	if (m_bFilterNoFullServers)
	{
		strcat(buf, "\\full\\1");
	}

	int regCode = GetRegionCodeToFilter();

	if (regCode > 0)
	{
		char szRegCode[32];
		Q_snprintf(szRegCode, sizeof(szRegCode), "%i", regCode);
		strcat(buf, "\\region\\");
		strcat(buf, szRegCode);
	}

	KeyValues *filter = ServerBrowserDialog().GetFilterSaveData(GetName());

	if (!ServerBrowserDialog().GetActiveModName())
		filter->SetString("game", m_szGameFilter);

	filter->SetString("map", m_szMapFilter);
	filter->SetInt("ping", m_iPingFilter);

	if (m_pLocationFilter->GetItemCount() > 1)
		filter->SetInt("location", m_pLocationFilter->GetActiveItem());

	filter->SetInt("NoFull", m_bFilterNoFullServers);
	filter->SetInt("NoEmpty", m_bFilterNoEmptyServers);
	filter->SetInt("NoPassword", m_bFilterNoPasswordedServers);

	OnSaveFilter(filter);

	RecalculateFilterString();
}

void CBaseGamesPage::OnSaveFilter(KeyValues *filter)
{
}

void CBaseGamesPage::OnLoadFilter(KeyValues *filter)
{
}

void CBaseGamesPage::RecalculateFilterString(void)
{
#if 0
	wchar_t unicode[2048], tempUnicode[128], spacerUnicode[8];
	unicode[0] = 0;

	Q_UTF8ToUnicode("; ", spacerUnicode, sizeof(spacerUnicode));

	if (m_pLocationFilter->GetActiveItem() > 0)
	{
		m_pLocationFilter->GetText(tempUnicode, sizeof(tempUnicode));
		wcscat(unicode, tempUnicode);
		wcscat(unicode, spacerUnicode);
	}

	if (m_iPingFilter)
	{
		char tmpBuf[16];
		_itoa(m_iPingFilter, tmpBuf, 10);

		wcscat(unicode, g_pVGuiLocalize->Find("#ServerBrowser_FilterDescLatency"));
		Q_UTF8ToUnicode(" < ", tempUnicode, sizeof(tempUnicode));
		wcscat(unicode, tempUnicode);
		Q_UTF8ToUnicode(tmpBuf, tempUnicode, sizeof(tempUnicode));
		wcscat(unicode, tempUnicode);
		wcscat(unicode, spacerUnicode);
	}

	if (m_bFilterNoFullServers)
	{
		wcscat(unicode, g_pVGuiLocalize->Find("#ServerBrowser_FilterDescNotFull"));
		wcscat(unicode, spacerUnicode);
	}

	if (m_bFilterNoEmptyServers)
	{
		wcscat(unicode, g_pVGuiLocalize->Find("#ServerBrowser_FilterDescNotEmpty"));
		wcscat(unicode, spacerUnicode);
	}

	if (m_bFilterNoPasswordedServers)
	{
		wcscat(unicode, g_pVGuiLocalize->Find("#ServerBrowser_FilterDescNoPassword"));
		wcscat(unicode, spacerUnicode);
	}

	if (m_szMapFilter[0])
	{
		Q_UTF8ToUnicode(m_szMapFilter, tempUnicode, sizeof(tempUnicode));
		wcscat(unicode, tempUnicode);
	}

	m_pFilterString->SetText(unicode);
#endif
}

bool CBaseGamesPage::CheckPrimaryFilters(serveritem_t &server)
{
	if (m_szGameFilter[0] && (server.gameDir[0] || server.ping) && Q_stricmp(m_szGameFilter, server.gameDir))
		return false;

	return true;
}

bool CBaseGamesPage::CheckSecondaryFilters(serveritem_t &server)
{
	if (m_bFilterNoEmptyServers && (server.players - server.botPlayers) < 1)
		return false;

	if (m_bFilterNoFullServers && server.players >= server.maxPlayers)
		return false;

	if (m_iPingFilter && server.ping > m_iPingFilter)
		return false;

	if (m_bFilterNoPasswordedServers && server.password)
		return false;

	int count = Q_strlen(m_szMapFilter);

	if (count && Q_strnicmp(server.map, m_szMapFilter, count))
		return false;

	return true;
}

const char *CBaseGamesPage::GetFilterString(void)
{
	return m_szMasterServerFilterString;
}

void CBaseGamesPage::SetRefreshing(bool state)
{
	if (state)
	{
		ServerBrowserDialog().UpdateStatusText("#ServerBrowser_RefreshingServerList");

		m_pGameList->SetEmptyListText("");
		m_pRefreshAll->SetText("#ServerBrowser_StopRefreshingList");
		m_pRefreshAll->SetCommand("stoprefresh");
		m_pRefreshQuick->SetEnabled(false);
	}
	else
	{
		ServerBrowserDialog().UpdateStatusText("");

		if (SupportsItem(IGameList::GETNEWLIST))
			m_pRefreshAll->SetText("#ServerBrowser_RefreshAll");
		else
			m_pRefreshAll->SetText("#ServerBrowser_Refresh");

		m_pRefreshAll->SetCommand("GetNewList");

		if (m_pGameList->GetItemCount() > 0)
			m_pRefreshQuick->SetEnabled(true);
		else
			m_pRefreshQuick->SetEnabled(false);
	}
}

void CBaseGamesPage::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "Connect"))
		OnBeginConnect();
	else if (!Q_stricmp(command, "stoprefresh"))
		StopRefresh();
	else if (!Q_stricmp(command, "refresh"))
		StartRefresh();
	else if (!Q_stricmp(command, "GetNewList"))
		GetNewServerList();
	else
		BaseClass::OnCommand(command);
}

void CBaseGamesPage::OnItemSelected(void)
{
	if (m_pGameList->GetSelectedItemsCount() < 1)
		m_pConnect->SetEnabled(false);
	else
		m_pConnect->SetEnabled(true);
}

void CBaseGamesPage::OnKeyCodePressed(vgui::KeyCode code)
{
	if (code == KEY_F5)
		StartRefresh();
	else
		BaseClass::OnKeyCodePressed(code);
}

bool CBaseGamesPage::OnGameListEnterPressed(void)
{
	return false;
}

int CBaseGamesPage::GetSelectedItemsCount(void)
{
	return m_pGameList->GetSelectedItemsCount();
}

void CBaseGamesPage::OnAddToFavorites(void)
{
}

void CBaseGamesPage::OnRefreshServer(int serverID)
{
}

void CBaseGamesPage::StartRefresh(void)
{
	SetRefreshing(true);

	m_iServerRefreshCount = 0;
}

void CBaseGamesPage::ClearServerList(void)
{ 
	m_pGameList->RemoveAll();
}

void CBaseGamesPage::GetNewServerList(void)
{
	StartRefresh();
}

void CBaseGamesPage::StopRefresh(void)
{
	m_iServerRefreshCount = 0;

	RefreshComplete();
}

bool CBaseGamesPage::IsRefreshing(void)
{
	return false;
}

void CBaseGamesPage::OnPageShow(void)
{
	if (!ServerBrowserDialog().IsVisible())
		return;

	StartRefresh();
}

void CBaseGamesPage::OnPageHide(void)
{
	if (m_Servers.IsRefreshing())
		StopRefresh();
}

void CBaseGamesPage::OnBeginConnect(void)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

	StopRefresh();
	ServerBrowserDialog().JoinGame(this, serverID);
}

void CBaseGamesPage::OnViewGameInfo(void)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemUserData(m_pGameList->GetSelectedItem(0));

	StopRefresh();
	ServerBrowserDialog().OpenGameInfoDialog(this, serverID);
}