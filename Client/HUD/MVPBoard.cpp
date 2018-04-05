#include "metahook.h"
#include "bte_const.h"
#include "exportfuncs.h"
#include "hud.h"
#include "parsemsg.h"
#include "DrawTargaImage.h"
#include "Fonts.h"
#include "util.h"
#include "common.h"
#include "Encode.h"

#include "MVPBoard.h"
#include "DrawTABPanel.h"
#include "Client/HUD/DrawTGA.h"

static CHudMVPBoard g_HudMVPBoard;
CHudMVPBoard &HudMVPBoard()
{
	return g_HudMVPBoard;
}

/*
*	Slow code, DO NOT COPY
*	Or you will get slower XD
*/

void CHudMVPBoard::Init(void)
{
	gEngfuncs.pfnHookUserMsg("MVPBoard", [](const char *pszName, int iSize, void *pbuf){ return HudMVPBoard().MsgFunc_MVPBoard(pszName, iSize, pbuf); });
	gEngfuncs.pfnHookUserMsg("MVP", [](const char *pszName, int iSize, void *pbuf){ return HudMVPBoard().MsgFunc_MVP(pszName, iSize, pbuf); });

	m_iFlags |= HUD_ACTIVE;

	m_hBoard_Blue = Hud().m_TGA.FindTexture("resource\\pvp2nd\\pvp2_combatpopup_top_blue");
	m_hBoard_Red = Hud().m_TGA.FindTexture("resource\\pvp2nd\\pvp2_combatpopup_top_red");
	m_hBoard_BG = Hud().m_TGA.FindTexture("resource\\pvp2nd\\pvp2_combatpopup_board");
}

void CHudMVPBoard::VidInit(void)
{
	SetVisible(false);

	memset(m_iTopFive, 0, sizeof(m_iTopFive));
	wcscpy(m_lpMVPText, UTF8ToUnicode(""));
	wcscpy(m_lpTime, UTF8ToUnicode(""));
	wcscpy(m_lpWinText, UTF8ToUnicode(""));

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			wcscpy(m_lpScore[i][j], UTF8ToUnicode(""));
			wcscpy(m_lpTopFive[i][j], UTF8ToUnicode(""));
		}
	}

	m_pos.x = ScreenWidth / 2 - g_Texture[m_hBoard_Blue].iWidth / 2 - 4;
	m_pos.y = g_Texture[m_hBoard_Blue].iHeight / 4 * 3;

}

int CHudMVPBoard::CheckWinTeam(int iTeam, int iPlayer)
{
	char pTeam[256];
	char pVictory[256];
	strcpy(pVictory, GetLangUtf("#CSO_Victory"));
	
	if (IS_ZOMBIE_MODE)
	{
		switch (iTeam)
		{
		case 0:
		{
			SetVisible(false);
			return 0;
		}
		case 2:
		{
			wcscpy(m_lpWinText, GetLangUni("#CSO_HumanWin"));
			m_hBoard = m_hBoard_Blue;
			break;
		}
		default:
		{
			 wcscpy(m_lpWinText, GetLangUni("#CSO_ZombiWin"));
			 m_hBoard = m_hBoard_Red;
			 break;
		}
		}
	}
	else if (g_iMod == MOD_DM)
	{
		switch (iTeam)
		{
		case 0:
		{
			SetVisible(false);
			return 0;
		}
		case 2:
		{
			m_hBoard = m_hBoard_Blue;
			break;
		}
		default:
		{
			 m_hBoard = m_hBoard_Red;
			 break;
		}
		}
	}
	else
	{
		switch (iTeam)
		{
		case 0:
		{
			SetVisible(false);
			return 0;
		}
		case 2:
		{
			strcpy(pTeam, GetLangUtf("#CSO_ClsCT"));
			m_hBoard = m_hBoard_Blue;
			break;
		}
		default:
		{
			strcpy(pTeam, GetLangUtf("#CSO_ClsTER"));
			m_hBoard = m_hBoard_Red;
			break;
		}
		}

		wcscpy(m_lpWinText, UTF8ToUnicode(va("%s %s", pTeam, pVictory)));
	}
	if (iTeam == 1)
	{
		m_iColor[0][0] = 232;
		m_iColor[0][1] = 150;
		m_iColor[0][2] = 138;
		m_iColor[1][0] = 163;
		m_iColor[1][1] = 89;
		m_iColor[1][2] = 80;
	}
	else
	{
		m_iColor[0][0] = 131;
		m_iColor[0][1] = 203;
		m_iColor[0][2] = 225;
		m_iColor[1][0] = 55;
		m_iColor[1][1] = 151;
		m_iColor[1][2] = 201;
	}
	return 1;
}

