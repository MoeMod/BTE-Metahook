#include "base.h"
#include "hud.h"
#include "DrawTargaImage.h"
#include "BaseUI.h"
#include "Fonts.h"
#include "util.h"

#include "Statistics.h"

#define FADEIN_TIME 0.3
#define FADEOUT_TIME 0.2
#define Y_OFFSET 50
#define Y_OFFSET_DIE 85

//DECLARE_COMMAND(m_Statistics, StatisticsOpen)

static CHudStatistics g_HudStatistics;
CHudStatistics &HudStatistics()
{
	return g_HudStatistics;
}

enum statistics_anim
{
	ANIM_NONE,
	ANIM_FADEIN,
	ANIM_FADEOUT
};

void CHudStatistics::UserCmd_StatisticsOpen()
{
	if (!m_bCanDraw)
	{
		Open();
		gEngfuncs.pfnPlaySoundByName("sound/ui/menu_select.wav", 1.0);

		return;
	}

	if (m_bCanDraw && m_iAnimStatus == ANIM_NONE)
	{
		Close();
		gEngfuncs.pfnPlaySoundByName("sound/ui/menu_show.wav", 1.0);

		return;
	}
}

void CHudStatistics::Open()
{
	m_iX = ScreenWidth * 0.5 - m_iW * 0.5;
	//m_iY = ScreenHeight - m_iH - m_iY_Ofs;
	m_bCanDraw = true;
	m_flAnimTime = Hud().m_flTime + FADEIN_TIME;
	m_iAlpha = 0;
	m_iAnimStatus = ANIM_FADEIN;
}

void CHudStatistics::Close()
{
	m_flAnimTime = Hud().m_flTime + FADEOUT_TIME;
	m_flDie = 0.0f;
	m_iAnimStatus = ANIM_FADEOUT;
}

void CHudStatistics::Init()
{
	m_iFlags |= HUD_ACTIVE;
	
	//HOOK_COMMAND("statistics", StatisticsOpen);
	gEngfuncs.pfnAddCommand("statistics", [](){HudStatistics().UserCmd_StatisticsOpen(); });

	m_pKilledShow = gEngfuncs.pfnRegisterVariable("cl_statistics_killed_show", "1", FCVAR_ARCHIVE);
	m_pRoundEndShow = gEngfuncs.pfnRegisterVariable("cl_statistics_roundend_show", "1", FCVAR_ARCHIVE);
	
	Load();

	wcscpy(m_szTitle, GetLangUni("CSBTE_Statistics_Title"));
	//wcscpy(m_szTip, GetLangUni("CSBTE_Statistics_Close"));

	wcscpy(m_szKill[0], GetLangUni("CSBTE_Statistics_Kills"));
	//wcscpy(m_szDeath[0], GetLangUni("CSBTE_Statistics_Deaths"));
	wcscpy(m_szHs[0][0], GetLangUni("CSBTE_Statistics_HeadShot"));
	wcscpy(m_szHs[1][0], GetLangUni("CSBTE_Statistics_Knife"));
	wcscpy(m_szHs[2][0], GetLangUni("CSBTE_Statistics_Knife"));
	wcscpy(m_szShoots[0], GetLangUni("CSBTE_Statistics_Shoots"));
	//wcscpy(m_szHit[0], GetLangUni("CSBTE_Statistics_Hit"));

	for (int i = 0; i < 3; i++)
		m_iIcon[i] = vgui::surface()->CreateNewTextureID();
}

void CHudStatistics::VidInit()
{
	m_bCanDraw = false;
	
	/*m_iW = 200;
	m_iH = 283;
	m_iX = 5;
	m_iY = ScreenHeight * 0.5 - m_iH * 0.45;*/

	m_iW = 300;
	m_iH = 110;
	m_iX = ScreenWidth * 0.5 - m_iW * 0.5;
	//m_iY = ScreenHeight - m_iH - Y_OFFSET;

	m_iY_Ofs = Y_OFFSET;
	m_flDie = 0.0f;

	vgui::surface()->DrawSetTextureFile(m_iIcon[0], "resource\\hud\\combatinfo\\headshotkill", true, false);
	vgui::surface()->DrawSetTextureFile(m_iIcon[1], "resource\\hud\\combatinfo\\knifekill", true, false);
	vgui::surface()->DrawSetTextureFile(m_iIcon[2], "resource\\hud\\combatinfo\\hegrenadekill", true, false);
}

