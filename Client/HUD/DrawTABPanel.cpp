#include "metahook.h"
#include "bte_const.h"
#include "exportfuncs.h"
#include "hud.h"
#include "message.h"
#include "DrawTargaImage.h"
#include "BaseUI.h"
#include "Fonts.h"
#include "util.h"
#include "Encode.h"
#include "common.h"

#include "DrawTABPanel.h"

#include "Client/HUD/DrawTGA.h"

static CHudTABBoard g_HudTABBoard;
CHudTABBoard &HudTABBoard()
{
	return g_HudTABBoard;
}

team_info_t g_TeamInfo[MAX_PLAYERS + 1];
hud_player_info_t	 g_PlayerInfoList[MAX_PLAYERS + 1];
int g_PingOffset = 0;

struct vRankInfo
{
	int iFrag;
	int iId;
};

vRankInfo vAfterSort[33];

int iValidPlayer[33];

float fUpdateTime = 0;

wchar_t *g_szLeaderNames[] = { L"csoldjb", L"NoName", L"ltndkl" };

void CHudTABBoard::Init()
{
	m_iFlags |= HUD_ACTIVE;
	m_bCanDraw = false;
	m_type = 2333;
}

void CHudTABBoard::VidInit()
{
	m_iX = 4;
	if ((float)ScreenHeight / (float)ScreenWidth < 0.75)
		m_iX += 100 * (ScreenHeight / 768.0);

	m_iY = 40 * (ScreenHeight / 768.0);

	m_iW = ScreenWidth - 2 * m_iX + 4;
	m_iH = ScreenHeight - 2 * m_iY + 25;

	m_flScale = (float)m_iH / 1024;
	m_flScale2 = m_flScale / 0.6328125;

	m_bLargeFont = (ScreenWidth > 1024 && ScreenHeight > 768);

	m_flNextThink = 0.0f;

	wcscpy(m_szCT, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Team_CT")));
	wcscpy(m_szTR, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Team_T")));
	wcscpy(m_szHM, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Team_Human")));
	wcscpy(m_szZB, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Team_Zombie")));

	wcscpy(m_szBOT, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Attrib_Bot")));
	wcscpy(m_szHost, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Attrib_Host")));
	wcscpy(m_szDead, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Attrib_Dead")));
	wcscpy(m_szBomb, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Attrib_Bomb")));
	wcscpy(m_szHero, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Attrib_Hero")));
	wcscpy(m_szZombie, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Attrib_Zombie")));
	wcscpy(m_szVIP, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Attrib_VIP")));

	wcscpy(m_szName, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Title_Player")));
	wcscpy(m_szKill, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Title_Score")));
	wcscpy(m_szDeath, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Title_Death")));
	wcscpy(m_szPing, UTF8ToUnicode(GetLangUtf("CSBTE_TabPanel_Title_Ping")));


	memset(m_szServerName, 0, sizeof(m_szServerName));
	memset(m_szGameMode, 0, sizeof(m_szGameMode));
	memset(m_szSpec, 0, sizeof(m_szSpec));

	memset(m_bValidPlayer, 0, sizeof(m_bValidPlayer));
	memset(m_iPlayer, 0, sizeof(m_iPlayer));

	m_bHostOwnBuff = false;
	m_iBuffIcon = Hud().m_TGA.FindTexture("resource\\icon\\buff");
}

void CHudTABBoard::Draw(float time)
{
	if (!m_bCanDraw)
		return;

	
	m_flScale2 = m_flScale / 0.6328125;

	if (m_bLargeFont)
		m_flScale2 *= 1.05;

	GL_DrawFullPanel(g_UI_Panel, m_iX, m_iY, m_iW, m_iH, 255);

	// Draw Server Name
	if (!m_bLargeFont)
		g_FontBold.SetWidth(20);
	else
		g_FontBold.SetWidth(22);
	g_FontBold.SetColor(255, 255, 255, 255);
	
	static wchar_t szBuffer[96];
	swprintf(szBuffer, L"%s (%s)", m_szServerName, m_szGameMode);
	
	int iCenterX = ScreenWidth / 2;
	int iTextLen = g_FontBold.GetLen(szBuffer);
	g_FontBold.DrawString(szBuffer, iCenterX - iTextLen / 2, m_iY + 35, 1000);
	
	Think();

	if ((g_iMod == MOD_ZB || g_iMod == MOD_ZB2 || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4 || g_iMod == MOD_ZB5 || g_iMod == MOD_ZSE || g_iMod == MOD_DM))
		DrawScore(false);
	else
		DrawScore(true);
}

void CHudTABBoard::DrawScore(bool bDivideTeam)
{
	int iLocalIndex = gEngfuncs.GetLocalPlayer()->index;

	//g_Font.SetColor(255, 255, 255, 255);
	//g_Font.SetWidth(16);
	//g_Font.DrawString(m_szSpec, m_iX + 20, m_iY + m_iH - 30, 1000);

	bool self = false;

	for (int iColumn = 1; iColumn <= 2; iColumn++)
	{
		int iDraw = 0;

		int x = m_iX + (iColumn - 1) * ((m_iW / 2) - 12);
		int y = m_iY;

		int iTotalScore = 0, iTotalDeath = 0, iTotalPing = 0, iPingDivider = 0, iPlayerCount = 0;

		char szBuf[128];
		wchar_t szText[128];

		for (int i = 1; i <= 32; i++)
		{
			int id = m_iSortedId[i];

			if (!g_PlayerInfoList[id].name)
				continue;

			if (vPlayer[id].team != 1 && vPlayer[id].team != 2)
				continue;

			if (bDivideTeam && vPlayer[id].team != (iColumn == 1 ? 2:1))
				continue;

			if (!bDivideTeam && ((i & 1) != (iColumn - 1)))
				continue;

			if (id == iLocalIndex)
			{
				g_FontBold.SetColor(255, 255, 255, 255);
			}
			else if (vPlayer[id].team == 1)
			{
				g_FontBold.SetColor(173, 201, 235, 255);
			}
			else if (vPlayer[id].team == 2)
			{
				g_FontBold.SetColor(216, 81, 80, 255);
			}
			else
			{
				g_FontBold.SetColor(255, 255, 255, 255);
			}

			iDraw++;

			int offsetY = 120 + 21 * iDraw + 6;
			if (m_bLargeFont)
				offsetY += 2 * iDraw;

			if (!m_bLargeFont)
				g_FontBold.SetWidth(14);
			else
				g_FontBold.SetWidth(16);

			if (m_bHostOwnBuff)
				GL_DrawTGA2(g_Texture[m_iBuffIcon].iTexture, x + 80, y + offsetY - 14, g_Texture[m_iBuffIcon].iWidth, g_Texture[m_iBuffIcon].iHeight, 255);

			g_FontBold.DrawString(UTF8ToUnicode(g_PlayerInfoList[id].name), x + 105, y + offsetY, 1000);

			sprintf(szBuf, "%d", g_PlayerExtraInfo[id].frags);
			g_FontBold.C2W(szText, 128, szBuf);
			g_FontBold.DrawString(szText, x + (m_iW / 2) - 5 - 145 - g_FontBold.GetLen(szText), y + offsetY, 1000);

			sprintf(szBuf, "%d", g_PlayerExtraInfo[id].deaths);
			g_FontBold.C2W(szText, 128, szBuf);
			g_FontBold.DrawString(szText, x + (m_iW / 2) - 5 - 75 - g_FontBold.GetLen(szText), y + offsetY, 1000);

			if (g_PlayerExtraInfo[id].iFlag & SCOREATTRIB_DEAD)
				g_FontBold.DrawString(m_szDead, x + (m_iW / 2) - 5 - 220 - g_FontBold.GetLen(m_szDead), y + offsetY, 1000);
			else if (g_PlayerExtraInfo[id].iFlag & SCOREATTRIB_BOMB && (g_iTeam == 2 || (!g_iTeam || g_iTeam == 3)))
				g_FontBold.DrawString(m_szBomb, x + (m_iW / 2) - 5 - 220 - g_FontBold.GetLen(m_szBomb), y + offsetY, 1000);
			else if (g_PlayerExtraInfo[id].iFlag & SCOREATTRIB_HERO && (g_iTeam == 1 || (!g_iTeam || g_iTeam == 3)))
				g_FontBold.DrawString(m_szHero, x + (m_iW / 2) - 5 - 220 - g_FontBold.GetLen(m_szHero), y + offsetY, 1000);
			else if (g_PlayerExtraInfo[id].iFlag & SCOREATTRIB_VIP && (g_iTeam == 1 || (!g_iTeam || g_iTeam == 3)))
				g_FontBold.DrawString(m_szVIP, x + (m_iW / 2) - 5 - 220 - g_FontBold.GetLen(m_szVIP), y + offsetY, 1000);
			else if (IS_ZOMBIE_MODE && vPlayer[id].team == 2)
				g_FontBold.DrawString(m_szZombie, x + (m_iW / 2) - 5 - 220 - g_FontBold.GetLen(m_szZombie), y + offsetY, 1000);

			if (g_PlayerInfoList[id].ping == 0)
			{
				const char *isBotString = gEngfuncs.PlayerInfo_ValueForKey(id, "*bot");
				if (isBotString && atoi(isBotString) > 0)
					g_FontBold.DrawString(m_szBOT, x + (m_iW / 2) - 5 - g_FontBold.GetLen(m_szBOT), y + offsetY, 1000);
				else
					g_FontBold.DrawString(m_szHost, x + (m_iW / 2) - 5 - g_FontBold.GetLen(m_szHost), y + offsetY, 1000);
			}
			else
			{
				char ping[8];
				wchar_t pping[8];
				sprintf(ping, "%d", g_PlayerInfoList[id].ping);
				g_FontBold.C2W(pping, 255, ping);
				
				g_FontBold.DrawString(pping, x + (m_iW / 2) - 5 - g_FontBold.GetLen(pping), y + offsetY, 1000);
			}

			iTotalScore += g_PlayerExtraInfo[id].frags;
			iTotalDeath += g_PlayerExtraInfo[id].deaths;
			iTotalPing += g_PlayerInfoList[id].ping;
			if (g_PlayerInfoList[id].ping)
				iPingDivider++;
			iPlayerCount++;
		}

		if (!m_bLargeFont)
			g_FontBold.SetWidth(14);
		else
			g_FontBold.SetWidth(16);

		g_FontBold.SetColor(255, 188, 0, 255);

		g_FontBold.DrawString(m_szKill, x + (m_iW / 2) - 5 - 145 - g_FontBold.GetLen(m_szKill), y + 80 + 12, 1000);
		g_FontBold.DrawString(m_szDeath, x + (m_iW / 2) - 5 - 75 - g_FontBold.GetLen(m_szDeath), y + 80 + 12, 1000);
		g_FontBold.DrawString(m_szPing, x + (m_iW / 2) - 5 - 5 - g_FontBold.GetLen(m_szPing), y + 80 + 12, 1000);

		gEngfuncs.pfnFillRGBA(x + 9, y + 96, (m_iW / 2) - 16, 1, 172, 104, 0, 255);

		if (bDivideTeam && iPlayerCount)
		{
			if (iColumn == 2)
				swprintf(szText, L"%s  (%d)", IS_ZOMBIE_MODE ? m_szHM : m_szCT, iPlayerCount);
			else
				swprintf(szText, L"%s  (%d)", IS_ZOMBIE_MODE ? m_szZB : m_szTR, iPlayerCount);
			g_FontBold.DrawString(szText, x + 105, y + 110 + 12, 1000);

			gEngfuncs.pfnFillRGBA(x + 10, y + 125, (m_iW / 2) - 16, 1, 188, 112, 0, 255);

			swprintf(szText, L"%d", iTotalScore / iPlayerCount);
			g_FontBold.DrawString(szText, x + (m_iW / 2) - 5 - 145 - g_FontBold.GetLen(szText), y + 110 + 12, 1000);

			swprintf(szText, L"%d", iTotalDeath / iPlayerCount);
			g_FontBold.DrawString(szText, x + (m_iW / 2) - 5 - 75 - g_FontBold.GetLen(szText), y + 110 + 12, 1000);

			if (iPingDivider)
			{ 
				swprintf(szText, L"%d", iTotalPing / iPingDivider);
				g_FontBold.DrawString(szText, x + (m_iW / 2) - 5 - g_FontBold.GetLen(szText), y + 110 + 12, 1000);
			}

		}
		else if (!bDivideTeam && iPlayerCount)
		{
			if (iColumn == 2)
				swprintf(szText, L"%s  (%d)", IS_ZOMBIE_MODE ? m_szHM : m_szCT, m_iPlayer[iColumn == 1 ? 1 : 0][0]);
			else
				swprintf(szText, L"%s  (%d)", IS_ZOMBIE_MODE ? m_szZB : m_szTR, m_iPlayer[iColumn == 1 ? 1 : 0][0]);
			g_FontBold.DrawString(szText, x + 105, y + 110 + 12, 1000);
			
			
			swprintf(szText, L"%d (%s)", m_iTeamScore[iColumn == 1 ? 1:0], GetLangUni("CSO_Round"));
			g_FontBold.DrawString(szText, x + (m_iW / 2) - 7 - g_FontBold.GetLen(szText), y + 110 + 12, 1000);

			gEngfuncs.pfnFillRGBA(x + 10, y + 125, (m_iW / 2) - 15, 1, 188, 112, 0, 255);
		}


		if (!m_bLargeFont)
			g_FontBold.SetWidth(14);
		else
			g_FontBold.SetWidth(16);

		if (iColumn == 1)
		{
			// 画司令部装逼名单
			int iRankGameMode = 0;
			if (IS_ZOMBIE_MODE)
				iRankGameMode = 2;
			else if (g_iMod == MOD_TDM || g_iMod == MOD_DM || g_iMod == MOD_GD)
				iRankGameMode = 4;
			else if (g_iMod == MOD_NONE)
				iRankGameMode = 1;
			else if (g_iMod == MOD_HMS)
				iRankGameMode = 3;

			if (iRankGameMode)
			{
				g_FontBold.SetColor(255, 188, 0, 255);
				swprintf(szText, GetLangUni("CSO_Rank_ScoreBoard_Title"), GetLangUni(va("CSO_ModeTitle_%d", iRankGameMode)));
				g_FontBold.DrawString(szText, x + 29, y + m_iH - 229 + 12, 1000);
				gEngfuncs.pfnFillRGBA(x + 19, y + m_iH - 212, (m_iW / 2) - 49, 1, 188, 112, 0, 255);
				g_FontBold.SetColor(188, 112, 0, 255);
				for (int i = 0; i < 3; i++)
				{
					swprintf(szText, L"%d. %s", i + 1, g_szLeaderNames[i]);
					g_FontBold.DrawString(szText, x + 34, y + m_iH - 194 + 25 * i, 1000);
				}
			}
		}
		else
		{
			iPlayerCount = 0;
			for (int id = 1; id <= 32; id++)
			{
				if (!g_PlayerInfoList[id].name)
					continue;
				if (vPlayer[id].team == 1 || vPlayer[id].team == 2)
					continue;

				iPlayerCount++;
				g_FontBold.SetColor(255, 255, 255, 255);

				int offsetY = m_iH - 213 + 21 * iPlayerCount;
				g_FontBold.DrawString(UTF8ToUnicode(g_PlayerInfoList[id].name), x + 35, y + offsetY, 1000);
			}
			if (iPlayerCount)
			{
				g_FontBold.SetColor(255, 188, 0, 255);
				g_FontBold.DrawString(GetLangUni("Cstrike_TitlesTXT_Spectators"), x + 19, y + m_iH - 228 + 12, 1000);
				gEngfuncs.pfnFillRGBA(x + 19, y + m_iH - 212, (m_iW / 2) - 49, 1, 188, 112, 0, 255);
			}
		}
	}

	
}

void CHudTABBoard::ResetLevel()
{

	char wpnlimit[32], mode[32];

	switch (g_iWeaponLimit)
	{
	case 0:
	{
		strcpy(wpnlimit, GetLangUtf("CSBTE_TabPanel_NoLimit"));
		break;
	}
	case 1:
	{
		strcpy(wpnlimit, GetLangUtf("CSBTE_TabPanel_SniperOnly"));
		break;
	}
	case 2:
	{
		strcpy(wpnlimit, GetLangUtf("CSBTE_TabPanel_PistolOnly"));
		break;
	}
	case 3:
	{
		strcpy(wpnlimit, GetLangUtf("CSBTE_TabPanel_GrenadeOnly"));
		break;
	}
	case 4:
	{
		strcpy(wpnlimit, GetLangUtf("CSBTE_TabPanel_KnifelOnly"));
		break;
	}
	}
	/*
	strcpy(levelname, gEngfuncs.pfnGetLevelName());
	levelname[strlen(levelname) - 4] = 0;

	char find_map[64];
	sprintf(find_map, "MapName_%s", levelname + 5);
	memset(levelname, 0, sizeof(levelname));
	strcpy(levelname, GetLangUtf(find_map));
	*/
	char find[16];
	sprintf(find, "CSBTE_MOD_%d", g_iMod);
	strcpy(mode, GetLangUtf(find));

	wcscpy(m_szGameMode, UTF8ToUnicode(va("%s / %s", mode, wpnlimit)));
}

void CHudTABBoard::ResetServerName(char *pszName)
{
	wcscpy(m_szServerName, UTF8ToUnicode(pszName));
}

void CHudTABBoard::ResetSpec()
{
	char spec[1024];
	memset(spec, 0, sizeof(spec));

	m_iSpec = 0;

	for (int i = 1; i <= 32; i++)
	{
		if (!m_bValidPlayer[i])
			continue;

		if (/*vPlayer[i].team == 0 || */vPlayer[i].team == 3)
		{
			strcat(spec, ", ");
			strcat(spec, g_PlayerInfoList[i].name);

			m_iSpec++;
		}
	}

	if (!m_iSpec)
	{
		m_szSpec[0] = 0;
		return;
	}

	wchar_t *pBuf;
	pBuf = vgui::localize()->Find("CSBTE_TabPanel_Spec");

	if (!pBuf)
		return;

	/*wchar_t szBuf[256];
	g_Font.C2W(szBuf, 256, spec + 2);*/

	vgui::localize()->ConstructString(m_szSpec, 256, pBuf, 1, UTF8ToUnicode(spec + 2));
}

void CHudTABBoard::ResetAlivePlayer()
{
	wchar_t *pBuf;
	pBuf = vgui::localize()->Find("CSBTE_TabPanel_Alive_PLayer");

	for (int i = 0; i <= 1; i++)
	{
		char player[2][4];
		sprintf(player[1], "%d", m_iPlayer[i][0]);
		sprintf(player[0], "%d", m_iPlayer[i][1]);

		wchar_t szBuf[2][4];
		g_Font.C2W(szBuf[0], 4, player[0]);
		g_Font.C2W(szBuf[1], 4, player[1]);

		vgui::localize()->ConstructString(m_szAlivePlayer[i], 256, pBuf, 2, szBuf[0], szBuf[1]);
	}
}

void CHudTABBoard::Think()
{
	if (m_flNextThink > Hud().m_flTime)
		return;

	m_flNextThink = Hud().m_flTime + 0.5f;

	int iSpec = 0;
	int iPlayer[2] = { 0, 0 }, iPlayerAlive[2] = { 0, 0 };

	for (int i = 1; i <= 32; i++)
	{
		gEngfuncs.pfnGetPlayerInfo(i, &g_PlayerInfoList[i]);

		if (g_PlayerInfoList[i].name)
			m_bValidPlayer[i] = true;
		else
			m_bValidPlayer[i] = false;

		/*if (vPlayer[i].team == 3 && m_bValidPlayer[i])
		iSpec ++;*/

		if (vPlayer[i].team == 1 || vPlayer[i].team == 2)
		{
			iPlayer[vPlayer[i].team - 1] ++;
			iPlayerAlive[vPlayer[i].team - 1] ++;

			if (g_PlayerExtraInfo[i].iFlag & SCOREATTRIB_DEAD)
				iPlayerAlive[vPlayer[i].team - 1] --;
		}

		m_iSortedId[i] = i;
	}

	if (iPlayer[0] != m_iPlayer[0][0] || iPlayerAlive[0] != m_iPlayer[0][1] || iPlayer[1] != m_iPlayer[1][0] || iPlayerAlive[1] != m_iPlayer[1][1])
	{
		m_iPlayer[0][0] = iPlayer[0];
		m_iPlayer[0][1] = iPlayerAlive[0];
		m_iPlayer[1][0] = iPlayer[1];
		m_iPlayer[1][1] = iPlayerAlive[1];

		ResetAlivePlayer();
	}

	for (int i = 1; i <= 32; i++)
	{
		if (!m_bValidPlayer[m_iSortedId[i]])
			continue;

		//for (int j = 1; j <= 32; j++)
		for (int j = 1; j <= 32 - i - 1; j++)
		{
			if (!m_bValidPlayer[m_iSortedId[j]])
				continue;

			if (g_PlayerExtraInfo[m_iSortedId[j]].frags <= g_PlayerExtraInfo[m_iSortedId[j+1]].frags)
			{
				if (g_PlayerExtraInfo[m_iSortedId[j]].frags == g_PlayerExtraInfo[m_iSortedId[j + 1]].frags)
				{
					if (g_PlayerExtraInfo[m_iSortedId[j]].deaths > g_PlayerExtraInfo[m_iSortedId[j + 1]].deaths)
					{
						int temp = m_iSortedId[j + 1];
						m_iSortedId[j + 1] = m_iSortedId[j];
						m_iSortedId[j] = temp;
					}
				}
				else
				{
					int temp = m_iSortedId[j + 1];
					m_iSortedId[j + 1] = m_iSortedId[j];
					m_iSortedId[j] = temp;
				}
			}
		}
	}

	//if (iSpec != m_iSpec)
	//	Hud().m_TABBoard.ResetSpec();

	ResetSpec();
}