// what a fuck function name ?

wchar_t *CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(wchar_t *lpText)
{
	if (g_Font.GetLen(lpText) <= 262) return lpText;
	wchar_t *lpEdited = lpText;
	for (unsigned int iLen = wcslen(lpText) - 1; iLen > 0; iLen--)
	{
		lpEdited[iLen] = NULL;
		if (g_Font.GetLen(lpEdited) < 259)
		{
			wcscat(lpEdited, L"...");
			break;
		}
	}
	return lpEdited;
}

void CHudMVPBoard::Draw(float time)
{
	if (IS_ZOMBIE_MODE)
	{
		DrawZB();
	}
	else
	{
		DrawNormal();
	}
}

void CHudMVPBoard::DrawNormal(void)
{
	if (!GetVisible()) return;
	int x = m_pos.x + 15, y = m_pos.y + 51;
	int iFillX = x + 2, iFillY;
	int right = m_pos.x + g_Texture[m_hBoard].iWidth - 15;
	GL_DrawFullPanel(g_UI_Panel_Dark, m_pos.x - 4, m_pos.y, g_Texture[m_hBoard].iWidth + 14, int(g_Texture[m_hBoard].iHeight * 3.2), 225);
	GL_DrawTGA(g_Texture[m_hBoard].iTexture, 255, 255, 255, 255, m_pos.x, m_pos.y, 1.0);
	g_Font.SetWidth(25);
	g_Font.SetColor(m_iColor[0][0], m_iColor[0][1], m_iColor[0][2], 255);
	g_Font.DrawString(m_lpWinText, x, y, 1000);
	y += 14 + 18;
	g_Font.SetWidth(16);
	g_Font.SetColor(m_iColor[1][0], m_iColor[1][1], m_iColor[1][2], 255);
	g_Font.DrawString(m_lpMVPText, x, y, 1000);
	g_Font.DrawString(m_lpTime, right - g_Font.GetLen(m_lpTime), y, 1000);
	x += 15;
	y += 20 + 33;
	g_Font.SetColor(255, 255, 255, 255);
	g_Font.SetWidth(17);
	g_Font.DrawString(GetLangUni("#CSO_KillCount"), x, y, 1000);
	iFillY = y + 16;
	// Draw the background twice to make it brighter ._.||
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	int iPlayerX = x, iPlayerY = y + 37;
	int iLocal = gEngfuncs.GetLocalPlayer()->index;
	for (int i = 0; i < 5; i++)
	{
		if (HudTABBoard().m_bValidPlayer[m_iTopFive[0][i]])
		{
			if (m_iTopFive[0][i] == iLocal)
				gEngfuncs.pfnFillRGBA(iPlayerX - 4, iPlayerY - 14, g_Texture[m_hBoard_BG].iWidth - 17, 17, 50, 50, 50, 255);
			g_Font.SetWidth(14);
			if (vPlayer[m_iTopFive[0][i]].team == 1)
				g_Font.SetColor(173, 201, 235, 255);
			else if (vPlayer[m_iTopFive[0][i]].team == 2)
				g_Font.SetColor(216, 81, 80, 255);
			g_Font.DrawString(m_lpTopFive[0][i], iPlayerX, iPlayerY, 1000);
			g_Font.DrawString(m_lpScore[0][i], iPlayerX - 3 + g_Texture[m_hBoard_BG].iWidth - 32 - g_Font.GetLen(m_lpScore[0][i]), iPlayerY, 1000);
			iPlayerY += 21;
		}
	}
	y += g_Texture[m_hBoard_BG].iHeight + 38;
	iFillY += g_Texture[m_hBoard_BG].iHeight + 32;
	g_Font.SetColor(255, 255, 255, 255);
	g_Font.SetWidth(17);
	g_Font.DrawString(GetLangUni("#CSO_Sc_Assist"), x, y, 1000);
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	iPlayerY = y + 31;
	for (int i = 0; i < 5; i++)
	{
		if (HudTABBoard().m_bValidPlayer[m_iTopFive[1][i]])
		{
			if (m_iTopFive[1][i] == iLocal)
				gEngfuncs.pfnFillRGBA(iPlayerX - 4, iPlayerY - 14, g_Texture[m_hBoard_BG].iWidth - 17, 17, 55, 55, 55, 255);
			g_Font.SetWidth(14);
			if (vPlayer[m_iTopFive[1][i]].team == 1)
				g_Font.SetColor(173, 201, 235, 255);
			else if (vPlayer[m_iTopFive[1][i]].team == 2)
				g_Font.SetColor(216, 81, 80, 255);
			g_Font.DrawString(m_lpTopFive[1][i], iPlayerX, iPlayerY, 1000);
			g_Font.DrawString(m_lpScore[1][i], iPlayerX - 3 + g_Texture[m_hBoard_BG].iWidth - 32 - g_Font.GetLen(m_lpScore[1][i]), iPlayerY, 1000);
			iPlayerY += 21;
		}
	}
}

