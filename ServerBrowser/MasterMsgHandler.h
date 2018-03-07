#ifndef MASTERMSGHANDLER_H
#define MASTERMSGHANDLER_H
#ifdef _WIN32
#pragma once
#endif

#include "Socket.h"

class CMasterMsgHandler : public CMsgHandler
{
public:
	CMasterMsgHandler(IGameList *baseobject, HANDLERTYPE type, int typeinfo);

public:
	virtual bool Process(netadr_t *from, CMsgBuffer *msg);

private:
	IGameList *m_pGameList;
};

#endif