#ifndef SERVERINFOMSGHANDLERDETAILS_H
#define SERVERINFOMSGHANDLERDETAILS_H

#ifdef _WIN32
#pragma once
#endif

#include "Socket.h"

class CServerInfo;

class CServerInfoMsgHandlerDetails : public CMsgHandler
{
public:
	CServerInfoMsgHandlerDetails(CServerInfo *baseobject, HANDLERTYPE type, int typeinfo);

public:
	virtual bool Process(netadr_t *from, CMsgBuffer *msg);

private:
	CServerInfo *m_pServerInfo;
};

#endif