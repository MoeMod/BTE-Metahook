#include "ServerInfoMsgHandler.h"
#include "serverinfo.h"

CServerInfoMsgHandlerDetails::CServerInfoMsgHandlerDetails(CServerInfo *baseobject, HANDLERTYPE type, int typeinfo) : CMsgHandler(type, typeinfo)
{
	m_pServerInfo = baseobject;
}

bool CServerInfoMsgHandlerDetails::Process(netadr_t *from, CMsgBuffer *msg)
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

	m_pServerInfo->UpdateServer(from, (serverType == 'p'), name, map, gamedir, desc, players, maxplayers, msg->GetTime(), password);
	return true;
}