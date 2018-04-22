#include "metahook.h"
#include "bte_const.h"
#include "configs.h"
#include "exportfuncs.h"
#include "message.h"
#include "msghook.h"
#include "hud.h"
#include "BaseUI.h"
#include "texdraw.h"
#include "gl/gl.h"
#include "TriAPI.h"
#include "Fonts.h"
#include "util.h"
#include "Encode.h"

#include "zb4ui.h"
#include "scoreboard.h"
#include "Client/HUD/DrawTGA.h"

static CHudZB4UI g_HudZombieMod4;
CHudZB4UI &HudZombieMod4()
{
	return g_HudZombieMod4;
}

#define	Hide	0
#define	Normal	1
#define	Flash	2
#define	Flash2	3

#define DISABLE_LIGHT_UI

#define X_Start 10

void CHudZB4UI::Init()
{
	m_iFlags |= HUD_ACTIVE;
	
	m_ScoreBoard.Init();
	m_Radar.Init();
	m_Power.Init();
	m_Icon.Init();
	m_Tip.Init();
}

void CHudZB4UI::VidInit()
{
	m_Tip.VidInit();
	m_Bink.Init("cstrike//resource//hud//zombie//infection2.bik");
	m_Bink.VidInit();

	m_Retina[0].VidInit("cstrike//resource//hud//zombie//zombispeedup.tga", true, true);
	m_Retina[1].VidInit("cstrike//resource//hud//zombie//zombicrazy.tga", false, true);

	m_Retina[2].VidInit("cstrike//resource//hud//zombie//zombinight.tga", true, false);

	m_Retina[3].VidInit("cstrike//resource//hud//zombie//zombidash.tga", true, true);
	m_Retina[4].VidInit("cstrike//resource//hud//zombie//zombihiding2.tga", true, true);
	m_Retina[5].VidInit("cstrike//resource//hud//zombie//zombitrap.tga", true, true);	
}

void CHudZB4UI::Draw(float flTime)
{
	if (g_iMod != MOD_ZB4)
		return;

	if(g_bAlive)
	{
		TEXD_SetRenderMode(kRenderTransTexture);
		m_Bink.SetSize(ScreenWidth, ScreenHeight);
		m_Bink.Paint(flTime);
		//m_Retina.SetCanDraw(true);
		
		for (int i=0;i<8;i++)
			m_Retina[i].Redraw(flTime);
	}

	m_ScoreBoard.Redraw();
	//m_Radar.Redraw();
	m_Power.Redraw();
	m_Icon.Redraw();
	m_Tip.Redraw(flTime);		
}

void CHudZB4UIBase::DrawNumber(int n, int x, int y, float size)
{
	TEXD_BindTexture(m_iNumber);
	TEXD_DrawTexture(x, y, 256, 32, &m_rcNumber[n], size);
}

void CHudZB4UIBase::DrawTexture(int id, wrect_t rc, int iX, int iY)
{	
	int iW, iH;
	iW = rc.right;
	iH = rc.bottom;
	
	vgui::surface()->DrawSetTexture(id);
	vgui::surface()->DrawTexturedRect(iX, iY, iX + iW, iY + iH);
}

void CHudZB4UIBase::LoadTexture(char *name, int &id, wrect_t &rc)
{	
	int i = Hud().m_TGA.FindTexture(name);
	id = g_Texture[i].iTexture;
	rc.right = g_Texture[i].iWidth;
	rc.bottom = g_Texture[i].iHeight;
}

void CHudZB4UIBase::DrawNumbers(int n, int x, int y, int from_right, float size)
{
	int width = 14;
	width *= size;

	int k;
	
	/*if(from_right)
		x += 14 * max * size;*/

	if (n >= 100000)
	{
		k = n / 100000;
		DrawNumber(k, x, y, size);
		if(!from_right)
			x += width;
	}
	if(from_right)
		x += width;

	if (n >= 10000)
	{
		k = (n % 100000) / 10000;
		DrawNumber(k, x, y, size);
		if(!from_right)
			x += width;
	}
	if(from_right)
		x += width;

	if (n >= 1000)
	{
		k = (n % 10000) / 1000;
		DrawNumber(k, x, y, size);		
		if(!from_right)
			x += width;
	}
	if(from_right)
		x += width;

	if (n >= 100)
	{
		k = (n % 1000) / 100;
		DrawNumber(k, x, y, size);		
		if(!from_right)
			x += width;
	}
	if(from_right)
		x += width;

	if (n >= 10)
	{
		k = (n % 100) / 10;
		DrawNumber(k, x, y, size);
		if(!from_right)
			x += width;
	}
	if(from_right)
		x += width;

	k = n % 10;
	DrawNumber(k, x, y, size);
}

int CHudZB4UIBase::GetNumWidth(int n, int w, float size)
{
	w *= size;

	int width = 0;

	if (n < 0)
		return w;

	if (n >= 1000000)
		width += w;

	if (n >= 100000)
		width += w;

	if (n >= 10000)
		width += w;

	if (n >= 1000)
		width += w;

	if (n >= 100)
		width += w;

	if (n >= 10)
		width += w;

	width += w;
	return width;
}

void CHudZB4UIBase::BuildNumberRC(wrect_t *rgrc, int w, int h)
{
	int nw = 0;
	int nh = 0;

	for (int i = 0; i < 10; i++)
	{
		rgrc[i].left = nw;
		rgrc[i].top = 0;
		rgrc[i].right = rgrc[i].left + w;
		rgrc[i].bottom = h;

		nw += w;
		nh += h;
	}
}

