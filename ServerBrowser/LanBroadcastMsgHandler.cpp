#include "LanBroadcastMsgHandler.h"
#include "server.h"
#include "IGameList.h"

CLanBroadcastMsgHandler::CLanBroadcastMsgHandler(IGameList *baseobject, HANDLERTYPE type, int typeinfo) : CMsgHandler(type, typeinfo)
{
	m_pGameList = baseobject;
}

bool CLanBroadcastMsgHandler::Process(netadr_t *from, CMsgBuffer *msg)
{
	char name[256], map[256], gamedir[256], desc[256];

	int header = msg->ReadByte();
	const char *str = msg->ReadString();

	if (!str[0])
		return false;

	str = msg->ReadString();

	if (!str[0])
		return false;

	Q_strncpy(name, str, 255);

	str = msg->ReadString();

	if (!str[0])
		return false;

	Q_strncpy(map, str, 255);

	str = msg->ReadString();

	if (!str[0])
		return false;

	Q_strncpy(gamedir, str, 255);
	strlwr(gamedir);

	str = msg->ReadString();

	if (!str[0])
		return false;

	Q_strncpy(desc, str, 255);

	int players = msg->ReadByte();
	int maxplayers = msg->ReadByte();
	int protocol = msg->ReadByte();
	char serverType = msg->ReadByte();
	char serverOS = msg->ReadByte();
	bool password = msg->ReadByte() != 0;
	bool isMod = msg->ReadByte() != 0;

	serveritem_t server;
	memset(&server, 0, sizeof(server));

	for (int i = 0; i < 4; i++)
		server.ip[i] = from->ip[i];

	server.port = (from->port & 0xff) << 8 | (from->port & 0xff00) >> 8;
	server.doNotRefresh = false;
	server.hadSuccessfulResponse = true;

	Q_strncpy(server.name, name, sizeof(server.name));
	Q_strncpy(server.map, map, sizeof(server.map));
	Q_strncpy(server.gameDir, gamedir, sizeof(server.gameDir));
	Q_strncpy(server.gameDescription, desc, sizeof(server.gameDescription));
	server.players = players;
	server.maxPlayers = maxplayers;
	server.proxy = (serverType == 'p');
	server.ping = (int)((CSocket::GetClock() - m_flRequestTime) * 1000.0f);

	m_pGameList->AddNewServer(server);
	return true;
}

void CLanBroadcastMsgHandler::SetRequestTime(float flRequestTime)
{
	m_flRequestTime = flRequestTime;
}