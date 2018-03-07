#ifndef SERVERINFO_H
#define SERVERINFO_H

#ifdef _WIN32
#pragma once
#endif

#include "server.h"
#include "netadr.h"

class CSocket;
class IResponse;

class CServerInfo
{
public:
	CServerInfo(IResponse *target, serveritem_t &server);
	CServerInfo(IResponse *target);
	~CServerInfo(void);

public:
	void Query(void);
	void Refresh(void);
	bool IsRefreshing(void);
	void SetServer(serveritem_t &server);
	serveritem_t &GetServer(void);
	void RunFrame(void);
	bool Refreshed(void);
	void UpdateServer(netadr_t *adr, bool proxy, const char *serverName, const char *map, const char *gamedir, const char *gameDescription, int players, int maxPlayers, float recvTime, bool password);

public:
	int serverID;
	int received;

private:
	serveritem_t m_Server;
	CSocket *m_pQuery;

	IResponse *m_pResponseTarget;

	bool m_bIsRefreshing;
	float m_fSendTime;
	bool m_bRefreshed;
};

#endif