void CHudZB4UIPower::Init()
{
	int i = Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4number");
	m_iNumber = g_Texture[i].iTexture;
	BuildNumberRC(m_rcNumber, 14, 32);

	LoadTexture("resource\\hud\\zombie\\adrenalinebg", m_iPowerBackground, m_rcPowerBackground);
	LoadTexture("resource\\hud\\zombie\\adrenalinnegauge", m_iPowerLine, m_rcPowerLine);
	LoadTexture("resource\\hud\\zombie\\adrenalinnegauge2", m_iPowerLine2, m_rcPowerLine2);
	LoadTexture("resource\\hud\\zombie\\adrenalinneleft", m_iPowerLeft2, m_rcPowerLeft2);
	LoadTexture("resource\\hud\\zombie\\adrenalinnepoint", m_iPowerPoint, m_rcPowerPoint);
	LoadTexture("resource\\hud\\zombie\\z4_6key", m_iKey[0], m_rcKey[0]);
	LoadTexture("resource\\hud\\zombie\\right", m_iKey[1], m_rcKey[1]);

	m_iSkillUsage = 11;
	m_iText = 0;
	m_iPower[0] = 0;
	m_iPower[1] = 0;
	m_iPower2[0] = 0;
	m_iPower2[1] = 0;
	m_iPowerServer = 0;
	m_iUseTimesOld = -1;

	m_flPowerStartRefreshTime[0] = 0.0f;
	m_flPowerEndRefreshTime[0] = 0.0f;
	m_flPowerStartRefreshTime[1] = 0.0f;
	m_flPowerEndRefreshTime[1] = 0.0f;

	m_flFlashTime = 0.0f;


	wcscpy(m_szDrawText, GetLangUni("CSBTE_ZB4_Skill_Human"));
	g_Font.LoadStr(m_szDrawText);

	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Format"));
	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Normal"));
	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Speed"));
	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Heavy"));
	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Hidden"));

}

void CHudZB4UIPower::Redraw()
{
	if (!g_bAlive)
		return;

	int iX, iY;
	iX = ScreenWidth / 2 - m_rcPowerBackground.right / 2;
	iY = ScreenHeight - m_rcPowerBackground.bottom - 60;

	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	TEXD_SetRenderMode(kRenderTransTexture);
	DrawTexture(m_iPowerBackground, m_rcPowerBackground, iX, iY);

	float flFlashTime = Hud().m_flTime - m_flFlashTime;

	if (2 * flFlashTime >= M_PI)
		m_flFlashTime = Hud().m_flTime;

	m_iAlpha = 255.0f * sin(2 * flFlashTime);

	if (m_iAlpha > 255)
		m_iAlpha = 255;

	if (m_iAlpha < 0)
		m_iAlpha = 0;

	vgui::surface()->DrawSetColor(255, 255, 255, m_iAlpha);
	TEXD_SetRenderMode(kRenderTransTexture);
	DrawTexture(m_iPowerLeft2, m_rcPowerLeft2, iX, iY);


	for (int i = 0; i <= 3; i++)
	{
		vgui::surface()->DrawSetColor(255, 255, 255, 255);
		DrawTexture(m_iPowerPoint, m_rcPowerPoint, iX + 141 + i * 42, iY + 44);
	}

	iX += 108;
	iY += 46;

	int iPower = m_iPowerServer;

	if (m_iPowerServer < m_iPower[0] && m_flPowerEndRefreshTime[0] > Hud().m_flTime)
	{
		iPower = m_iPower2[0] + (m_iPower[0] - m_iPower2[0]) * (m_flPowerEndRefreshTime[0] - Hud().m_flTime) / (m_flPowerEndRefreshTime[0] - m_flPowerStartRefreshTime[0]);

		if (iPower < 0)
			iPower = 0;

		if (m_iPowerServer > m_iPower[1] && m_flPowerEndRefreshTime[1] > Hud().m_flTime)
			m_rcPowerLine2.left = m_iPower[1] / 100.0f * 210;
		else
			m_rcPowerLine2.left = m_iPowerServer / 100.0f * 210;

		m_rcPowerLine2.right = iPower / 100.0f * 210;

		if (m_rcPowerLine2.right >= m_iPower[1] / 100.0f * 210)
		{
			TEXD_BindTexture(m_iPowerLine2);
			TEXD_DrawTexture(iX + m_rcPowerLine2.left, iY, 252, 16, &m_rcPowerLine2);
		}
	}

	if (m_iPowerServer > m_iPower[1] && m_flPowerEndRefreshTime[1] > Hud().m_flTime)
	{
		iPower = m_iPower2[1] - (m_iPower2[1] - m_iPower[1]) * (m_flPowerEndRefreshTime[1] - Hud().m_flTime) / (m_flPowerEndRefreshTime[1] - m_flPowerStartRefreshTime[1]);

		if (iPower > 100)
			iPower = 100;

		m_rcPowerLine.left = 0/*m_iPower[1] / 100.0f * 210*/;
		m_rcPowerLine.right = iPower / 100.0f * 210;

		TEXD_SetColor(225, 225, 225, 255);
		TEXD_BindTexture(m_iPowerLine);
		TEXD_DrawTexture(iX + m_rcPowerLine.left, iY, 252, 16, &m_rcPowerLine);
	}
	else
	{
		m_iPower[1] = m_iPowerServer;

		m_rcPowerLine.left = 0/*m_iPower[1] / 100.0f * 210*/;
		m_rcPowerLine.right = m_iPowerServer / 100.0f * 210;

		TEXD_SetColor(225, 225, 225, 255);
		TEXD_BindTexture(m_iPowerLine);
		TEXD_DrawTexture(iX + m_rcPowerLine.left, iY, 252, 16, &m_rcPowerLine);
	}

	m_rcPowerLine.left = 0;
	m_rcPowerLine.right = m_iPowerServer / 100.0f * 210/*m_iPower[1] / 100.0f * 210*/;

	TEXD_SetColor(225, 225, 225, 200);

	TEXD_BindTexture(m_iPowerLine);
	TEXD_DrawTexture(iX, iY, 252, 16, &m_rcPowerLine);

	/*TEXD_SetColor(225, 225, 225, m_iAlpha);

	TEXD_BindTexture(m_iPowerLine);
	TEXD_DrawTexture(iX, iY, 252, 16, &m_rcPowerLine);*/


	iX = ScreenWidth / 2 - m_rcPowerBackground.right / 2;
	iY = ScreenHeight - m_rcPowerBackground.bottom - 60;


	iX += 52;
	iY += 43;

	TEXD_SetColor(225, 225, 225, 255);

	if (iPower < 10)
		iX += 6;

	if (iPower == 100)
		iX -= 10;

	DrawNumbers(iPower, iX, iY, false, 1.1);

#if 0
	iX = 10;
	iY = ScreenHeight - 95;
#else
	iX = ScreenWidth / 2 - m_rcPowerBackground.right / 2 + 95;
	iY = ScreenHeight - m_rcPowerBackground.bottom - 60 - 15;
#endif
	if (g_iTeam == 1 && m_iPowerServer >= 30)
	{
		if (!m_iTextFlash)
		{
			m_iTextFlash = 1;
			m_flTextFlashStart = Hud().m_flTime;
		}

		float flFlashTime = Hud().m_flTime - m_flTextFlashStart;

		int iAlpha = abs(sin(flFlashTime * M_PI / 1.5)) * 255 * 0.9 + 25;
		iAlpha = iAlpha>255 ? 255 : iAlpha;
		iAlpha = iAlpha<0 ? 0 : iAlpha;

		if (flFlashTime >= 3.75)
			iAlpha = 255;

		vgui::surface()->DrawSetColor(255, 255, 255, iAlpha);
		TEXD_SetRenderMode(kRenderTransTexture);
		DrawTexture(m_iKey[0], m_rcKey[0], iX, iY);

		iX += m_rcKey[0].right + 5;

		g_Font.SetColor(251, 201, 96, iAlpha);
		g_Font.SetWidth(20);
		g_Font.DrawString(m_szDrawText, iX, iY + 30, 200);
	}
	else if (g_iTeam == 2)
	{
		m_iTextFlash = 0;
		m_flTextFlashStart = 0.0f;


		if (m_iUseTimesOld != int(m_iPowerServer / m_iSkillUsage))
		{
			m_iUseTimesOld = m_iPowerServer / m_iSkillUsage;

			wchar_t szBuf[3][256];

			wchar_t *pBuf[2];
			pBuf[0] = vgui::localize()->Find("CSBTE_ZB4_Skill_Format");

			switch (m_iText)
			{
			case 0: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Normal"); break;
			case 1: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Speed"); break;
			case 2: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Heavy"); break;
			case 3: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Hidden"); break;
			case 4: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Humpback"); break;
			}
			memset(szBuf, 0, sizeof(szBuf));
			memset(m_szDrawText2, 0, sizeof(m_szDrawText2));

			wcsncpy(szBuf[0], pBuf[0], sizeof(szBuf[0]));
			wcsncpy(szBuf[1], pBuf[1], sizeof(szBuf[1]));

			szBuf[2][0] = L'0' + m_iUseTimesOld;
			szBuf[2][1] = 0;

			vgui::localize()->ConstructString(m_szDrawText2, 64, szBuf[0], 2, szBuf[1], szBuf[2]);
		}

		vgui::surface()->DrawSetColor(255, 255, 255, 255);
		TEXD_SetRenderMode(kRenderTransTexture);
		DrawTexture(m_iKey[1], m_rcKey[1], iX, iY);

		iX += m_rcKey[1].right + 5;

		g_Font.SetColor(251, 201, 96, 255);
		g_Font.SetWidth(20);
		g_Font.DrawString(m_szDrawText2, iX, iY + 30, 200);

	}
}

