#include "base.h"
#include "exportfuncs.h"
#include "hud.h"
#include "message.h"
#include "parsemsg.h"
#include "DrawTargaImage.h"
#include "HUD3D_ZB.h"


#include "weapons.h"
#include "Client/WeaponManager.h"

#include "ammo.h"
#include "deathmsg.h"
#include "alarm.h"
#include "playbink.h"
#include "gdboard.h"
#include "DrawTABPanel.h"
#include "Statistics.h"
#include "Client/HUD/DrawTGA.h"

static CHudDeathNotice g_HudDeathNotice;
CHudDeathNotice &HudDeathNotice()
{
	return g_HudDeathNotice;
}

inline int IsKnife(char *szWpnName)
{
#if 0
	return (strstr(szWpnName,"axe") || strstr(szWpnName,"hdagger")||strstr(szWpnName,"combat")
		|| strstr(szWpnName,"hammer")||strstr(szWpnName,"katana")||strstr(szWpnName,"skullaxe")
		|| strstr(szWpnName,"d_knife") || strstr(szWpnName,"tomahawk") || strstr(szWpnName,"dragontail")
		|| strstr(szWpnName,"sword") || strstr(szWpnName,"d_zsh_"));
#endif
	/*
	static int iBteWpn = 0;
	iBteWpn = FindWeaponData(szWpnName);
	if (iBteWpn < 1 || iBteWpn > 512)
		return 0;
	return g_WeaponData[iBteWpn].iSlot == 3;*/

	return WeaponManager()[szWpnName].iSlot == 3;
}


inline int IsGrenade(char *szWpnName)
{
	/*static int iBteWpn = 0;
	iBteWpn = FindWeaponData(szWpnName);
	if (iBteWpn < 1 || iBteWpn > 512)
		return 0;
	return g_WeaponData[iBteWpn].iSlot == 4;*/
	return WeaponManager()[szWpnName].iSlot == 4;
}

int CountPlayers(int iTeam, int iAlive)
{
	static int iMaxClients = gEngfuncs.GetMaxClients();
	static int iPlayerCount;
	iPlayerCount = 0;
	if (!iTeam)
	{
		for (int i = 1; i < iMaxClients; i++)
		{
			if (!iValidPlayer[i]) continue;
			if (g_PlayerExtraInfo[i].iFlag & SCOREATTRIB_DEAD && iAlive) continue;

			iPlayerCount++;
		}
	}
	else
	{
		for (int i = 1; i < iMaxClients; i++)
		{
			if (!iValidPlayer[i]) continue;
			if (g_PlayerExtraInfo[i].iFlag & SCOREATTRIB_DEAD && iAlive) continue;
			if (vPlayer[i].team != iTeam) continue;
			iPlayerCount++;
		}
	}
	return iPlayerCount;
}

int RankFirst(int iTeam)
{
	if (!iTeam)
		return HudTABBoard().m_iSortedId[1];
	for (int i = 1; i < 33; i++)
	{
		if (!HudTABBoard().m_bValidPlayer[HudTABBoard().m_iSortedId[i]]) continue;
		if (vPlayer[HudTABBoard().m_iSortedId[i]].team != iTeam) continue;
			return HudTABBoard().m_iSortedId[i];
	}
	return 0;
}

int RankLast(int iTeam)
{
	if (!iTeam)
		return HudTABBoard().m_iSortedId[CountPlayers(iTeam, FALSE)];
	int iTeamCount = CountPlayers(iTeam, FALSE);
	int iCount = 0;
	for (int i = 1; i < 33; i++)
	{
		if (!HudTABBoard().m_bValidPlayer[HudTABBoard().m_iSortedId[i]]) continue;
		if (vPlayer[HudTABBoard().m_iSortedId[i]].team != iTeam) continue;
		iCount++;
		if (iCount == iTeamCount)
			return HudTABBoard().m_iSortedId[i];
	}
	return 0;
}

