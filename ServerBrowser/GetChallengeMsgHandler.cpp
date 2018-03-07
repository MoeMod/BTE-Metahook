#include "GetChallengeMsgHandler.h"
#include "getchallenge.h"

CGetChallengeMsgHandlerDetails::CGetChallengeMsgHandlerDetails(CGetChallenge *baseobject, HANDLERTYPE type, int typeinfo) : CMsgHandler(type, typeinfo)
{
	m_pGetChallenge = baseobject;
}

bool CGetChallengeMsgHandlerDetails::Process(netadr_t *from, CMsgBuffer *msg)
{
	int header = msg->ReadByte();
	int challenge = msg->ReadLong();

	m_pGetChallenge->UpdateChallenge(challenge);
	return true;
}