void CHudZB4UIPower::SetPower(int power, float flashtime)
{
	if (power < m_iPowerServer)
	{
		if (m_iPowerServer < m_iPower[0] && m_flPowerEndRefreshTime[0] > Hud().m_flTime)
		{
			m_iPower2[0] = power;
			m_flPowerEndRefreshTime[0] = Hud().m_flTime + flashtime;
		}
		else
		{
			m_iPower[0] = m_iPowerServer;
			m_iPower2[0] = power;
			m_flPowerEndRefreshTime[0] = Hud().m_flTime + flashtime;
			m_flPowerStartRefreshTime[0] = Hud().m_flTime;
			m_iAlpha = 0;
		}

	}

	if (power > m_iPowerServer)
	{
		if (m_iPowerServer > m_iPower[1] && m_flPowerEndRefreshTime[1] > Hud().m_flTime)
		{
			m_iPower2[1] = power;
			m_flPowerEndRefreshTime[1] = Hud().m_flTime + flashtime;
		}
		else
		{
			m_iPower[1] = m_iPowerServer;
			m_iPower2[1] = power;
			m_flPowerEndRefreshTime[1] = Hud().m_flTime + flashtime;
			m_flPowerStartRefreshTime[1] = Hud().m_flTime;
		}
	}

	m_iPowerServer = power;

}

