#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#ifdef _WIN32
#pragma once
#endif

#include "server.h"
#include "netadr.h"

class CSocket;
class IServerPlayersResponse;

class CPlayerInfo
{
public:
	CPlayerInfo(IServerPlayersResponse *target);
	~CPlayerInfo(void);

public:
	void Query(void);
	serveritem_t &GetServer(void);
	void SetServer(serveritem_t &server);
	void SetChallenge(int challenge);
	void RunFrame(void);
	void UpdatePlayerInfo(int index, const char *playerName, int score, float timePlayedSeconds);

public:
	int serverID;
	int received;

private:
	serveritem_t m_Server;
	CSocket *m_pQuery;

	IServerPlayersResponse *m_pResponseTarget;

	bool m_bIsRefreshing;
	float m_fSendTime;
	bool m_bRefreshed;
	int m_iChallenge;
};

#endif