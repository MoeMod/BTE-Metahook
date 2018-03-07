#include "IServerRefreshResponse.h"
#include "ServerList.h"
#include "Socket.h"
#include "proto_oob.h"
#include "sys.h"

typedef enum
{
	NONE = 0,
	INFO_REQUESTED,
	INFO_RECEIVED
}
QUERYSTATUS;

#define min(a, b) (((a) < (b)) ? (a) : (b))

bool QueryLessFunc(const query_t &item1, const query_t &item2)
{
	if (item1.addr.port < item2.addr.port)
		return true;
	else if (item1.addr.port > item2.addr.port)
		return false;

	int ip1 = *(int *)&item1.addr.ip;
	int ip2 = *(int *)&item2.addr.ip;

	return ip1 < ip2;
}

CServerList::CServerList(IServerRefreshResponse *target) : m_Queries(0, MAX_QUERY_SOCKETS, QueryLessFunc)
{
	static char speedBuf[32] = "";

	if (!strlen(speedBuf))
		Sys_GetRegKeyValueUnderRoot("Software\\Valve\\Tracker", "Rater", speedBuf, sizeof(speedBuf) - 1, "7500");

	m_pResponseTarget = target;
	m_iUpdateSerialNumber = 1;

	int internetSpeed = atoi(speedBuf);
	int maxSockets = (MAX_QUERY_SOCKETS * internetSpeed) / 10000;

	if (internetSpeed < 6000)
		maxSockets /= 2;

	m_nMaxActive = maxSockets;

	m_nRampUpSpeed = 1;
	m_bQuerying = false;
	m_nMaxRampUp = 1;

	m_nInvalidServers = 0;
	m_nRefreshedServers = 0;
}

CServerList::~CServerList(void)
{
	Clear();
}

void CServerList::RunFrame(void)
{
	for (int i = 0; i < m_Servers.Count(); i++)
		m_Servers[i]->RunFrame();

	QueryFrame();
}

serveritem_t &CServerList::GetServer(unsigned int serverID)
{
	if (m_Servers.IsValidIndex(serverID))
		return m_Servers[serverID]->GetServer();

	static serveritem_t dummyServer;
	memset(&dummyServer, 0, sizeof(dummyServer));
	return dummyServer;
}

unsigned int CServerList::ServerCount(void)
{
	return m_Servers.Count();
}

int CServerList::RefreshListRemaining(void)
{
	return m_RefreshList.Count();
}

bool CServerList::IsRefreshing(void)
{
	return m_bQuerying;
}

unsigned int CServerList::AddNewServer(serveritem_t &server)
{
	// No more same server
	for (unsigned int i = 0; i < ServerCount(); i++)
	{
		if (server == m_Servers[i]->GetServer())
		{
			return m_Servers[i]->serverID;
		}
	}

	unsigned int serverID = m_Servers.AddToTail();

	m_Servers[serverID] = new CServerInfo(this);
	m_Servers[serverID]->SetServer(server);
	m_Servers[serverID]->serverID = serverID;
	return serverID;
}

void CServerList::Clear(void)
{
	StopRefresh();

	for (int i = 0; i < m_Servers.Size(); i++)
		delete m_Servers[i];

	m_Servers.RemoveAll();
}

void CServerList::StopRefresh(void)
{
	for (unsigned int i = 0; i < ServerCount(); i++)
		m_Servers[i]->received = 0;

	m_Queries.RemoveAll();
	m_RefreshList.RemoveAll();

	m_iUpdateSerialNumber++;
	m_nInvalidServers = 0;
	m_nRefreshedServers = 0;
	m_bQuerying = false;
	m_nMaxRampUp = m_nRampUpSpeed;
}

void CServerList::AddServerToRefreshList(unsigned int serverID)
{
	if (!m_Servers.IsValidIndex(serverID))
		return;

	serveritem_t &server = m_Servers[serverID]->GetServer();
	server.received = NONE;

	m_RefreshList.AddToTail(serverID);
}

void CServerList::StartRefresh(void)
{
	if (m_RefreshList.Count() > 0)
		m_bQuerying = true;
}