void CHudZB4UIIcon::Init()
{
	int i = Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4number");
	m_iNumber = g_Texture[i].iTexture;
	BuildNumberRC(m_rcNumber, 14, 32);

	LoadTexture("resource\\hud\\zombie\\z4_powerup", m_iIcon[0], m_rcIcon[0]);
	LoadTexture("resource\\hud\\zombie\\z4_criticalshot", m_iIcon[1], m_rcIcon[1]);
	LoadTexture("resource\\hud\\zombie\\z4_midnightup", m_iIcon[2], m_rcIcon[2]);
	LoadTexture("resource\\hud\\zombie\\z4_midnightdown", m_iIcon[3], m_rcIcon[3]);
	LoadTexture("resource\\hud\\zombie\\z4_speedup", m_iIcon[4], m_rcIcon[4]);
	LoadTexture("resource\\hud\\zombie\\z4_speeddown", m_iIcon[5], m_rcIcon[5]);
	LoadTexture("resource\\hud\\zombie\\z4_cannotcontrol", m_iIcon[6], m_rcIcon[6]);
	LoadTexture("resource\\hud\\zombie\\z4_defenseup", m_iIcon[7], m_rcIcon[7]);
	LoadTexture("resource\\hud\\zombie\\z4_defensedown", m_iIcon[8], m_rcIcon[8]);
	LoadTexture("resource\\hud\\zombie\\z4_hpup", m_iIcon[9], m_rcIcon[9]);
	LoadTexture("resource\\hud\\zombie\\z4_hpdown", m_iIcon[10], m_rcIcon[10]);

	memset(m_iStatus, 0, sizeof(m_iStatus));
	memset(m_iAlpha, 0, sizeof(m_iAlpha));
	memset(m_flFlashTime, 0, sizeof(m_flFlashTime));
	memset(m_iFlash, 0, sizeof(m_iFlash));
}

void CHudZB4UIIcon::Redraw()
{
	if (!g_bAlive)
		return;

	if (!g_iTeam || g_iTeam == 3)
		return;

	m_iTeam = g_iTeam - 1;

	int iX, iY;
	iX = X_Start;
	iY = ScreenHeight * 0.54;

	int iIcons = 0;

	for (int i = 0; i<12; i++)
	{
		if (m_iStatus[i] == Flash)
		{
			float flFlashTime = Hud().m_flTime - m_flFlashTime[i];

			if (2.5 * flFlashTime >= M_PI)
			{
				m_flFlashTime[i] = Hud().m_flTime;
				m_iFlash[i] --;
			}

			m_iAlpha[i] = 255.0f * sin(2.5 * flFlashTime);

			if (m_iAlpha[i] > 255)
				m_iAlpha[i] = 255;

			if (m_iAlpha[i] < 0)
				m_iAlpha[i] = 0;

			if (m_iFlash[i] <= 1 && m_iAlpha[i] >= 250)
			{
				m_iAlpha[i] = 255;
				m_iStatus[i] = Normal;
			}
		}

		if (m_iStatus[i] == Flash2)
		{
			float flFlashTime = Hud().m_flTime - m_flFlashTime[i];

			if (5.5 * flFlashTime >= M_PI)
			{
				m_flFlashTime[i] = Hud().m_flTime;
				m_iFlash[i] --;
			}

			m_iAlpha[i] = 255.0f * abs(sin(5.5 * flFlashTime + 0.5 * M_PI));

			if (m_iAlpha[i] < 50)	m_iAlpha[i] = 0;

			m_iAlpha[i] *= 1.25;
			if (m_iAlpha[i] > 255) m_iAlpha[i] = 255;

			if (m_iFlash[i] <= 1 && m_iAlpha[i] <= 10)
			{
				m_iAlpha[i] = 255;
				m_iStatus[i] = Hide;
			}
		}

		if (m_iStatus[i] != Hide)
		{
			if (m_iStatus[i] == Normal)
				m_iAlpha[i] = 255;

			TEXD_SetRenderMode(kRenderTransColor);
			vgui::surface()->DrawSetColor(255, 255, 255, m_iAlpha[i]);
			DrawTexture(m_iIcon[m_iId[i]], m_rcIcon[m_iId[i]], iX, iY);

			iIcons++;

#ifndef DISABLE_LIGHT_UI
			TEXD_SetRenderMode(kRenderTransAdd);
			vgui::surface()->DrawSetColor(120, 120, 120, 255);
			DrawTexture(m_iIcon[i], m_rcIcon[i], iX, iY);
#endif
			iX += m_rcIcon[i].right + 5;
		}


		if ((i/*iIcons*/ + 1) % 4 == 0)
		{
			iX = X_Start;
			iY += m_rcIcon[i].bottom + 5;
		}
	}

	if (m_iStatus[0] != Hide)
	{
		char power[8];
		sprintf(power, "%d%%", m_iPower);

		wchar_t *ppower = UTF8ToUnicode(power);

		g_Font.SetWidth(14);
		g_Font.SetColor(125, 160, 200, m_iAlpha[0]);
		g_Font.DrawString(ppower, X_Start + m_rcIcon[0].right - g_Font.GetLen(ppower) - 3, ScreenHeight * 0.54 + m_rcIcon[0].bottom - 6, 100);

#ifndef DISABLE_LIGHT_UI
		TEXD_SetRenderMode(kRenderGlow);
		vgui::surface()->DrawSetColor(90, 90, 90, 255);
		DrawTexture(m_iIcon[0], m_rcIcon[0], 10, 340);
#endif
	}
}

void CHudZB4UIIcon::AddIcon(int slot, int id, int status)
{
	if (m_iFlash[slot] > 0 && m_iStatus[slot] == Flash)
		return;

	m_iId[slot] = id;
	m_iStatus[slot] = status;
	m_iAlpha[slot] = 0;
	m_flFlashTime[slot] = Hud().m_flTime;
	m_iFlash[slot] = 0;

	if (m_iStatus[slot] == Flash)
	{
		m_iFlash[slot] = 2;
		m_iAlpha[slot] = 0;
	}

	if (m_iStatus[slot] == Flash2)
	{
		m_iFlash[slot] = 2;
		m_iAlpha[slot] = 255;
	}
}