void CHudMVPBoard::DrawZB(void)
{
	if (!GetVisible()) return;
	int x = m_pos.x + 15, y = m_pos.y + 51;
	int iFillX = x + 2, iFillY;
	int right = m_pos.x + g_Texture[m_hBoard].iWidth - 15;
	GL_DrawFullPanel(g_UI_Panel_Dark, m_pos.x - 4, m_pos.y, g_Texture[m_hBoard].iWidth + 14, int(g_Texture[m_hBoard].iHeight * 4.3), 225);
	GL_DrawTGA(g_Texture[m_hBoard].iTexture, 255, 255, 255, 255, m_pos.x, m_pos.y, 1.0);
	g_Font.SetWidth(25);
	g_Font.SetColor(m_iColor[0][0], m_iColor[0][1], m_iColor[0][2], 255);
	g_Font.DrawString(m_lpWinText, x, y, 1000);
	y += 14 + 18;
	g_Font.SetWidth(16);
	g_Font.SetColor(m_iColor[1][0], m_iColor[1][1], m_iColor[1][2], 255);
	g_Font.DrawString(m_lpMVPText, x, y, 1000);
	g_Font.DrawString(m_lpTime, right - g_Font.GetLen(m_lpTime), y, 1000);
	x += 15;
	y += 20 + 33;
	g_Font.SetColor(255, 255, 255, 255);
	g_Font.SetWidth(17);
	g_Font.DrawString(GetLangUni("#CSO_ZombieKill"), x, y, 1000);
	iFillY = y + 16;
	// Draw the background twice to make it brighter ._.||
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	int iPlayerX = x, iPlayerY = y + 38;
	int iLocal = gEngfuncs.GetLocalPlayer()->index;
	for (int i = 0; i < 5; i++)
	{
		if (HudTABBoard().m_bValidPlayer[m_iTopFive[0][i]])
		{
			if (m_iTopFive[0][i] == iLocal)
			{
				gEngfuncs.pfnFillRGBA(iPlayerX - 4, iPlayerY - 14, g_Texture[m_hBoard_BG].iWidth - 17, 17, 55, 55, 55, 255);
			}
			g_Font.SetWidth(14);
			if (vPlayer[m_iTopFive[0][i]].team == 1)
				g_Font.SetColor(173, 201, 235, 255);
			else if (vPlayer[m_iTopFive[0][i]].team == 2)
				g_Font.SetColor(216, 81, 80, 255);
			g_Font.DrawString(m_lpTopFive[0][i], iPlayerX, iPlayerY, 1000);
			g_Font.DrawString(m_lpScore[0][i], iPlayerX - 3 + g_Texture[m_hBoard_BG].iWidth - 32 - g_Font.GetLen(m_lpScore[0][i]), iPlayerY, 1000);
			iPlayerY += 21;
		}
	}
	y += g_Texture[m_hBoard_BG].iHeight + 38;
	iFillY += g_Texture[m_hBoard_BG].iHeight + 32;
	g_Font.SetColor(255, 255, 255, 255);
	g_Font.SetWidth(17);
	g_Font.DrawString(GetLangUni("#CSO_Sc_Assist"), x, y, 1000);
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	iPlayerY = y + 31;
	for (int i = 0; i < 5; i++)
	{
		if (HudTABBoard().m_bValidPlayer[m_iTopFive[1][i]])
		{
			if (m_iTopFive[1][i] == iLocal)
				gEngfuncs.pfnFillRGBA(iPlayerX - 4, iPlayerY - 14, g_Texture[m_hBoard_BG].iWidth - 17, 17, 55, 55, 55, 255);
			g_Font.SetWidth(14);
			if (vPlayer[m_iTopFive[1][i]].team == 1)
				g_Font.SetColor(173, 201, 235, 255);
			else if (vPlayer[m_iTopFive[1][i]].team == 2)
				g_Font.SetColor(216, 81, 80, 255);
			g_Font.DrawString(m_lpTopFive[1][i], iPlayerX, iPlayerY, 1000);
			g_Font.DrawString(m_lpScore[1][i], iPlayerX - 3 + g_Texture[m_hBoard_BG].iWidth - 32 - g_Font.GetLen(m_lpScore[1][i]), iPlayerY, 1000);
			iPlayerY += 21;
		}
	}
	y += g_Texture[m_hBoard_BG].iHeight + 34;
	iFillY += g_Texture[m_hBoard_BG].iHeight + 32;
	g_Font.SetColor(255, 255, 255, 255);
	g_Font.SetWidth(17);
	g_Font.DrawString(GetLangUni("#CSO_Rank_Zombie_Info"), x, y, 1000);
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	GL_DrawTGA(g_Texture[m_hBoard_BG].iTexture, 255, 255, 255, 150, iFillX, iFillY, 1.0);
	iPlayerY = y + 30;
	for (int i = 0; i < 5; i++)
	{
		if (HudTABBoard().m_bValidPlayer[m_iTopFive[2][i]])
		{
			if (m_iTopFive[2][i] == iLocal)
				gEngfuncs.pfnFillRGBA(iPlayerX - 4, iPlayerY - 14, g_Texture[m_hBoard_BG].iWidth - 17, 17, 55, 55, 55, 255);
			g_Font.SetWidth(14);
			if (vPlayer[m_iTopFive[2][i]].team == 1)
				g_Font.SetColor(173, 201, 235, 255);
			else if (vPlayer[m_iTopFive[2][i]].team == 2)
				g_Font.SetColor(216, 81, 80, 255);
			g_Font.DrawString(m_lpTopFive[2][i], iPlayerX, iPlayerY, 1000);
			g_Font.DrawString(m_lpScore[2][i], iPlayerX - 3 + g_Texture[m_hBoard_BG].iWidth - 32 - g_Font.GetLen(m_lpScore[2][i]), iPlayerY, 1000);
			iPlayerY += 21;
		}
	}
}

