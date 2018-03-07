#ifndef SPECTATEGAMES_H
#define SPECTATEGAMES_H

#ifdef _WIN32
#pragma once
#endif

#include "InternetGames.h"

class CSpectateGames : public CInternetGames
{
public:
	CSpectateGames(vgui::Panel *parent);

protected:
	virtual void RequestServers(int Start, const char *filterString);
	virtual bool CheckPrimaryFilters(serveritem_t &server);

private:
	typedef CInternetGames BaseClass;
};

#endif