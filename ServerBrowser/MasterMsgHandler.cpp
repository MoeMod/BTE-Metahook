#include "MasterMsgHandler.h"
#include "server.h"
#include "IGameList.h"

CMasterMsgHandler::CMasterMsgHandler(IGameList *baseobject, HANDLERTYPE type, int typeinfo) : CMsgHandler(type, typeinfo)
{
	m_pGameList = baseobject;
}

bool CMasterMsgHandler::Process(netadr_t *from, CMsgBuffer *msg)
{
	msg->ReadByte();
	msg->ReadByte();

	int unique = msg->ReadLong();
	int nNumAddresses = msg->GetCurSize() - msg->GetReadCount();

	Assert(!(nNumAddresses % 6));

	nNumAddresses /= 6;

	while (nNumAddresses > 0)
	{
		serveritem_t server;
		memset(&server, 0, sizeof(server));

		for (int i = 0; i < 4; i++)
			server.ip[i] = msg->ReadByte();

		server.port = msg->ReadShort();
		server.port = (server.port & 0xff) << 8 | (server.port & 0xff00) >> 8;
		server.received = 0;
		server.listEntryID = m_pGameList->GetInvalidServerListID();
		server.doNotRefresh = false;
		server.hadSuccessfulResponse = false;
		server.map[0] = 0;
		server.gameDir[0] = 0;

		m_pGameList->AddNewServer(server);

		nNumAddresses--;
	}

	if (!unique)
		m_pGameList->ListReceived(false, 0);
	else
		m_pGameList->ListReceived(true, unique);

	return true;
}