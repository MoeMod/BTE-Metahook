#include "serverinfo.h"
#include "IResponse.h"

#include "ServerInfoMsgHandler.h"
#include "Socket.h"
#include "proto_oob.h"

namespace
{
	const float SERVER_TIMEOUT = 5.0f;
}

typedef enum
{
	NONE = 0,
	INFO_REQUESTED,
	INFO_RECEIVED
}
RCONSTATUS;

CServerInfo::CServerInfo(IResponse *target)
{
	m_pResponseTarget = target;
	m_bIsRefreshing = false;
	m_bRefreshed = false;

	m_pQuery = new CSocket("internet server query", -1);
	m_pQuery->AddMessageHandler(new CServerInfoMsgHandlerDetails(this, CMsgHandler::MSGHANDLER_BYTECODE, S2A_INFO_GOLDSRC));
	m_fSendTime = 0;
}

CServerInfo::CServerInfo(IResponse *target, serveritem_t &server)
{
	SetServer(server);

	CServerInfo::CServerInfo(target);
}

CServerInfo::~CServerInfo(void)
{
	delete m_pQuery;
}

void CServerInfo::Query(void)
{
	CMsgBuffer *buffer = m_pQuery->GetSendBuffer();
	assert(buffer);

	if (!buffer)
		return;

	m_bIsRefreshing = true;
	m_bRefreshed = false;

	netadr_t adr;

	adr.ip[0] = m_Server.ip[0];
	adr.ip[1] = m_Server.ip[1];
	adr.ip[2] = m_Server.ip[2];
	adr.ip[3] = m_Server.ip[3];
	adr.port = (m_Server.port & 0xff) << 8 | (m_Server.port & 0xff00) >> 8;
	adr.type = NA_IP;

	m_Server.received = (int)INFO_REQUESTED;

	buffer->Clear();
	buffer->WriteLong(0xFFFFFFFF);
	buffer->WriteByte(A2S_INFO);
	buffer->WriteString(A2S_KEY_STRING);

	m_pQuery->SendMessage(&adr);
	m_fSendTime = CSocket::GetClock();
}

void CServerInfo::RunFrame(void)
{
	float curtime = CSocket::GetClock();

	if (m_fSendTime != 0 && (curtime - m_fSendTime) > 5.0f)
	{
		m_fSendTime = 0;
		m_pResponseTarget->ServerFailedToRespond();
	}

	if (m_pQuery)
		m_pQuery->Frame();
}

void CServerInfo::UpdateServer(netadr_t *adr, bool proxy, const char *serverName, const char *map, const char *gamedir, const char *gameDescription, int players, int maxPlayers, float recvTime, bool password)
{
	m_Server.received = INFO_RECEIVED;
	m_Server.hadSuccessfulResponse = true;

	Q_strncpy(m_Server.gameDir, gamedir, sizeof(m_Server.gameDir) - 1);
	Q_strncpy(m_Server.map, map, sizeof(m_Server.map) - 1);
	Q_strncpy(m_Server.name, serverName, sizeof(m_Server.name) - 1);
	Q_strncpy(m_Server.gameDescription, gameDescription, sizeof(m_Server.gameDescription) - 1);
	m_Server.players = players;
	m_Server.maxPlayers = maxPlayers;
	m_Server.proxy = proxy;
	m_Server.password = password;

	int ping = (int)((recvTime - m_fSendTime) * 1000);

	if (ping > 3000 || ping < 0)
		ping = 1200;

	m_Server.ping = ping;

	m_bIsRefreshing = false;
	m_bRefreshed = true;
	m_fSendTime = 0;

	m_pResponseTarget->ServerResponded();
}

void CServerInfo::Refresh(void)
{
	Query();
}

bool CServerInfo::IsRefreshing(void)
{
	return m_bIsRefreshing;
}

void CServerInfo::SetServer(serveritem_t &server)
{
	memcpy(&m_Server, &server, sizeof(serveritem_t));
}

serveritem_t &CServerInfo::GetServer(void)
{
	return m_Server;
}

bool CServerInfo::Refreshed(void)
{
	bool val = m_bRefreshed;
	m_bRefreshed = false;
	return val;
}