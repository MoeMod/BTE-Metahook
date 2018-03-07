#include "getchallenge.h"
#include "IServerGetChallengeResponse.h"
#include "GetChallengeMsgHandler.h"
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

CGetChallenge::CGetChallenge(IServerGetChallengeResponse *target)
{
	m_pResponseTarget = target;

	m_pQuery = new CSocket("internet server query", -1);
	m_pQuery->AddMessageHandler(new CGetChallengeMsgHandlerDetails(this, CMsgHandler::MSGHANDLER_BYTECODE, S2C_CHALLENGE));

	m_fSendTime = 0;
}

CGetChallenge::~CGetChallenge(void)
{
	delete m_pQuery;
}

void CGetChallenge::Query(void)
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
	buffer->WriteByte(A2S_SERVERQUERY_GETCHALLENGE);

	m_pQuery->SendMessage(&adr);

	m_fSendTime = CSocket::GetClock();
}

void CGetChallenge::RunFrame(void)
{
	float curtime = CSocket::GetClock();

	if (m_fSendTime != 0 && (curtime - m_fSendTime) > 5.0f)
	{
		m_fSendTime = 0;
		m_pResponseTarget->GetChallengeFailedToRespond();
	}

	if (m_pQuery)
		m_pQuery->Frame();
}

void CGetChallenge::UpdateChallenge(int challenge)
{
	m_Server.received = INFO_RECEIVED;
	m_Server.hadSuccessfulResponse = true;

	m_fSendTime = 0;
	m_pResponseTarget->GetChallengeRefreshComplete(challenge);
}

serveritem_t &CGetChallenge::GetServer(void)
{
	return m_Server;
}

void CGetChallenge::SetServer(serveritem_t &server)
{
	memcpy(&m_Server, &server, sizeof(serveritem_t));
}