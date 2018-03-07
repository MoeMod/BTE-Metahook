//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef CREATEMULTIPLAYERGAMESERVERPAGE_H
#define CREATEMULTIPLAYERGAMESERVERPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>
#include "cvartogglecheckbutton.h"
#include <UtlRBTree.h>
#include <UtlSymbol.h>

#define MAX_GAMEMODES 1000

namespace vgui
{
class ListPanel;
}

//-----------------------------------------------------------------------------
// Purpose: server options page of the create game server dialog
//-----------------------------------------------------------------------------
class CCreateMultiplayerGameServerPage : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE( CCreateMultiplayerGameServerPage, vgui::PropertyPage );

public:
	CCreateMultiplayerGameServerPage(vgui::Panel *parent, const char *name);
	~CCreateMultiplayerGameServerPage();

	// returns currently entered information about the server
	void SetMap(const char *name);
	bool IsRandomMapSelected();

	void SetMode(const int id);
	void UpdateMode();

	const char *GetMapName();
	const int GetModeID();

	// CS Bots
	void SetBotQuota( int quota );
	void SetBotsEnabled( bool enabled );
	int GetBotQuota( void );
	bool GetBotsEnabled( void );

protected:
	virtual void OnApplyChanges();
	MESSAGE_FUNC( OnCheckButtonChecked, "CheckButtonChecked" );

private:
	void LoadMapList();
	void LoadMaps( const char *pszPathID );

	void LoadModeList();
	void UpdateCurrentMode(int i);

	vgui::ComboBox *m_pMapList,*m_pModeList;
	vgui::TextEntry *m_pBotQuotaCombo;
	vgui::CheckButton *m_pEnableBotsCheck;
	CCvarToggleCheckButton *m_pEnableTutorCheck;

	enum { DATA_STR_LENGTH = 64 };
	char m_szHostName[DATA_STR_LENGTH];
	char m_szPassword[DATA_STR_LENGTH];
	char m_szMapName[DATA_STR_LENGTH];

	int m_iMaxPlayers;
	CUtlRBTree<CUtlSymbol> m_MapNames;
};


#endif // CREATEMULTIPLAYERGAMESERVERPAGE_H