void CHudZB4UIRadar::Init()
{
	m_iBackground = g_Texture[Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4_radarbg")].iTexture;
	m_iCover = g_Texture[Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4_radarline")].iTexture;
}

void CHudZB4UIRadar::Redraw()
{
	if (gConfigs.bEnableNewHud)
		return;

	if (!g_bAlive || Hud().m_FOV <= 40.0)
		return;

	vgui::surface()->DrawSetTexture(m_iCover);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vgui::surface()->DrawTexturedRect(0, 0, 201, 200);
}

void CHudZB4UIRadar::DrawBackground()
{
	if (gConfigs.bEnableNewHud)
		return;

	if (!g_bAlive || Hud().m_FOV <= 40.0)
		return;

	vgui::surface()->DrawSetTexture(m_iBackground);
	TEXD_SetRenderMode(kRenderTransTexture);
	vgui::surface()->DrawSetColor(255, 255, 255, 150);
	vgui::surface()->DrawTexturedRect(0, 0, 201, 200);
}

void CHudZB4UITip::Init()
{

}

void CHudZB4UITip::VidInit()
{
	if (!m_iBg[0] || !m_iBg[1])
	{
		m_iBg[0] = vgui::surface()->CreateNewTextureID();
		LoadTexture("resource\\hud\\zombie\\z4_hmmsgbg", m_iBg[0], m_rcBg[0]);

		m_iBg[1] = vgui::surface()->CreateNewTextureID();
		LoadTextureTga("cstrike\\resource\\hud\\zombie\\z4_zbmsgbg.tga", m_iBg[1], m_rcBg[1].right, m_rcBg[1].bottom);
	}

	//LoadTexture("resource\\hud\\zombie\\z4_hmmsgbg", m_iBg[0], m_rcBg[0]);
	//LoadTexture("resource\\hud\\zombie\\z4_zbmsgbg", m_iBg[1], m_rcBg[1]);

	memset(m_szText, 0, sizeof(m_szText));

	m_iTeam = 2;
	m_flTimeStart = 0.0f;
	m_flTimeEnd = 0.0f;
	m_flDisplayEnd = 0.0f;
	m_iCount = -1;

	LoadText("CSBTE_ZB4_MSG_Human_RoundStart5_1");
	LoadText("CSBTE_ZB4_MSG_Human_RoundStart5_2");
	LoadText("CSBTE_ZB4_MSG_Human_RoundStart5_3");

	LoadText("CSBTE_ZB4_MSG_Human_RoundStart22_1");
	LoadText("CSBTE_ZB4_MSG_Human_RoundStart22_2");
	LoadText("CSBTE_ZB4_MSG_Human_RoundStart22_3");

	LoadText("CSBTE_ZB4_MSG_Human_NightBegin_1");
	LoadText("CSBTE_ZB4_MSG_Human_NightBegin_2");
	LoadText("CSBTE_ZB4_MSG_Human_NightBegin_3");

	LoadText("CSBTE_ZB4_MSG_Human_NightEnd_1");
	LoadText("CSBTE_ZB4_MSG_Human_NightEnd_2");
	LoadText("CSBTE_ZB4_MSG_Human_NightEnd_3");

	LoadText("CSBTE_ZB4_MSG_Zombi_FirstZombi_1");
	LoadText("CSBTE_ZB4_MSG_Zombi_FirstZombi_2");
	LoadText("CSBTE_ZB4_MSG_Zombi_FirstZombi_3");

	LoadText("CSBTE_ZB4_MSG_Zombi_NightBegin_1");
	LoadText("CSBTE_ZB4_MSG_Zombi_NightBegin_2");
	LoadText("CSBTE_ZB4_MSG_Zombi_NightBegin_3");

	LoadText("CSBTE_ZB4_MSG_Zombi_NightEnd_1");
	LoadText("CSBTE_ZB4_MSG_Zombi_NightEnd_2");
	LoadText("CSBTE_ZB4_MSG_Zombi_NightEnd_3");

	m_iNightStart = 100;
	m_iNightEnd = 40;

	g_iZ4Nvg = 0;
}

void CHudZB4UITip::Redraw(float flTime)
{
	if (g_iRoundTime < 15)
		return;

	m_iTeam = g_iTeam - 1;

	int time = m_flTimeEnd - flTime + 1;

	if (time > 180)
		return;

	int iX, iY;
	iX = ScreenWidth / 2 - m_rcBg[0].right / 2;
	iY = ScreenHeight * 0.18;

	if (m_iTeam == 1)
	{
		if (time > m_iNightEnd - 10 && time <= m_iNightStart + 10)
		{
			HudZombieMod4().m_Retina[2].SetCanDraw(true);
			HudZombieMod4().m_Retina[2].SetEndTime(flTime + 9999.0);
		}

		if (time >= m_iNightStart && time <= m_iNightStart + 10)
			HudZombieMod4().m_Retina[2].SetColor(255, 255, 255, float(m_iNightStart + 10.0 - (m_flTimeEnd - flTime + 1.0)) / 10.0 * 255);

		if (time >= m_iNightEnd - 10 && time <= m_iNightEnd)
			HudZombieMod4().m_Retina[2].SetColor(255, 255, 255, float((m_flTimeEnd - flTime + 1.0) - m_iNightEnd) / 10.0 * 255 + 255);
	}
	else
	{
		HudZombieMod4().m_Retina[2].SetCanDraw(false);
	}

	if (g_iZ4Nvg == 0 && g_bAlive)
	{
		g_iZ4Nvg = 1;
		if (time <= m_iNightStart && time >= m_iNightEnd)
			g_iZ4Nvg = 2;
	}

	if (m_iCount != time)
	{
		if (time == 180)
		{
			gEngfuncs.pfnClientCmd("mp3 stop");
			gEngfuncs.pfnClientCmd("mp3 play sound//music//ZB4_start.mp3");

			HudZombieMod4().m_Icon.AddIcon(0, 0, 2);

			m_flDisplayEnd = 0.0f;
		}

		if (time == m_iNightStart + 10)
		{
			g_iZ4Nvg = 3;
			g_flZ4Nvg = Hud().m_flTime;
		}

		if (time == m_iNightEnd + 10)
		{
			g_iZ4Nvg = 4;
			g_flZ4Nvg = Hud().m_flTime + 10.0f;
		}

		if (time == m_iNightStart + 5)
		{
			gEngfuncs.pfnClientCmd("mp3 stop");
			gEngfuncs.pfnClientCmd("mp3 play sound//music//Play_BGM_Rex.mp3");
		}

		if (time == m_iNightStart)
		{
			g_iZ4Nvg = 2;

			if (m_iTeam == 0)
			{
				HudZombieMod4().m_Icon.AddIcon(5, 3, 2);

				m_flDisplayEnd = Hud().m_flTime + 3.0f;

				int iRand = gEngfuncs.pfnRandomLong(0, 2);

				switch (iRand)
				{
				case 0: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_NightBegin_1")); break;
				case 1: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_NightBegin_2")); break;
				default: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_NightBegin_3")); break;
				}

				gEngfuncs.pfnClientCmd("spk sound/events/human_message.wav");
			}
			else
			{
				m_flDisplayEnd = Hud().m_flTime + 3.0f;

				int iRand = gEngfuncs.pfnRandomLong(0, 2);

				switch (iRand)
				{
				case 0: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Zombi_NightBegin_1")); break;
				case 1: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Zombi_NightBegin_2")); break;
				default: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Zombi_NightBegin_3")); break;
				}

				gEngfuncs.pfnClientCmd("spk sound/events/zombie_message.wav");

				HudZombieMod4().m_Icon.AddIcon(5, 2, 2);
				HudZombieMod4().m_Icon.AddIcon(4, 9, 2);
			}
		}

		if (time == m_iNightEnd - 1)
		{
			if (m_iTeam == 0)
			{
				HudZombieMod4().m_Icon.AddIcon(5, 3, 3);

			}
			else
			{
				HudZombieMod4().m_Icon.AddIcon(5, 2, 3);
				HudZombieMod4().m_Icon.AddIcon(4, 9, 3);
			}
		}

		if (time == m_iNightEnd)
		{
			if (m_iTeam == 0)
			{
				HudZombieMod4().m_Icon.AddIcon(5, 3, 3);

				m_flDisplayEnd = Hud().m_flTime + 3.0f;

				int iRand = gEngfuncs.pfnRandomLong(0, 2);

				switch (iRand)
				{
				case 0: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_NightEnd_1")); break;
				case 1: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_NightEnd_2")); break;
				default: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_NightEnd_3")); break;
				}

				gEngfuncs.pfnClientCmd("spk sound/events/human_message.wav");
			}
			else
			{
				HudZombieMod4().m_Icon.AddIcon(5, 2, 3);
				HudZombieMod4().m_Icon.AddIcon(4, 9, 3);
				HudZombieMod4().m_Icon.AddIcon(7, 5, 1);

				m_flDisplayEnd = Hud().m_flTime + 3.0f;

				int iRand = gEngfuncs.pfnRandomLong(0, 2);

				switch (iRand)
				{
				case 0: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Zombi_NightEnd_1")); break;
				case 1: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Zombi_NightEnd_2")); break;
				default: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Zombi_NightEnd_3")); break;
				}

				gEngfuncs.pfnClientCmd("spk sound/events/zombie_message.wav");
			}
		}

		if (time == m_iNightEnd - 4 && m_iTeam == 1)
		{
			HudZombieMod4().m_Icon.AddIcon(7, 5, 3);
		}

		if (time == 175)
		{
			m_flDisplayEnd = Hud().m_flTime + 3.0f;

			int iRand = gEngfuncs.pfnRandomLong(0, 2);

			switch (iRand)
			{
			case 0: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart5_1")); break;
			case 1: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart5_2")); break;
			default: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart5_3")); break;
			}

			gEngfuncs.pfnClientCmd("spk sound/events/human_message.wav");
		}

		if (time == 158 && m_iTeam == 0)
		{
			m_flDisplayEnd = Hud().m_flTime + 3.0f;

			int iRand = gEngfuncs.pfnRandomLong(0, 2);

			if (m_iTeam == 0)
			{
				switch (iRand)
				{
				case 0: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart22_1")); break;
				case 1: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart22_2")); break;
				default: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart22_3")); break;
				}
				gEngfuncs.pfnClientCmd("spk sound/events/human_message.wav");
			}
		}

		if (time == 158 && m_iTeam == 1)
		{
			m_flDisplayEnd = Hud().m_flTime + 3.0f;

			int iRand = gEngfuncs.pfnRandomLong(0, 2);

			if (m_iTeam == 1)
			{
				switch (iRand)
				{
				case 0: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Zombi_FirstZombi_1")); break;
				case 1: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Zombi_FirstZombi_2")); break;
				default: wcscpy(m_szText, GetLangUni("CSBTE_ZB4_MSG_Zombi_FirstZombi_3")); break;
				}
				gEngfuncs.pfnClientCmd("spk sound/events/zombie_message.wav");
			}

		}


		if (time > 160)
		{
			char count[4];
			sprintf(count, "%d", time - 160);

			MSG_BeginWrite("TextMsg");
			MSG_WriteByte(4);
			MSG_WriteString("#CSBTE_ZombiSelectCount");
			MSG_WriteString(count);
			MSG_EndWrite();

			if (time <= 171)
			{
				switch (time - 161)
				{
				case 0: gEngfuncs.pfnClientCmd("spk sound/vox/one.wav"); break;
				case 1: gEngfuncs.pfnClientCmd("spk sound/vox/two.wav"); break;
				case 2: gEngfuncs.pfnClientCmd("spk sound/vox/three.wav"); break;
				case 3: gEngfuncs.pfnClientCmd("spk sound/vox/four.wav"); break;
				case 4: gEngfuncs.pfnClientCmd("spk sound/vox/five.wav"); break;
				case 5: gEngfuncs.pfnClientCmd("spk sound/vox/six.wav"); break;
				case 6: gEngfuncs.pfnClientCmd("spk sound/vox/seven.wav"); break;
				case 7: gEngfuncs.pfnClientCmd("spk sound/vox/eight.wav"); break;
				case 8: gEngfuncs.pfnClientCmd("spk sound/vox/nine.wav"); break;
				case 9: gEngfuncs.pfnClientCmd("spk sound/vox/ten.wav"); break;
				}
			}
		}

		if (time > m_iNightStart)
		{
			if (m_iTeam == 1)
				HudZombieMod4().m_Icon.AddIcon(0, 0, 0);
		}

		m_iCount = time;
	}




	if (m_flDisplayEnd > Hud().m_flTime)
	{
		/*TEXD_SetRenderMode(kRenderTransColor);
		vgui::surface()->DrawSetColor(255, 255, 255, 255);
		DrawTexture(m_iBg[m_iTeam], m_rcBg[m_iTeam], iX, iY);
		*/
		int r, g, b, a;

		a = 255;

		if (m_flDisplayEnd - Hud().m_flTime > 2.6)
			a = abs((m_flDisplayEnd - Hud().m_flTime - 3.0f) * 255 / 0.4);

		if (a > 255) a = 255;

		if (m_flDisplayEnd - Hud().m_flTime < 0.5)
			a = (m_flDisplayEnd - Hud().m_flTime) * 255 * 2;


		vgui::surface()->DrawSetColor(255, 255, 255, a);

		DrawTexture(m_iBg[m_iTeam], m_rcBg[m_iTeam], iX, iY);

		/*Tri_Enable(GL_TEXTURE_2D);
		Tri_BindTexture(GL_TEXTURE_2D, m_iBg[m_iTeam]);

		glColor4ub(255, 255, 255, a);

		Tri_Enable(GL_BLEND);
		Tri_Enable(GL_ALPHA_TEST);
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Tri_AlphaFunc(GL_GREATER, 0.0);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);

		gEngfuncs.pTriAPI->TexCoord2f(0,1);
		gEngfuncs.pTriAPI->Vertex3f(iX,iY+m_rcBg[m_iTeam].bottom,0);

		gEngfuncs.pTriAPI->TexCoord2f(1,1);
		gEngfuncs.pTriAPI->Vertex3f(iX+m_rcBg[m_iTeam].right,iY+m_rcBg[m_iTeam].bottom,0);

		gEngfuncs.pTriAPI->TexCoord2f(1,0);
		gEngfuncs.pTriAPI->Vertex3f(iX+m_rcBg[m_iTeam].right,iY,0);

		gEngfuncs.pTriAPI->TexCoord2f(0,0);
		gEngfuncs.pTriAPI->Vertex3f(iX,iY,0);
		gEngfuncs.pTriAPI->End();*/

		int a2;

		GetFontColor(m_iTeam, r, g, b, a2);

		g_Font.SetWidth(22);
		g_Font.SetColor(r, g, b, int(a * (a2 / 255.0f)));

		iX += 127;
		iY += 80;

		g_Font.DrawString(m_szText, iX, iY, 1000);
	}
}

void CHudZB4UITip::SetTimeEnd(float flTime)
{
	if (flTime < 60)
		return;

	m_flTimeStart = Hud().m_flTime;
	m_flTimeEnd = Hud().m_flTime + flTime;
}

void CHudZB4UITip::ZombieIconsCheck(float flTime)
{
	int time = m_flTimeEnd - flTime + 1;

	if (time > m_iNightEnd && time <= m_iNightStart)
	{
		HudZombieMod4().m_Icon.AddIcon(5, 2, 2);
		HudZombieMod4().m_Icon.AddIcon(4, 9, 2);
	}
}

void CHudZB4UITip::LoadText(char *text)
{
	wchar_t text2[128];

	wcscpy(text2, GetLangUni(text));
	g_Font.LoadStr(text2);
}

void CHudZB4UITip::GetFontColor(int team, int &r, int &g, int &b, int &a)
{
	if (m_iTeam)
	{
		r = 180;
		g = 80;
		b = 75;
		a = 200;
	}
	else
	{
		r = 146;
		g = 158;
		b = 185;
		a = 200;
	}
}

void CHudZB4UIScoreBoard::Init()
{
	int i;
	i = Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4_scoreboard");
	m_iScroreBorad = g_Texture[i].iTexture;
	m_rcScroreBorad.right = g_Texture[i].iWidth;
	m_rcScroreBorad.bottom = g_Texture[i].iHeight;

	i = Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4_scorebg");
	m_iDamageBorad = g_Texture[i].iTexture;
	m_rcDamageBorad.right = g_Texture[i].iWidth;
	m_rcDamageBorad.bottom = g_Texture[i].iHeight;

	i = Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4_scorehuman");
	m_iTeamIcon[0] = g_Texture[i].iTexture;
	m_rcTeamIcon[0].right = g_Texture[i].iWidth;
	m_rcTeamIcon[0].bottom = g_Texture[i].iHeight;

	i = Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4_scorezombie");
	m_iTeamIcon[1] = g_Texture[i].iTexture;
	m_rcTeamIcon[1].right = g_Texture[i].iWidth;
	m_rcTeamIcon[1].bottom = g_Texture[i].iHeight;

	i = Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4number");
	m_iNumber = g_Texture[i].iTexture;
	BuildNumberRC(m_rcNumber, 14, 32);

	m_iTeam = 1;
	m_iDamage = 0;
	m_iLastScore = 0;
}

void CHudZB4UIScoreBoard::Redraw()
{
	DrawScoreBoard();
	DrawScore();
}

void CHudZB4UIScoreBoard::SetTeam(int team)
{
	m_iTeam = team;
}

void CHudZB4UIScoreBoard::SetDamage(int damage)
{
	m_iDamage = damage;
}

void CHudZB4UIScoreBoard::DrawScoreBoard()
{
	int iW, iH, iX, iY;
	iW = m_rcScroreBorad.right;
	iH = m_rcScroreBorad.bottom;
	iX = ScreenWidth / 2 - iW / 2;
	iY = 0;

	vgui::surface()->DrawSetTexture(m_iScroreBorad);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	TEXD_SetRenderMode(kRenderTransTexture);
	vgui::surface()->DrawTexturedRect(iX, iY, iX + iW, iY + iH);

	TEXD_SetColor(255, 255, 255, 255);

	iX = ScreenWidth / 2 - 14;
	iY = 27;
	DrawNumbers(13, iX, iY, false);

	TEXD_SetColor(245, 245, 245, 255);

	iX = ScreenWidth / 2 - 136;
	iY = 7;
	DrawNumbers(HudScoreBoard().m_iLeft, iX, iY, true, 0.9);

	iX = ScreenWidth / 2 + 67;
	iY = 7;
	DrawNumbers(HudScoreBoard().m_iRight, iX, iY, false, 0.9);

	TEXD_SetColor(200, 200, 200, 255);

	HudScoreBoard().m_iLeft2 = 0;
	HudScoreBoard().m_iRight2 = 0;

	for (int i = 1; i<33; i++)
	{
		if (vPlayer[i].team == 2 && vPlayer[i].alive) HudScoreBoard().m_iLeft2 += 1;
		if (vPlayer[i].team == 1 && vPlayer[i].alive) HudScoreBoard().m_iRight2 += 1;
	}

	iX = ScreenWidth / 2 - 115;
	iY = 34;
	DrawNumbers(HudScoreBoard().m_iLeft2, iX, iY, true, 0.65);

	iX = ScreenWidth / 2 + 63;
	iY = 34;
	DrawNumbers(HudScoreBoard().m_iRight2, iX, iY, false, 0.65);
}

void CHudZB4UIScoreBoard::DrawScore()
{
	if (m_iTeam == 0 || m_iTeam == 3)
		return;

	if (g_szCurWeapon2 && g_bAlive)
	{
		g_Font.SetColor(200, 144, 72, 255);
		g_Font.SetWidth(17);

		if (gConfigs.bEnableNewHud)
			g_Font.DrawString(g_szCurWeapon2, ScreenWidth - g_Font.GetLen(g_szCurWeapon2) - 18, ScreenHeight - 50 - ScreenHeight * 0.2, 1000);
		else
			g_Font.DrawString(g_szCurWeapon2, ScreenWidth - g_Font.GetLen(g_szCurWeapon2) - 18, ScreenHeight - 50, 1000);

	}

	int iW, iH, iX, iY;
	iW = m_rcDamageBorad.right;
	iH = m_rcDamageBorad.bottom;
	iX = ScreenWidth - iW - 7;
	iY = ScreenHeight - iH - 80;

	if (gConfigs.bEnableNewHud)
		iY -= ScreenHeight * 0.2;

	vgui::surface()->DrawSetTexture(m_iDamageBorad);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	TEXD_SetRenderMode(kRenderTransColor);
	vgui::surface()->DrawTexturedRect(iX, iY, iX + iW, iY + iH);

#ifndef DISABLE_LIGHT_UI
	TEXD_SetRenderMode(kRenderGlow);
	vgui::surface()->DrawSetColor(255, 255, 255, 150);
	vgui::surface()->DrawTexturedRect(iX, iY, iX + iW, iY + iH);
#endif

	int team = m_iTeam - 1;

	iW = m_rcTeamIcon[team].right;
	iH = m_rcTeamIcon[team].bottom;
	iX = ScreenWidth - iW - 122;
	iY = ScreenHeight - iH - 88;

	if (gConfigs.bEnableNewHud)
		iY -= ScreenHeight * 0.2;

	vgui::surface()->DrawSetTexture(m_iTeamIcon[team]);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	TEXD_SetRenderMode(kRenderTransColor);
	vgui::surface()->DrawTexturedRect(iX, iY, iX + iW, iY + iH);

#ifndef DISABLE_LIGHT_UI
	TEXD_SetRenderMode(kRenderGlow);
	vgui::surface()->DrawSetColor(255, 255, 255, 200);
	vgui::surface()->DrawTexturedRect(iX, iY, iX + iW, iY + iH);
#endif

	TEXD_SetColor(200, 144, 72, 255);

	iX = ScreenWidth - iW - 56;
	iY = ScreenHeight - iH - 80;

	if (gConfigs.bEnableNewHud)
		iY -= ScreenHeight * 0.2;

	int iScore = g_PlayerExtraInfo[gEngfuncs.GetLocalPlayer()->index].frags - m_iLastScore;
	if (iScore < 0)
		iScore = 0;

	DrawNumbers(iScore, iX, iY, true, 0.95);

	iX = ScreenWidth - iW - 44;
	iY = ScreenHeight - iH - 56;

	if (gConfigs.bEnableNewHud)
		iY -= ScreenHeight * 0.2;

	if (m_iDamage > 99999)
		m_iDamage = 99999;

	DrawNumbers(m_iDamage, iX, iY, true, 0.8);
}