void CHudStatistics::Draw(float flTime)
{
	OnThink(flTime);
	
	if (!m_bCanDraw)
		return;

	GL_DrawFullPanel(g_UI_Panel, m_iX, m_iY, m_iW, m_iH, m_iAlpha);

	g_FontBold.SetWidth(16);
	g_FontBold.SetColor(255 * 0.3, 242 * 0.3, 98 * 0.3, m_iAlpha);
	g_FontBold.DrawString(m_szTitle, m_iX + 13, m_iY + 28, 1000);

	g_FontBold.SetColor(255, 242, 98, m_iAlpha);
	g_FontBold.DrawString(m_szTitle, m_iX + 11, m_iY + 26, 1000);

	//g_Font.SetColor(255, 242, 80, m_iAlpha);
	

	
	//g_Font.SetWidth(12);
	//g_Font.SetColor(200, 200, 200, m_iAlpha);
	//g_Font.DrawString(m_szTip, m_iX + 8/*m_iX + m_iW - 16 - g_Font.GetLen(m_szTip)*/, m_iY + m_iH - 20, 1000);

	g_Font.SetWidth(14);
	//g_Font.SetColor(255, 255, 255, m_iAlpha);
	g_Font.SetColor(230, 230, 230, m_iAlpha);
	g_Font.DrawString(m_szKill[1], m_iX + 14, m_iY + 48, 1000);
	g_Font.DrawString(m_szShoots[1], m_iX + 14, m_iY + 66, 1000);
	
	int w, h;
	vgui::surface()->DrawSetColor(255, 255, 255, m_iAlpha);
	vgui::surface()->DrawSetTexture(m_iIcon[0]);
	vgui::surface()->DrawGetTextureSize(m_iIcon[0], w, h);
	vgui::surface()->DrawTexturedRect(m_iX + 14, m_iY + 72, m_iX + 14 + w, m_iY + 72 + h);
	g_Font.DrawString(m_szHs[0][1], m_iX + 18 + w, m_iY + 72 + 13, 1000);

	vgui::surface()->DrawSetTexture(m_iIcon[1]);
	vgui::surface()->DrawGetTextureSize(m_iIcon[1], w, h);
	vgui::surface()->DrawTexturedRect(m_iX + 140, m_iY + 72, m_iX + 140 + w, m_iY + 72 + h);
	g_Font.DrawString(m_szHs[1][1], m_iX + 144 + w, m_iY + 72 + 13, 1000);

	vgui::surface()->DrawSetTexture(m_iIcon[2]);
	vgui::surface()->DrawGetTextureSize(m_iIcon[2], w, h);
	vgui::surface()->DrawTexturedRect(m_iX + 205, m_iY + 72, m_iX + 205 + w, m_iY + 72 + h);
	g_Font.DrawString(m_szHs[2][1], m_iX + 205 + w, m_iY + 72 + 13, 1000);

}