void CHudDeathNotice::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
	
	m_iInfects = m_iKills = m_iTotalKills = m_iDeaths = m_iRoundDidNotKill = 0;
	m_bLastRoundKilled = 1;
	m_bFirstKill = TRUE;
}

void CHudDeathNotice::VidInit(void)
{
	// Killer
	m_iKillCount = 0;
	m_fLastKillTime = 0.0;
	m_iTotalKills = 0;
	m_iDeathTga[0][0] = Hud().m_TGA.FindTexture("resource\\hud\\deathnotice\\killbg_left");
	m_iDeathTga[0][1] = Hud().m_TGA.FindTexture("resource\\hud\\deathnotice\\killbg_center");
	m_iDeathTga[0][2] = Hud().m_TGA.FindTexture("resource\\hud\\deathnotice\\killbg_right");
	m_iDeathTga[1][0] = Hud().m_TGA.FindTexture("resource\\hud\\deathnotice\\deathbg_left");
	m_iDeathTga[1][1] = Hud().m_TGA.FindTexture("resource\\hud\\deathnotice\\deathbg_center");
	m_iDeathTga[1][2] = Hud().m_TGA.FindTexture("resource\\hud\\deathnotice\\deathbg_right");
	memset(m_rgDeathNoticeList,0,sizeof(m_rgDeathNoticeList));

	m_iInfects = m_iKills = m_iTotalKills = m_iDeaths = m_iRoundDidNotKill = 0;
	m_bLastRoundKilled = 1;
	m_bFirstKill = TRUE;
} 

void CHudDeathNotice::OnPlayerSpawn()
{
	if (!m_bLastRoundKilled)
		m_iRoundDidNotKill++;
	m_iInfects = m_iKills = m_iTotalKills = 0;
}

void CHudDeathNotice::OnRoundStart()
{
	m_bFirstKill = TRUE;
	m_iKills = m_iTotalKills = 0;
	HudAlarm().ClearAllHappenedTimes();
}

inline int DrawConsoleString( int x, int y, const char *string )
{
	return gEngfuncs.pfnDrawConsoleString( x, y, (char*) string );
}

inline void GetConsoleStringSize( const char *string, int *width, int *height )
{
	gEngfuncs.pfnDrawConsoleStringLen( string, width, height );
}

inline int ConsoleStringLen( const char *string )
{
	int _width, _height;
	GetConsoleStringSize( string, &_width, &_height );
	return _width;
}

