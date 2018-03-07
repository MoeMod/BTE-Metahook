#include "PlayerInfoMsgHandler.h"
#include "playerinfo.h"

CPlayerInfoMsgHandlerDetails::CPlayerInfoMsgHandlerDetails(CPlayerInfo *baseobject, HANDLERTYPE type, int typeinfo) : CMsgHandler(type, typeinfo)
{
	m_pPlayerInfo = baseobject;
}

bool CPlayerInfoMsgHandlerDetails::Process(netadr_t *from, CMsgBuffer *msg)
{
	int header = msg->ReadByte();
	int players = msg->ReadByte();

	for (int i = 0; i < players; i++)
	{
		int index = msg->ReadByte();
		char *name = msg->ReadString();
		int score = msg->ReadLong();
		float duration = msg->ReadFloat();

		m_pPlayerInfo->UpdatePlayerInfo(index, name, score, duration);
	}

	return true;
}