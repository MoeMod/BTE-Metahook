#include "base.h"
#include "BaseUI.h"
#include "configs.h"

#define DISABLE_LIGHT_UI

void CHudZB4UIScoreBoard::Init()
{
	int i;
	i = gHUD.m_TGA.FindTexture("resource\\hud\\zombie\\z4_scoreboard");
	m_iScroreBorad = g_Texture[i].iTexture;
	m_rcScroreBorad.right = g_Texture[i].iWidth;
	m_rcScroreBorad.bottom = g_Texture[i].iHeight;

	i = gHUD.m_TGA.FindTexture("resource\\hud\\zombie\\z4_scorebg");
	m_iDamageBorad = g_Texture[i].iTexture;
	m_rcDamageBorad.right = g_Texture[i].iWidth;
	m_rcDamageBorad.bottom = g_Texture[i].iHeight;

	i = gHUD.m_TGA.FindTexture("resource\\hud\\zombie\\z4_scorehuman");
	m_iTeamIcon[0] = g_Texture[i].iTexture;
	m_rcTeamIcon[0].right = g_Texture[i].iWidth;
	m_rcTeamIcon[0].bottom = g_Texture[i].iHeight;

	i = gHUD.m_TGA.FindTexture("resource\\hud\\zombie\\z4_scorezombie");
	m_iTeamIcon[1] = g_Texture[i].iTexture;
	m_rcTeamIcon[1].right = g_Texture[i].iWidth;
	m_rcTeamIcon[1].bottom = g_Texture[i].iHeight;

	i = gHUD.m_TGA.FindTexture("resource\\hud\\zombie\\z4number");
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
	DrawNumbers(gHUD.m_ScoreBoard.m_iLeft, iX, iY, true, 0.9);

	iX = ScreenWidth / 2 + 67;
	iY = 7;
	DrawNumbers(gHUD.m_ScoreBoard.m_iRight, iX, iY, false, 0.9);

	TEXD_SetColor(200, 200, 200, 255);

	gHUD.m_ScoreBoard.m_iLeft2 = 0;
	gHUD.m_ScoreBoard.m_iRight2 = 0;

	for(int i=1;i<33;i++)
	{
		if(vPlayer[i].team == 2 && vPlayer[i].alive) gHUD.m_ScoreBoard.m_iLeft2 += 1;
		if(vPlayer[i].team == 1 && vPlayer[i].alive) gHUD.m_ScoreBoard.m_iRight2 += 1;		
	}

	iX = ScreenWidth / 2 - 115;
	iY = 34;
	DrawNumbers(gHUD.m_ScoreBoard.m_iLeft2, iX, iY, true, 0.65);

	iX = ScreenWidth / 2 + 63;
	iY = 34;
	DrawNumbers(gHUD.m_ScoreBoard.m_iRight2, iX, iY, false, 0.65);
}

void CHudZB4UIScoreBoard::DrawScore()
{
	if(m_iTeam == 0 || m_iTeam == 3)
		return;
	
	if(g_szCurWeapon2 && g_bAlive)
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

