#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
#pragma once
#endif

struct serveritem_t
{
	serveritem_t(void)
	{
		pings[0] = 0;
		pings[1] = 0;
		pings[2] = 0;
	}

	unsigned char ip[4];
	int port;

	inline bool operator==(const serveritem_t &other) const
	{
		return ip[0] == other.ip[0] && ip[1] == other.ip[1] && ip[2] == other.ip[2] && ip[3] == other.ip[3] && port == other.port;
	}

	int received;
	float time;
	int ping;
	int pings[3];
	bool hadSuccessfulResponse;
	bool doNotRefresh;
	char gameDir[32];
	char map[32];
	char gameDescription[64];
	char name[64];
	int players;
	int maxPlayers;
	int botPlayers;
	bool proxy;
	bool password;
	unsigned int serverID;
	int listEntryID;
	char rconPassword[64];
	bool loadedFromFile;
};

#endif