int CHudMVPBoard::MsgFunc_MVPBoard(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iWinTeam = READ_SHORT();
	int iType = READ_SHORT();
	int iPlayer;
	hud_player_info_t hPlayerInfo;
	if (iType == 3)
	{
		iPlayer = READ_SHORT();	// get the player who planted the bomb
		gEngfuncs.pfnGetPlayerInfo(iPlayer, &hPlayerInfo);
		char pChar[1024];
		sprintf_s(pChar, GetLangUtf("#CSO_mvp_c4plant"), hPlayerInfo.name);
		wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(pChar)));
	}
	else if (iType == 4)
	{
		iPlayer = READ_SHORT();	// get the player who defused the bomb
		gEngfuncs.pfnGetPlayerInfo(iPlayer, &hPlayerInfo);
		char pChar[1024];
		sprintf_s(pChar, GetLangUtf("#CSO_mvp_c4defuse"), hPlayerInfo.name);
		wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(pChar)));
	}
	else if (iType == 5)
	{
		iPlayer = READ_SHORT();	// get the player who finish 30 kills the first in DeathMatch mode
		gEngfuncs.pfnGetPlayerInfo(iPlayer, &hPlayerInfo);

		char szWinText[512];
		sprintf_s(szWinText, va("%s %s", hPlayerInfo.name, GetLangUtf("#CSO_Victory")));
		wcscpy(m_lpWinText, UTF8ToUnicode(szWinText));

		if (m_iScore[0][0] >= m_iScore[1][0] && m_iScore[0][0] >= m_iScore[2][0])
		{
			char pChar[1024];
			gEngfuncs.pfnGetPlayerInfo(m_iTopFive[0][0], &hPlayerInfo);
			sprintf_s(pChar, GetLangUtf("#CSO_mvp_killace"), hPlayerInfo.name);
			wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(pChar)));
		}
		else if (m_iScore[1][0] > m_iScore[0][0] && m_iScore[1][0] >= m_iScore[2][0])
		{
			char pChar[1024];
			gEngfuncs.pfnGetPlayerInfo(m_iTopFive[1][0], &hPlayerInfo);
			sprintf_s(pChar, GetLangUtf("#CSO_mvp_assistace"), hPlayerInfo.name);
			wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(pChar)));
		}
		if (!m_iScore[0][0] && !m_iScore[1][0] && !m_iScore[2][0])
			wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(" ")));
	}
	else
	{
		if (IS_ZOMBIE_MODE)
		{
			if (m_iScore[0][0] >= m_iScore[1][0] && m_iScore[0][0] >= m_iScore[2][0])
			{
				char pChar[1024];
				gEngfuncs.pfnGetPlayerInfo(m_iTopFive[0][0], &hPlayerInfo);
				sprintf_s(pChar, GetLangUtf("#CSO_mvp_killace"), hPlayerInfo.name);
				wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(pChar)));
			}
			else if (m_iScore[1][0] > m_iScore[0][0] && m_iScore[1][0] >= m_iScore[2][0])
			{
				char pChar[1024];
				gEngfuncs.pfnGetPlayerInfo(m_iTopFive[1][0], &hPlayerInfo);
				sprintf_s(pChar, GetLangUtf("#CSO_mvp_assistace"), hPlayerInfo.name);
				wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(pChar)));
			}
			else if (m_iScore[2][0] > m_iScore[0][0] && m_iScore[2][0] > m_iScore[1][0])
			{
				char pChar[1024];
				gEngfuncs.pfnGetPlayerInfo(m_iTopFive[2][0], &hPlayerInfo);
				sprintf_s(pChar, GetLangUtf("#CSO_mvp_zombieace"), hPlayerInfo.name);
				wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(pChar)));
			}
			if (!m_iScore[0][0] && !m_iScore[1][0] && !m_iScore[2][0])
				wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(" ")));
		}
		else
		{
			if (m_iScore[0][0] >= m_iScore[1][0] && m_iScore[0][0] >= m_iScore[2][0])
			{
				char pChar[1024];
				gEngfuncs.pfnGetPlayerInfo(m_iTopFive[0][0], &hPlayerInfo);
				sprintf_s(pChar, GetLangUtf("#CSO_mvp_killace"), hPlayerInfo.name);
				wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(pChar)));
			}
			else if (m_iScore[1][0] > m_iScore[0][0] && m_iScore[1][0] >= m_iScore[2][0])
			{
				char pChar[1024];
				gEngfuncs.pfnGetPlayerInfo(m_iTopFive[1][0], &hPlayerInfo);
				sprintf_s(pChar, GetLangUtf("#CSO_mvp_assistace"), hPlayerInfo.name);
				wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(pChar)));
			}
			if (!m_iScore[0][0] && !m_iScore[1][0] && !m_iScore[2][0])
				wcscpy(m_lpMVPText, CutAndAddSuspensionPointsAtTheEndOfTextIfHaveTo(UTF8ToUnicode(" ")));
		}
	}
	int iTime = READ_BYTE();
	char p[2][33];
	sprintf_s(p[0], "%d", iTime / 60);
	if (iTime % 60 < 10)
		sprintf_s(p[1], "0%d", iTime % 60);
	else
		sprintf_s(p[1], "%d", iTime % 60);
	char pChar[1024];
	sprintf_s(pChar, "%s%s:%s", GetLangUtf("#CSO_ZBS_RoundTime"), p[0], p[1]);
	wcscpy(m_lpTime, UTF8ToUnicode(pChar));
	if (CheckWinTeam(iWinTeam, iPlayer))
		SetVisible(true);
	return 0;
}