int GetClientColor( int clientIndex )
{
    int x = vPlayer[clientIndex].team;
	return vPlayer[clientIndex].team;
}
int CHudDeathNotice::MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf)
{	
	BEGIN_READ(pbuf, iSize);
	int iKiller = READ_BYTE();
	int iVictim = READ_BYTE();
	int iHeadShot = READ_BYTE();
	char szWpnName[2048];
	sprintf(szWpnName, "d_%s",READ_STRING());

	int iAssist = READ_BYTE();

	if(iKiller && iVictim)
	{
		if (g_iMod == MOD_ZB5 && vPlayer[iKiller].team != vPlayer[iVictim].team)
			gHud3D_ZB.SetAtk(1, 0.05);
		
		if (g_iMod == MOD_ZB || g_iMod == MOD_ZB2 || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4 || g_iMod == MOD_ZB5 || g_iMod == MOD_ZSE)
		{
			if (g_iTeam == 1 && vPlayer[iKiller].team == 2 || (!g_iTeam || g_iTeam == 3))
			{
				vPlayer[iVictim].fInfectTime = Hud().m_flTime + 3.0f;
				vPlayer[iVictim].vLastPos = gEngfuncs.GetEntityByIndex(iVictim)->origin;
			}
		}
		else if (g_iTeam == vPlayer[iVictim].team || (!g_iTeam || g_iTeam == 3))
		{
			vPlayer[iVictim].fDeathTime = Hud().m_flTime + 3.0f;
			vPlayer[iVictim].killtime = Hud().m_flTime + 3.0f;
			vPlayer[iVictim].vLastPos = gEngfuncs.GetEntityByIndex(iVictim)->origin;
		}
	}
	int i = 0;
	for (i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( m_rgDeathNoticeList[i].iId == 0 )
			break;
	}
	if ( i == MAX_DEATHNOTICES )
	{
		memmove( m_rgDeathNoticeList, m_rgDeathNoticeList+1, sizeof(DeathNoticeItem) * MAX_DEATHNOTICES );
		i = MAX_DEATHNOTICES - 1;
	}

	hud_player_info_t hPlayer;
	gEngfuncs.pfnGetPlayerInfo(iKiller,&hPlayer);
	char *killer_name = hPlayer.name;
	if ( !killer_name )
	{
		killer_name = "";
		m_rgDeathNoticeList[i].szKiller[0] = 0;
	}
	else
	{
		strncpy( m_rgDeathNoticeList[i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH );
		m_rgDeathNoticeList[i].szKiller[MAX_PLAYER_NAME_LENGTH-1] = 0;
	}
	// Get the Victim's name
	char *victim_name = NULL;
	// If victim is -1, the killer killed a specific, non-player object (like a sentrygun)
	if ( ((char)iVictim) != -1 )
	{
		gEngfuncs.pfnGetPlayerInfo(iVictim,&hPlayer);
		victim_name = hPlayer.name;
	}
	if ( !victim_name )
	{
		victim_name = "";
		m_rgDeathNoticeList[i].szVictim[0] = 0;
	}
	else
	{
		strncpy( m_rgDeathNoticeList[i].szVictim, victim_name, MAX_PLAYER_NAME_LENGTH );
		m_rgDeathNoticeList[i].szVictim[MAX_PLAYER_NAME_LENGTH-1] = 0;
	}
	// Is it a non-player object kill?
	if ( ((char)iVictim) == -1 )
	{
		m_rgDeathNoticeList[i].iNonPlayerKill = TRUE;

		// Store the object's name in the Victim slot (skip the d_ bit)
		strcpy( m_rgDeathNoticeList[i].szVictim, szWpnName+2 );
	}
	else
	{
		if ( iKiller == iVictim || iKiller == 0 )
			m_rgDeathNoticeList[i].iSuicide = TRUE;
	}

	int iIndex= gEngfuncs.GetLocalPlayer()->index;

	m_rgDeathNoticeList[i].iId = Hud().GetSpriteIndex(szWpnName);
	m_rgDeathNoticeList[i].flDisplayTime = Hud().m_flTime + 6.0f;
	m_rgDeathNoticeList[i].iHeadShot = iHeadShot;
	m_rgDeathNoticeList[i].Killer = vPlayer[iKiller].team;
	m_rgDeathNoticeList[i].Victim = vPlayer[iVictim].team;

	if(iKiller == iIndex)
	{
		if ((vPlayer[iIndex].team != vPlayer[iVictim].team || g_iMod == MOD_DM) && (g_iMod != MOD_GHOST || vPlayer[iVictim].team != 2))
			HudAmmo().Hit(Hud().m_flTime);

		if ((vPlayer[iIndex].team != vPlayer[iVictim].team || g_iMod == MOD_DM))
		{
			HudStatistics().m_iKill++;
			HudStatistics().m_iHit++;

			if (iHeadShot)
				HudStatistics().m_iHs++;
		}
	}

	if(g_iMod == MOD_DM)
	{
		if(iKiller != iIndex && iVictim != iIndex)
		{
			m_rgDeathNoticeList[i].Killer = m_rgDeathNoticeList[i].Victim = 2;
		}
		
		if(iKiller == iIndex && iVictim != iIndex)
		{
			m_rgDeathNoticeList[i].Killer = 1;
			m_rgDeathNoticeList[i].Victim = 2;
		}

		if(iVictim == iIndex && iKiller != iIndex)
		{
			m_rgDeathNoticeList[i].Killer = 2;
			m_rgDeathNoticeList[i].Victim = 1;
		}

	}
	
	if(iIndex == iVictim)
	{
		HudStatistics().m_iDeath++;
		HudAlarm().SetClearAssistTimes();
		if (iIndex != iKiller)
		{
			m_iDeaths++;
		}
		m_iKillCount = m_iTotalKills = 0;
	}

	if(iIndex == iKiller)
	{
		m_rgDeathNoticeList[i].iLocal = 1;
		m_iTotalKills += 1;
	}
	else if(iIndex == iVictim)
	{
		m_rgDeathNoticeList[i].iLocal = 2;
		m_iTotalKills = 0;		
	}
	else m_rgDeathNoticeList[i].iLocal = 0;

	if (iVictim==iIndex)
	{
		if (iKiller == iIndex || (IsKnife(szWpnName) && g_iMod != MOD_ZB && g_iMod != MOD_ZB2 && g_iMod != MOD_ZB3 && g_iMod != MOD_ZB4 && g_iMod != MOD_ZB5 && g_iMod != MOD_ZSE))
		{	char szCmd[256];
			sprintf(szCmd,"speak %s","ohno");
			gEngfuncs.pfnClientCmd(szCmd);
		}

		if (g_iMod == MOD_ZSE) // HM killed by ZB
		{
			if (vPlayer[iKiller].team == 2 && iKiller != iIndex)
			{				
				g_iZBNvg = 1;
				g_flZESFade = -1;
				//gEngfuncs.pfnClientCmd("nvgon");

				HudBinkPlayer().SetPos(ScreenWidth / 2, ScreenWidth / 2, 1);
				HudBinkPlayer().SetColor(255, 255, 255);

				HudBinkPlayer().BinkInstance("cstrike\\resource\\hud\\zombie\\infection.bik", 1, 0);
				HudBinkPlayer().BinkInstance("cstrike\\resource\\hud\\zombie\\infection.bik", 0, 0);
			}
		}
	}

	if (iKiller != iIndex && m_bFirstKill)
		m_bFirstKill = 0;

	if(iKiller==iIndex)
	{
		if (iKiller != iVictim)
		{
			bool bAddedAlarm = false;
			
			//gEngfuncs.pfnGetPlayerInfo(iKiller, &hPlayer);
			
			if (cl.time - m_fLastKillTime < 3.0)
				m_iKillCount++;
			else
				m_iKillCount=1;
			m_fLastKillTime = cl.time;
			m_iKillCount = m_iKillCount> 4 ? 4 : m_iKillCount;

			int iIsZombie = 0;
			if (IS_ZOMBIE_MODE && vPlayer[gEngfuncs.GetLocalPlayer()->index].team == 2)
				iIsZombie = 1;

			if (iIsZombie)
				m_iInfects++;
			else
				m_iKills++;

			if (IS_ZOMBIE_MODE)
			{
				if (m_iKills >= 6 && !HudAlarm().GetHappenedTimes(ALARM_LIBERATOR))
					HudAlarm().AddAlarm(ALARM_LIBERATOR);
				if (m_iInfects >= 8 && !HudAlarm().GetHappenedTimes(ALARM_INFECTOR))
					HudAlarm().AddAlarm(ALARM_INFECTOR);
				if (m_iKills >= 2 && m_iInfects >= 3 && !HudAlarm().GetHappenedTimes(ALARM_ALLROUND))
					HudAlarm().AddAlarm(ALARM_ALLROUND);
			}

			if (m_iTotalKills > 4 && m_iTotalKills < 26)
			{
				int iCount = m_iTotalKills % 5;
				if (!iCount)
				{
					bAddedAlarm = true;
					
					HudAlarm().AddAlarm(HudAlarm().GetPrivate(6 + m_iTotalKills / 5), false);
					if (IsKnife(szWpnName) && !iIsZombie)
						HudAlarm().AddAlarm(ALARM_KNIFE);
					if (IsGrenade(szWpnName))
						HudAlarm().AddAlarm(ALARM_GRENADE);
					if (iHeadShot)
						HudAlarm().AddAlarm(ALARM_HEADSHOT);
				}
				if (m_iTotalKills == 25)
					m_iTotalKills = 0;
			}
			else if (m_iTotalKills > 25)
				m_iTotalKills = 0;

			if (m_iKillCount > 1)
			{
				bAddedAlarm = true;
				HudAlarm().AddAlarm(HudAlarm().GetPrivate(2 + m_iKillCount), bAddedAlarm);
					
			}

			if (m_bFirstKill)
			{
				m_bFirstKill = 0;
				bAddedAlarm = true;
				HudAlarm().AddAlarm(ALARM_FIRSTBLOOD, false);
			}

			if (/*IsKnife(szWpnName)*/g_iCurrentWeapon == WEAPON_KNIFE && !iIsZombie)
			{
				bAddedAlarm = true;
				HudAlarm().AddAlarm(ALARM_KNIFE);
			}
			if (IsGrenade(szWpnName))
			{
				bAddedAlarm = true;
				HudAlarm().AddAlarm(ALARM_GRENADE);
			}
			if (iHeadShot)
			{
				bAddedAlarm = true;
				HudAlarm().AddAlarm(ALARM_HEADSHOT);
			}

			bool bTheLast = 1;
			for (int i = 1; i<33; i++)
			{
				if (vPlayer[i].team == vPlayer[iVictim].team && vPlayer[i].alive)
				{
					bTheLast = 0;
					break;
				}
			}
			if (bTheLast)
			{
				bAddedAlarm = true;
				HudAlarm().AddAlarm(ALARM_THELAST);
			}

			if (m_iDeaths >= 5 && m_iRoundDidNotKill <= 3)
			{
				m_iDeaths = 0;
				m_iRoundDidNotKill = 0;
				bAddedAlarm = true;
				HudAlarm().AddAlarm(ALARM_WELCOME);
			}
			else if (m_iDeaths >= 2 || m_iRoundDidNotKill >= 3)
			{
				m_iDeaths = 0;
				m_iRoundDidNotKill = 0;
				bAddedAlarm = true;
				HudAlarm().AddAlarm(ALARM_COMEBACK);
			}

			if (!bAddedAlarm)
				HudAlarm().AddAlarm(ALARM_1KILL, false);
			
			for (int iAlarm = 0; iAlarm < MAX_ALARM_TYPES + 1; iAlarm++)
			{
				if (HudAlarm().GetAlarm(iAlarm))
				{
					HudAlarm().AddAlarm(HudAlarm().GetPrivate(iAlarm));
					HudAlarm().RemovePrepareAlarmBits(iAlarm);
				}
			}
			
			if (vPlayer[iVictim].team)
			{
				int iFirst = RankFirst(vPlayer[iVictim].team);
				int iLast = RankLast(vPlayer[iVictim].team);

				if (iFirst != iLast && g_PlayerExtraInfo[iFirst].frags > 0)
				{
					if (iVictim == iFirst)
						HudAlarm().AddAlarm(ALARM_KINGMURDER);
					else if (iVictim == iLast)
						HudAlarm().AddAlarm(ALARM_BACKMARKER);
				}
			}
		}
	}

	hud_player_info_t hAssist;

	if (iAssist > 0 && iAssist < 33 && !m_rgDeathNoticeList[i].iSuicide && iKiller && iVictim)
	{
		hud_player_info_t info;
		gEngfuncs.pfnGetPlayerInfo(iKiller, &info);
		gEngfuncs.pfnGetPlayerInfo(iAssist, &hAssist);
		sprintf_s(m_rgDeathNoticeList[i].szKiller, "%s + %s", info.name, hAssist.name);
	}

	return 0;
}
#define Y_RES	120
#define X_RES	30
#define BORDER	10
#define DRAW_FIX	-10
#define HEADX 6

void CHudDeathNotice::Draw(float flTime)
{
	static int x, y, r, g, b;
	//int m_HUD_d_skull = Hud().GetSpriteIndex("d_skull");
	static int toggle = 1;
	static float flLast = 6.0f;
	static int iFillBg = 1;

	int iMaxNotice = 4;

	switch (g_iMod)
	{
		case MOD_TDM :
		{
			iMaxNotice = 6;
			break;
		}
		case MOD_DM :
		{
			iMaxNotice = 6;
			break;
		}
		case MOD_ZBU :
		{
			iMaxNotice = 8;
			break;
		}
	}


	for ( int i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( m_rgDeathNoticeList[i].iId == 0 )
			break;  // we've gone through them all

		if ( m_rgDeathNoticeList[i].flDisplayTime < flTime )
		{ // display time has expired
			// remove the current item from the list
			memmove( &m_rgDeathNoticeList[i], &m_rgDeathNoticeList[i+1], sizeof(DeathNoticeItem) * (MAX_DEATHNOTICES - i) );
			i--;  // continue on the next item;  stop the counter getting incremented
			continue;
		}
	}


	int iEnd;
	for ( iEnd = 0; iEnd < MAX_DEATHNOTICES; iEnd++ )
	{
		if ( m_rgDeathNoticeList[iEnd].iId == 0 )
			break;
	}

	int iOffset = 0;

	if ( iEnd > iMaxNotice )
		iOffset = iEnd - iMaxNotice;

	for ( int i = iOffset; i < iMaxNotice + iOffset; i++ )
	{
		if ( m_rgDeathNoticeList[i].iId == 0 )
			break;  // we've gone through them all

		/*if ( m_rgDeathNoticeList[i].flDisplayTime < flTime )
		{ // display time has expired
			// remove the current item from the list
			memmove( &m_rgDeathNoticeList[i], &m_rgDeathNoticeList[i+1], sizeof(DeathNoticeItem) * (MAX_DEATHNOTICES - i) );
			i--;  // continue on the next item;  stop the counter getting incremented
			continue;
		}*/
		
		m_rgDeathNoticeList[i].flDisplayTime = min( m_rgDeathNoticeList[i].flDisplayTime, Hud().m_flTime +flLast );

		if(!toggle) return;

			y = (Y_RES + 2 + (22 * (i - iOffset)));  //!!!

			if(g_iMod == MOD_GD)
			{
				y = HudGunDeathBoard().m_iY + 2 + (22 * (i - iOffset));
			}
			int iFillX;

			int id = (m_rgDeathNoticeList[i].iId == -1) ? Hud().GetSpriteIndex("d_skull"): m_rgDeathNoticeList[i].iId;
			x = ScreenWidth - ConsoleStringLen(m_rgDeathNoticeList[i].szVictim) - (Hud().GetSpriteRect(id).right - Hud().GetSpriteRect(id).left)-BORDER;
			x -= 20;
			
			iFillX = x;

			//background color
			int iWidth = ConsoleStringLen(m_rgDeathNoticeList[i].szVictim) + (Hud().GetSpriteRect(id).right - Hud().GetSpriteRect(id).left);


			int m_Head;
			
			if(g_iMod != MOD_ZB4)
				m_Head = Hud().GetSpriteIndex("d_headshot");
			else
				m_Head = Hud().GetSpriteIndex("d_adrenalineshot");

			if(m_rgDeathNoticeList[i].iHeadShot)
			{
				x -= (Hud().GetSpriteRect(m_Head).right - Hud().GetSpriteRect(m_Head).left) ;
				iFillX = x;
				iWidth += (Hud().GetSpriteRect(m_Head).right - Hud().GetSpriteRect(m_Head).left);
			}
			if ( !m_rgDeathNoticeList[i].iSuicide )
			{
				iFillX  -= (5 + ConsoleStringLen( m_rgDeathNoticeList[i].szKiller )+BORDER);
			}

			if(m_rgDeathNoticeList[i].iLocal ==1 && iFillBg)
			{
				int iDrawLen = m_rgDeathNoticeList[i].iSuicide?(iWidth+10+BORDER+5):(5 + ConsoleStringLen( m_rgDeathNoticeList[i].szKiller)+iWidth+10+2*BORDER+5);
				y-=1;
				//3
				GL_DrawTGA2(g_Texture[m_iDeathTga[0][0]].iTexture,iFillX-5,y,3,20,255);
				GL_DrawTGA2(g_Texture[m_iDeathTga[0][1]].iTexture,iFillX-2,y,iDrawLen-6,20,255);
				GL_DrawTGA2(g_Texture[m_iDeathTga[0][2]].iTexture,iFillX-5+iDrawLen-3,y,3,20,255);
			}
			else if(m_rgDeathNoticeList[i].iLocal ==2 && iFillBg)
			{
				int iDrawLen = m_rgDeathNoticeList[i].iSuicide?(iWidth+10+BORDER+5):(5 + ConsoleStringLen( m_rgDeathNoticeList[i].szKiller)+iWidth+10+2*BORDER+5);
				y-=1;
				//3

				GL_DrawTGA2(g_Texture[m_iDeathTga[1][0]].iTexture,iFillX-5,y,3,20,255);
				GL_DrawTGA2(g_Texture[m_iDeathTga[1][1]].iTexture,iFillX-2,y,iDrawLen-6,20,255);
				GL_DrawTGA2(g_Texture[m_iDeathTga[1][2]].iTexture,iFillX-5+iDrawLen-3,y,3,20,255);
			}
			y = (Y_RES + 2 + (22 * (i - iOffset)));
			if(g_iMod == MOD_GD)
			{
				y = HudGunDeathBoard().m_iY + 2 + (22 * (i - iOffset));
			}
			if ( !m_rgDeathNoticeList[i].iSuicide )
			{
				x -= (5 + ConsoleStringLen( m_rgDeathNoticeList[i].szKiller ) )+BORDER;
				// Draw killers name
				if(m_rgDeathNoticeList[i].Killer == 1)
				{
					gEngfuncs.pfnDrawSetTextColor(0.6, 0.8, 1.0);
				}
				else gEngfuncs.pfnDrawSetTextColor(1.0, 0.25, 0.25);
				
				x = 5 + DrawConsoleString( x, y, m_rgDeathNoticeList[i].szKiller )+BORDER;
			}

			if(m_rgDeathNoticeList[i].iHeadShot) x -= HEADX;
			
			r = 255;  g = 80;	b = 0;

			// Draw death weapon
			gEngfuncs.pfnSPR_Set( Hud().GetSprite(id), r, g, b );
			gEngfuncs.pfnSPR_DrawAdditive( 0, x, y, &Hud().GetSpriteRect(id) );
			
			x += (Hud().GetSpriteRect(id).right - Hud().GetSpriteRect(id).left);
			if(m_rgDeathNoticeList[i].iHeadShot) 
			{
				gEngfuncs.pfnSPR_Set( Hud().GetSprite(m_Head), r, g, b );
				gEngfuncs.pfnSPR_DrawAdditive( 0, x + HEADX + 3 , y, &Hud().GetSpriteRect(m_Head));
				x +=Hud().GetSpriteRect(m_Head).right - Hud().GetSpriteRect(m_Head).left;
			}
			if (m_rgDeathNoticeList[i].iNonPlayerKill == FALSE)
			{
				if(m_rgDeathNoticeList[i].Victim == 1)
				{
					gEngfuncs.pfnDrawSetTextColor(0.6, 0.8, 1.0);
				}
				else gEngfuncs.pfnDrawSetTextColor(1.0, 0.25, 0.25);
				x+=BORDER+5;
				if(m_rgDeathNoticeList[i].iHeadShot) x = DrawConsoleString( x + HEADX, y, m_rgDeathNoticeList[i].szVictim );
				else x = DrawConsoleString( x, y, m_rgDeathNoticeList[i].szVictim );
			}
	}
	return;
}