void CServerList::ServerResponded(void)
{
	for (int i = 0; i < m_Servers.Count(); i++)
	{
		if (m_Servers[i]->Refreshed())
		{
			serveritem_t &server = m_Servers[i]->GetServer();

			server.pings[0] = server.pings[1];
			server.pings[1] = server.pings[2];
			server.pings[2] = server.ping;

			int ping = CalculateAveragePing(server);

			if (server.ping == ping && ping > 0)
				ping--;

			if (ping == 0)
				ping = 1;

			server.ping = ping;
			server.received = INFO_RECEIVED;

			netadr_t adr;
			adr.ip[0] = server.ip[0];
			adr.ip[1] = server.ip[1];
			adr.ip[2] = server.ip[2];
			adr.ip[3] = server.ip[3];
			adr.port = (server.port & 0xff) << 8 | (server.port & 0xff00) >> 8;
			adr.type = NA_IP;

			query_t finder;
			finder.addr = adr;

			m_Queries.Remove(finder);
			m_pResponseTarget->ServerResponded(server);
		}
	}
}

void CServerList::ServerFailedToRespond(void)
{
}

int CServerList::CalculateAveragePing(serveritem_t &server)
{
	if (server.pings[0] && server.pings[0] < 1200)
	{
		int middlePing = 0, lowPing = 1, highPing = 2;

		if (server.pings[0] < server.pings[1])
		{
			if (server.pings[0] > server.pings[2])
			{
				lowPing = 2;
				middlePing = 0;
				highPing = 1;
			}
			else if (server.pings[1] < server.pings[2])
			{
				lowPing = 0;
				middlePing = 1;
				highPing = 2;
			}
			else
			{
				lowPing = 0;
				middlePing = 2;
				highPing = 1;
			}
		}
		else
		{
			if (server.pings[1] > server.pings[2])
			{
				lowPing = 2;
				middlePing = 1;
				highPing = 0;
			}
			else if (server.pings[0] < server.pings[2])
			{
				lowPing = 1;
				middlePing = 0;
				highPing = 2;
			}
			else
			{
				lowPing = 1;
				middlePing = 2;
				highPing = 0;
			}
		}

		if ((server.pings[middlePing] - server.pings[lowPing]) < (server.pings[highPing] - server.pings[middlePing]))
			return (server.pings[middlePing] + server.pings[lowPing]) / 2;
		else
			return (server.pings[middlePing] + server.pings[highPing]) / 2;
	}
	else if (server.pings[1] && server.pings[1] < 1200)
	{
		return (server.pings[1] + server.pings[2]) / 2;
	}
	else if (server.pings[2] && server.pings[2] < 1200)
	{
		return server.pings[2];
	}

	return 0;
}

void CServerList::QueryFrame(void)
{
	if (!m_bQuerying)
		return;

	float curtime = CSocket::GetClock();
	unsigned short idx = m_Queries.FirstInorder();

	while (m_Queries.IsValidIndex(idx))
	{
		query_t &query = m_Queries[idx];

		if ((curtime - query.sendTime) > 1.2f)
		{
			Assert(m_Servers.IsValidIndex(query.serverID));

			serveritem_t &item = m_Servers[query.serverID]->GetServer();
			item.pings[0] = item.pings[1];
			item.pings[1] = item.pings[2];
			item.pings[2] = 1200;
			item.ping = 1200;

			if (!item.hadSuccessfulResponse)
			{
				item.doNotRefresh = true;
				m_nInvalidServers++;
			}

			m_pResponseTarget->ServerFailedToRespond(item);
			item.received = false;

			unsigned short nextidx = m_Queries.NextInorder(idx);

			m_Queries.RemoveAt(idx);
			idx = nextidx;
		}
		else
		{
			idx = m_Queries.NextInorder(idx);
		}
	}

	m_nMaxRampUp = min(m_nMaxActive, m_nMaxRampUp + m_nRampUpSpeed);

	while (m_RefreshList.Count() > 0 && (int)m_Queries.Count() < m_nMaxRampUp)
	{
		int currentServer = m_RefreshList[0];

		if (!m_Servers.IsValidIndex(currentServer))
			break;

		serveritem_t &item = m_Servers[currentServer]->GetServer();
		item.time = curtime;

		m_Servers[currentServer]->Query();

		netadr_t adr;
		adr.ip[0] = item.ip[0];
		adr.ip[1] = item.ip[1];
		adr.ip[2] = item.ip[2];
		adr.ip[3] = item.ip[3];
		adr.port = (item.port & 0xff) << 8 | (item.port & 0xff00) >> 8;
		adr.type = NA_IP;

		query_t query;
		query.addr = adr;
		query.sendTime = curtime;
		query.serverID = currentServer;

		m_Queries.Insert(query);
		m_RefreshList.Remove((int)0);
	}

	if (m_Queries.Count() < 1)
	{
		m_bQuerying = false;
		m_pResponseTarget->RefreshComplete();
		m_iUpdateSerialNumber++;
	}
}