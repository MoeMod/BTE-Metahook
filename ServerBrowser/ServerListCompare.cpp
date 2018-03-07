#include "ServerListCompare.h"
#include "server.h"
#include "ServerBrowserDialog.h"

#include <KeyValues.h>
#include <vgui_controls/ListPanel.h>

int __cdecl PasswordCompare(ListPanel *pPanel, const ListPanelItem &p1, const ListPanelItem &p2)
{
	serveritem_t &s1 = ServerBrowserDialog().GetServer(p1.userData);
	serveritem_t &s2 = ServerBrowserDialog().GetServer(p2.userData);

	if (s1.password < s2.password)
		return 1;
	else if (s1.password > s2.password)
		return -1;

	return 0;
}

int __cdecl BotsCompare(ListPanel *pPanel, const ListPanelItem &p1, const ListPanelItem &p2)
{
	serveritem_t &s1 = ServerBrowserDialog().GetServer(p1.userData);
	serveritem_t &s2 = ServerBrowserDialog().GetServer(p2.userData);

	if (s1.botPlayers < s2.botPlayers)
		return 1;
	else if (s1.botPlayers > s2.botPlayers)
		return -1;

	return 0;
}

int __cdecl PingCompare(ListPanel *pPanel, const ListPanelItem &p1, const ListPanelItem &p2)
{
	serveritem_t &s1 = ServerBrowserDialog().GetServer(p1.userData);
	serveritem_t &s2 = ServerBrowserDialog().GetServer(p2.userData);

	if (s1.ping < s2.ping)
		return -1;
	else if (s1.ping > s2.ping)
		return 1;

	return 0;
}

int __cdecl MapCompare(ListPanel *pPanel, const ListPanelItem &p1, const ListPanelItem &p2)
{
	serveritem_t &s1 = ServerBrowserDialog().GetServer(p1.userData);
	serveritem_t &s2 = ServerBrowserDialog().GetServer(p2.userData);

	return Q_stricmp(s1.map, s2.map);
}

int __cdecl GameCompare(ListPanel *pPanel, const ListPanelItem &p1, const ListPanelItem &p2)
{
	serveritem_t &s1 = ServerBrowserDialog().GetServer(p1.userData);
	serveritem_t &s2 = ServerBrowserDialog().GetServer(p2.userData);

	return Q_stricmp(s1.gameDescription, s2.gameDescription);
}

int __cdecl ServerNameCompare(ListPanel *pPanel, const ListPanelItem &p1, const ListPanelItem &p2)
{
	serveritem_t &s1 = ServerBrowserDialog().GetServer(p1.userData);
	serveritem_t &s2 = ServerBrowserDialog().GetServer(p2.userData);

	return Q_stricmp(s1.name, s2.name);
}

int __cdecl PlayersCompare(ListPanel *pPanel, const ListPanelItem &p1, const ListPanelItem &p2)
{
	serveritem_t &s1 = ServerBrowserDialog().GetServer(p1.userData);
	serveritem_t &s2 = ServerBrowserDialog().GetServer(p2.userData);

	int s1p = max(0, s1.players - s1.botPlayers);
	int s1m = max(0, s1.maxPlayers - s1.botPlayers);
	int s2p = max(0, s2.players - s2.botPlayers);
	int s2m = max(0, s2.maxPlayers - s2.botPlayers);

	if (s1p > s2p)
		return -1;

	if (s1p < s2p)
		return 1;

	if (s1m > s2m)
		return -1;

	if (s1m < s2m)
		return 1;

	return 0;
}