void CHudStatistics::OnThink(float flTime)
{
	if (m_iDeathSave != m_iDeath && m_iDeathSave != -1 && m_pKilledShow->value)
	{
		Open();
		m_flDie = flTime + 3.0f;
	}
	
	if (!m_bCanDraw)
		return;
	
	m_iY_Ofs = g_bAlive ? Y_OFFSET : Y_OFFSET_DIE;
	m_iY = ScreenHeight - m_iH - m_iY_Ofs;

	if (m_flDie && m_flDie < flTime)
		Close();

	if (m_iAnimStatus == ANIM_FADEIN)
	{
		if (m_flAnimTime > flTime)
		{
			m_iAlpha = 255 - 255.0f * (m_flAnimTime - flTime) / FADEIN_TIME;
			m_iY = ScreenHeight - m_iH * ((FADEOUT_TIME - (m_flAnimTime - flTime)) / FADEOUT_TIME * 0.1 + 0.9) - m_iY_Ofs;
		}
		else
		{
			m_iY = ScreenHeight - m_iH - m_iY_Ofs;
			m_iAlpha = 255;
			m_iAnimStatus = ANIM_NONE;
		}
	}

	if (m_iAnimStatus == ANIM_FADEOUT)
	{
		if (m_flAnimTime > flTime)
		{
			m_iAlpha = 255.0f * (m_flAnimTime - flTime) / FADEOUT_TIME;
			//m_iX = 5 - (1.0f - (m_flAnimTime - flTime) / FADEOUT_TIME) * m_iW * 1.3;
			m_iY = ScreenHeight - m_iH * ((m_flAnimTime - flTime) / FADEOUT_TIME * 0.5 + 0.5) - m_iY_Ofs;
		}
		else
		{
			m_bCanDraw = false;
			m_iAnimStatus = ANIM_NONE;
		}
	}
		
	if (m_iKillSave != m_iKill || m_iDeathSave != m_iDeath || m_iHsSave != m_iHs || m_iShootsSave != m_iShoots || m_iHitSave != m_iHit)
	{
		ResetText();

		m_iKillSave = m_iKill;
		m_iDeathSave = m_iDeath;
		m_iHsSave = m_iHs;
		m_iShootsSave = m_iShoots;
		m_iHitSave = m_iHit;
		m_iKnifeKillSave = m_iKnifeKill;
		m_iGrenadeKillSave = m_iGrenadeKill;
	}
}

void CHudStatistics::Load()
{
	m_iKill = 0;
	m_iDeath = 0;
	m_iHs = 0;
	m_iShoots = 0;
	m_iHit = 0;

	m_iKillSave = -1;
	m_iDeathSave = -1;
	m_iHsSave = -1;
	m_iShootsSave = -1;
	m_iHitSave = -1;

	m_bSave = false;

	char szText[16];
	GetPrivateProfileStringA("SAVE", "Enable", "0", szText, sizeof(szText), "cstrike/save.ini");
	if (!strcmp(szText, "1")) m_bSave = true;	

	if (!m_bSave)
		return;

	GetPrivateProfileStringA("SAVE", "Kill", "0", szText, sizeof(szText), "cstrike/save.ini");
	m_iKill = atoi(szText);
	GetPrivateProfileStringA("SAVE", "Death", "0", szText, sizeof(szText), "cstrike/save.ini");
	m_iDeath = atoi(szText);
	GetPrivateProfileStringA("SAVE", "HeadShot", "0", szText, sizeof(szText), "cstrike/save.ini");
	m_iHs = atoi(szText);
	GetPrivateProfileStringA("SAVE", "Shoot", "0", szText, sizeof(szText), "cstrike/save.ini");
	m_iShoots = atoi(szText);
	GetPrivateProfileStringA("SAVE", "Hit", "0", szText, sizeof(szText), "cstrike/save.ini");
	m_iHit = atoi(szText);
	GetPrivateProfileStringA("SAVE", "Knife", "0", szText, sizeof(szText), "cstrike/save.ini");
	m_iKnifeKill = atoi(szText);
	GetPrivateProfileStringA("SAVE", "Grenade", "0", szText, sizeof(szText), "cstrike/save.ini");
	m_iGrenadeKill = atoi(szText);
}

