#ifndef GETCHALLENGE_H
#define GETCHALLENGE_H

#ifdef _WIN32
#pragma once
#endif

#include "server.h"
#include "netadr.h"

class CSocket;
class IServerGetChallengeResponse;

class CGetChallenge
{
public:
	CGetChallenge(IServerGetChallengeResponse *target);
	~CGetChallenge(void);

public:
	void Query(void);
	serveritem_t &GetServer(void);
	void SetServer(serveritem_t &server);
	void RunFrame(void);
	void UpdateChallenge(int challenge);

public:
	int serverID;
	int received;

private:
	serveritem_t m_Server;
	CSocket *m_pQuery;

	IServerGetChallengeResponse *m_pResponseTarget;

	bool m_bIsRefreshing;
	float m_fSendTime;
	bool m_bRefreshed;
};

#endif