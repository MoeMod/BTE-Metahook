#ifndef SERVERLIST_H
#define SERVERLIST_H

#ifdef _WIN32
#pragma once
#endif

#include "server.h"
#include "netadr.h"
#include "serverinfo.h"
#include "iresponse.h"

#include "UtlVector.h"
#include "UtlRBTree.h"

class CSocket;
class IServerRefreshResponse;

struct query_t
{
	netadr_t addr;
	int serverID;
	float sendTime;
};

class CServerList : public IResponse
{
public:
	CServerList(IServerRefreshResponse *gameList);
	~CServerList(void);

public:
	void RunFrame(void);
	serveritem_t &GetServer(unsigned int serverID);
	unsigned int ServerCount(void);
	unsigned int AddNewServer(serveritem_t &server);
	void StartRefresh(void);
	void StopRefresh(void);
	void Clear(void);
	void AddServerToRefreshList(unsigned int serverID);
	bool IsRefreshing(void);
	int RefreshListRemaining(void);
	void ServerResponded(void);
	void ServerFailedToRespond(void);

private:
	void QueryFrame(void);
	int CalculateAveragePing(serveritem_t &server);

private:
	IServerRefreshResponse *m_pResponseTarget;

private:
	enum
	{
		MAX_QUERY_SOCKETS = 255,
	};

	CUtlRBTree<query_t, unsigned short> m_Queries;
	CUtlVector<CServerInfo *> m_ServerCaches;
	CUtlVector<CServerInfo *> m_Servers;
	CUtlVector<int> m_RefreshList;

	int m_iUpdateSerialNumber;
	bool m_bQuerying;
	int m_nMaxActive;
	int m_nMaxRampUp;
	int m_nRampUpSpeed;
	int m_nInvalidServers;
	int m_nRefreshedServers;
};

#endif