void CHudStatistics::Save()
{
	if (!m_bSave)
		return;
	
	char szText[16];

	sprintf(szText, "%d", m_iKill);
	WritePrivateProfileString("SAVE", "Kill", szText, "cstrike/save.ini");
	sprintf(szText, "%d", m_iDeath);
	WritePrivateProfileString("SAVE", "Death", szText, "cstrike/save.ini");
	sprintf(szText, "%d", m_iHs);
	WritePrivateProfileString("SAVE", "HeadShot", szText, "cstrike/save.ini");
	sprintf(szText, "%d", m_iShoots);
	WritePrivateProfileString("SAVE", "Shoot", szText, "cstrike/save.ini");
	sprintf(szText, "%d", m_iHit);
	WritePrivateProfileString("SAVE", "Hit", szText, "cstrike/save.ini");
	sprintf(szText, "%d", m_iKnifeKill);
	WritePrivateProfileString("SAVE", "Knife", szText, "cstrike/save.ini");
	sprintf(szText, "%d", m_iGrenadeKill);
	WritePrivateProfileString("SAVE", "Grenade", szText, "cstrike/save.ini");

}

void CHudStatistics::ResetText()
{
	wchar_t pBuf[3][64];

	swprintf(pBuf[0], L"%d", m_iKill);
	swprintf(pBuf[1], L"%d", m_iDeath);
	swprintf(pBuf[2], L"%.2f", m_iDeath ? (float)m_iKill / m_iDeath : m_iKill ? m_iKill : 0);
	vgui::localize()->ConstructString(m_szKill[1], 256, m_szKill[0], 3, pBuf[0], pBuf[1], pBuf[2]);

	swprintf(pBuf[0], L"%d", m_iHs);
	swprintf(pBuf[1], L"%.2f", m_iKill ? (float)m_iHs / m_iKill * 100.0f : 0);
	vgui::localize()->ConstructString(m_szHs[0][1], 256, m_szHs[0][0], 2, pBuf[0], pBuf[1]);

	swprintf(pBuf[0], L"%d", m_iKnifeKill);
	vgui::localize()->ConstructString(m_szHs[1][1], 256, m_szHs[1][0], 1, pBuf[0]);
	swprintf(pBuf[0], L"%d", m_iGrenadeKill);
	vgui::localize()->ConstructString(m_szHs[2][1], 256, m_szHs[2][0], 1, pBuf[0]);

	swprintf(pBuf[0], L"%d", m_iShoots);
	swprintf(pBuf[1], L"%d", m_iHit);
	swprintf(pBuf[2], L"%.2f", m_iShoots ? (float)m_iHit / m_iShoots * 100.0f : m_iHit ? 100.0f : 0);
	vgui::localize()->ConstructString(m_szShoots[1], 256, m_szShoots[0], 3, pBuf[0], pBuf[1], pBuf[2]);

	//vgui::localize()->ConstructString(m_szDeath[1], 256, m_szDeath[0], 1, pBuf[0]);
}

/*void CHudStatistics::ResetText()
{
	wchar_t pBuf[3][64];

	swprintf(pBuf[0], L"%d", m_iKill);
	swprintf(pBuf[1], L"%d", m_iDeath);
	swprintf(pBuf[2], L"%.2f", m_iDeath ? (float)m_iKill / m_iDeath : m_iKill ? 1 : 0);
	vgui::localize()->ConstructString(m_szKill[1], 256, m_szKill[0], 3, pBuf[0], pBuf[1], pBuf[2]);

	swprintf(pBuf[0], L"%d", m_iHs);
	swprintf(pBuf[1], L"%.2f", m_iKill ? (float)m_iHs / m_iKill * 100.0f : 0);
	vgui::localize()->ConstructString(m_szHs[1], 256, m_szHs[0], 2, pBuf[0], pBuf[1]);

	swprintf(pBuf[0], L"%d", m_iShoots);
	swprintf(pBuf[1], L"%d", m_iHit);
	swprintf(pBuf[2], L"%.2f", m_iShoots ? (float)m_iHit / m_iShoots * 100.0f : m_iHit ? 100.0f : 0);
	vgui::localize()->ConstructString(m_szShoots[1], 256, m_szShoots[0], 3, pBuf[0], pBuf[1], pBuf[2]);

	//vgui::localize()->ConstructString(m_szDeath[1], 256, m_szDeath[0], 1, pBuf[0]);
}*/