void CHudMVPBoard::OnPlayerSpawn(void)
{
	SetVisible(false);
	memset(m_iTopFive, 0, sizeof(m_iTopFive));
	wcscpy(m_lpMVPText, L"");
	wcscpy(m_lpTime, L"");
	wcscpy(m_lpWinText, L"");
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			wcscpy(m_lpScore[i][j], L"");
			wcscpy(m_lpTopFive[i][j], L"");
		}
	}
	return;
}

int CHudMVPBoard::MsgFunc_MVP(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iType = READ_SHORT() - 1;
	hud_player_info_t info;

	char lags[3][512];
	strcpy(lags[0], GetLangUtf("#CSO_Kill"));
	strcpy(lags[1], GetLangUtf("#CSO_Sc_Assist"));
	strcpy(lags[2], GetLangUtf("#CSO_Rank_Zombie_Info"));

	for (int i = 0; i < 5; i++)
	{
		char pChar[512];
		int iData = READ_LONG();
		int iPlayer = iData % 100;
		if (iPlayer > 33 || !iPlayer) continue;
		gEngfuncs.pfnGetPlayerInfo(iPlayer, &info);
		strcpy(pChar, info.name);
		wcscpy(m_lpTopFive[iType][i], UTF8ToUnicode(pChar));
		m_iTopFive[iType][i] = iPlayer;
		m_iScore[iType][i] = iData / 100;
		sprintf_s(pChar, "%d%s", iData / 100, lags[iType]);
		wcscpy(m_lpScore[iType][i], UTF8ToUnicode(pChar));
	}

	return 0;
}