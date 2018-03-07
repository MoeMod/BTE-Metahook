#include "base.h"
#include "BaseUI.h"

void CHudZB4UIPower::Init()
{	
	int i = gHUD.m_TGA.FindTexture("resource\\hud\\zombie\\z4number");
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


	wcscpy(m_szDrawText,GetLangUni("CSBTE_ZB4_Skill_Human"));
	g_Font.LoadStr(m_szDrawText);

	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Format"));
	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Normal"));
	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Speed"));
	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Heavy"));
	g_Font.LoadStr(GetLangUni("CSBTE_ZB4_Skill_Hidden"));
	
}

void CHudZB4UIPower::Redraw()
{
	if(!g_bAlive)
		return;

	int iX, iY;
	iX = ScreenWidth / 2 - m_rcPowerBackground.right / 2;
	iY = ScreenHeight - m_rcPowerBackground.bottom - 60;

	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	TEXD_SetRenderMode(kRenderTransTexture);
	DrawTexture(m_iPowerBackground, m_rcPowerBackground, iX, iY);

	float flFlashTime = gHUD.m_flTime - m_flFlashTime;

	if(2 * flFlashTime >= M_PI)
		m_flFlashTime = gHUD.m_flTime;

	m_iAlpha = 255.0f * sin(2 * flFlashTime);

	if(m_iAlpha > 255)
		m_iAlpha = 255;

	if(m_iAlpha < 0)
		m_iAlpha = 0;
	
	vgui::surface()->DrawSetColor(255, 255, 255, m_iAlpha);
	TEXD_SetRenderMode(kRenderTransTexture);
	DrawTexture(m_iPowerLeft2, m_rcPowerLeft2, iX, iY);
	

	for(int i=0;i<=3;i++)
	{
		vgui::surface()->DrawSetColor(255, 255, 255, 255);
		DrawTexture(m_iPowerPoint, m_rcPowerPoint, iX + 141 + i * 42, iY + 44);
	}

	iX += 108;
	iY += 46;

	int iPower = m_iPowerServer;
	
	if(m_iPowerServer < m_iPower[0] && m_flPowerEndRefreshTime[0] > gHUD.m_flTime)
	{
		iPower = m_iPower2[0] + (m_iPower[0] - m_iPower2[0]) * (m_flPowerEndRefreshTime[0] - gHUD.m_flTime) / (m_flPowerEndRefreshTime[0] - m_flPowerStartRefreshTime[0]);

		if (iPower < 0)
			iPower = 0;

		if(m_iPowerServer > m_iPower[1] && m_flPowerEndRefreshTime[1] > gHUD.m_flTime)
			m_rcPowerLine2.left = m_iPower[1] / 100.0f * 210;
		else
			m_rcPowerLine2.left = m_iPowerServer / 100.0f * 210;

		m_rcPowerLine2.right = iPower / 100.0f * 210;

		if(m_rcPowerLine2.right >= m_iPower[1] / 100.0f * 210)
		{
			TEXD_BindTexture(m_iPowerLine2);
			TEXD_DrawTexture(iX + m_rcPowerLine2.left, iY, 252, 16, &m_rcPowerLine2);
		}
	}

	if(m_iPowerServer > m_iPower[1] && m_flPowerEndRefreshTime[1] > gHUD.m_flTime)
	{
		iPower = m_iPower2[1] - (m_iPower2[1] - m_iPower[1]) * (m_flPowerEndRefreshTime[1] - gHUD.m_flTime) / (m_flPowerEndRefreshTime[1] - m_flPowerStartRefreshTime[1]);

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

	if(iPower < 10)
		iX += 6;

	if(iPower == 100)
		iX -= 10;

	DrawNumbers(iPower, iX, iY, false, 1.1);

#if 0
	iX = 10;
	iY = ScreenHeight - 95;
#else
	iX = ScreenWidth / 2 - m_rcPowerBackground.right / 2 + 95;
	iY = ScreenHeight - m_rcPowerBackground.bottom - 60 - 15;
#endif
	if(g_iTeam == 1 && m_iPowerServer >= 30)
	{
		if(!m_iTextFlash)
		{
			m_iTextFlash = 1;
			m_flTextFlashStart = gHUD.m_flTime;
		}

		float flFlashTime = gHUD.m_flTime - m_flTextFlashStart;		

		int iAlpha = abs(sin(flFlashTime * M_PI / 1.5)) * 255 * 0.9 + 25;
		iAlpha = iAlpha>255?255:iAlpha;
		iAlpha = iAlpha<0?0:iAlpha;

		if(flFlashTime >= 3.75)		
			iAlpha = 255;
		
		vgui::surface()->DrawSetColor(255, 255, 255, iAlpha);
		TEXD_SetRenderMode(kRenderTransTexture);
		DrawTexture(m_iKey[0], m_rcKey[0], iX, iY);

		iX += m_rcKey[0].right + 5;

		g_Font.SetColor(251, 201, 96, iAlpha);
		g_Font.SetWidth(20);
		g_Font.DrawString(m_szDrawText, iX, iY + 30, 200);
	}
	else if(g_iTeam == 2)
	{
		m_iTextFlash = 0;
		m_flTextFlashStart = 0.0f;


		if(m_iUseTimesOld != int(m_iPowerServer / m_iSkillUsage))
		{
			m_iUseTimesOld = m_iPowerServer / m_iSkillUsage;
			
			wchar_t szBuf[3][256];		

			wchar_t *pBuf[2];
			pBuf[0] = vgui::localize()->Find("CSBTE_ZB4_Skill_Format");

			switch (m_iText)
			{
			case 0: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Normal");break;
			case 1: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Speed");break;
			case 2: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Heavy");break;
			case 3: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Hidden");break;
			case 4: pBuf[1] = vgui::localize()->Find("CSBTE_ZB4_Skill_Humpback");break;
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
	if(power < m_iPowerServer)
	{
		if(m_iPowerServer < m_iPower[0] && m_flPowerEndRefreshTime[0] > gHUD.m_flTime)
		{
			m_iPower2[0] = power;
			m_flPowerEndRefreshTime[0] = gHUD.m_flTime + flashtime;
		}
		else
		{
			m_iPower[0] = m_iPowerServer;
			m_iPower2[0] = power;
			m_flPowerEndRefreshTime[0] = gHUD.m_flTime + flashtime;
			m_flPowerStartRefreshTime[0] = gHUD.m_flTime;
			m_iAlpha = 0;
		}
		
	}

	if(power > m_iPowerServer)
	{
		if(m_iPowerServer > m_iPower[1] && m_flPowerEndRefreshTime[1] > gHUD.m_flTime)
		{
			m_iPower2[1] = power;
			m_flPowerEndRefreshTime[1] = gHUD.m_flTime + flashtime;
		}
		else
		{
			m_iPower[1] = m_iPowerServer;
			m_iPower2[1] = power;
			m_flPowerEndRefreshTime[1] = gHUD.m_flTime + flashtime;
			m_flPowerStartRefreshTime[1] = gHUD.m_flTime;
		}		
	}

	m_iPowerServer = power;

}