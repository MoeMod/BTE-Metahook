#include "base.h"
#include "BaseUI.h"

#define	Hide	0
#define	Normal	1
#define	Flash	2
#define	Flash2	3

#define DISABLE_LIGHT_UI

#define X_Start 10

void CHudZB4UIIcon::Init()
{
	int i = gHUD.m_TGA.FindTexture("resource\\hud\\zombie\\z4number");
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

	for(int i=0;i<12;i++)
	{		
		if(m_iStatus[i] == Flash)
		{
			float flFlashTime = gHUD.m_flTime - m_flFlashTime[i];

			if(2.5 * flFlashTime >= M_PI)
			{
				m_flFlashTime[i] = gHUD.m_flTime;
				m_iFlash[i] --;
			}

			m_iAlpha[i] = 255.0f * sin(2.5 * flFlashTime);

			if(m_iAlpha[i] > 255)
				m_iAlpha[i] = 255;

			if(m_iAlpha[i] < 0)
				m_iAlpha[i] = 0;

			if(m_iFlash[i] <= 1 && m_iAlpha[i] >= 250)
			{
				m_iAlpha[i] = 255;
				m_iStatus[i] = Normal;
			}
		}

		if(m_iStatus[i] == Flash2)
		{			
			float flFlashTime = gHUD.m_flTime - m_flFlashTime[i];			

			if(5.5 * flFlashTime >= M_PI)
			{
				m_flFlashTime[i] = gHUD.m_flTime;
				m_iFlash[i] --;
			}

			m_iAlpha[i] = 255.0f * abs(sin(5.5 * flFlashTime + 0.5 * M_PI));
						
			if(m_iAlpha[i] < 50)	m_iAlpha[i] = 0;

			m_iAlpha[i] *= 1.25;
			if(m_iAlpha[i] > 255) m_iAlpha[i] = 255;

			if(m_iFlash[i] <= 1 && m_iAlpha[i] <= 10)
			{
				m_iAlpha[i] = 255;
				m_iStatus[i] = Hide;
			}
		}

		if(m_iStatus[i] != Hide)
		{			
			if(m_iStatus[i] == Normal)
				m_iAlpha[i] = 255;

			TEXD_SetRenderMode(kRenderTransColor);
			vgui::surface()->DrawSetColor(255, 255, 255, m_iAlpha[i]);
			DrawTexture(m_iIcon[m_iId[i]], m_rcIcon[m_iId[i]], iX, iY);

			iIcons ++;

#ifndef DISABLE_LIGHT_UI
			TEXD_SetRenderMode(kRenderTransAdd);
			vgui::surface()->DrawSetColor(120, 120, 120, 255);
			DrawTexture(m_iIcon[i], m_rcIcon[i], iX, iY);
#endif
			iX += m_rcIcon[i].right + 5;
		}		
		

		if((i/*iIcons*/ + 1) % 4 == 0)
		{
			iX = X_Start;
			iY += m_rcIcon[i].bottom + 5;
		}
	}

	if(m_iStatus[0] != Hide)
	{
		char power[8];
		sprintf(power, "%d%%", m_iPower);

		wchar_t *ppower = UTF8ToUnicode(power);

		g_Font.SetWidth(14);
		g_Font.SetColor(125, 160, 200, m_iAlpha[0]);
		g_Font.DrawString(ppower, X_Start + m_rcIcon[0].right - g_Font.GetLen(ppower) - 3, ScreenHeight * 0.54 +  m_rcIcon[0].bottom - 6, 100);

#ifndef DISABLE_LIGHT_UI
		TEXD_SetRenderMode(kRenderGlow);
		vgui::surface()->DrawSetColor(90, 90, 90, 255);
		DrawTexture(m_iIcon[0], m_rcIcon[0], 10, 340);
#endif
	}
}

void CHudZB4UIIcon::AddIcon(int slot, int id, int status)
{	
	if(m_iFlash[slot] > 0 && m_iStatus[slot] == Flash)
		return;

	m_iId[slot] = id;
	m_iStatus[slot] = status;
	m_iAlpha[slot] = 0;
	m_flFlashTime[slot] = gHUD.m_flTime;
	m_iFlash[slot] = 0;

	if(m_iStatus[slot] == Flash)
	{		
		m_iFlash[slot] = 2;		
		m_iAlpha[slot] = 0;
	}

	if(m_iStatus[slot] == Flash2)
	{
		m_iFlash[slot] = 2;
		m_iAlpha[slot] = 255;
	}
}