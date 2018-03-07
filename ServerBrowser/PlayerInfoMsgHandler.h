#ifndef PLAYERINFOMSGHANDLERDETAILS_H
#define PLAYERINFOMSGHANDLERDETAILS_H

#ifdef _WIN32
#pragma once
#endif

#include "Socket.h"

class CPlayerInfo;

class CPlayerInfoMsgHandlerDetails : public CMsgHandler
{
public:
	CPlayerInfoMsgHandlerDetails(CPlayerInfo *baseobject, HANDLERTYPE type, int typeinfo);

public:
	virtual bool Process(netadr_t *from, CMsgBuffer *msg);

private:
	CPlayerInfo *m_pPlayerInfo;
};

#endif