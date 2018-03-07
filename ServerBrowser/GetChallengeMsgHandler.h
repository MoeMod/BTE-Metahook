#ifndef GETCHALLENGEMSGHANDLERDETAILS_H
#define GETCHALLENGEMSGHANDLERDETAILS_H

#ifdef _WIN32
#pragma once
#endif

#include "Socket.h"

class CGetChallenge;

class CGetChallengeMsgHandlerDetails : public CMsgHandler
{
public:
	CGetChallengeMsgHandlerDetails(CGetChallenge *baseobject, HANDLERTYPE type, int typeinfo);

public:
	virtual bool Process(netadr_t *from, CMsgBuffer *msg);

private:
	CGetChallenge *m_pGetChallenge;
};

#endif