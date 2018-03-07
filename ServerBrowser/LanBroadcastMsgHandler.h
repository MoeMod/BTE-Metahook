#ifndef LANBROADCASTMSGHANDLER_H
#define LANBROADCASTMSGHANDLER_H

#ifdef _WIN32
#pragma once
#endif

#include "Socket.h"

class CLanBroadcastMsgHandler : public CMsgHandler
{
public:
	CLanBroadcastMsgHandler(IGameList *baseobject, HANDLERTYPE type, int typeinfo);

public:
	virtual bool Process(netadr_t *from, CMsgBuffer *msg);

public:
	void SetRequestTime(float flRequestTime);

private:
	IGameList *m_pGameList;
	float m_flRequestTime;
};

#endif