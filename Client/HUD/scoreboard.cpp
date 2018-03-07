#include "base.h"
#include "exportfuncs.h"
#include "hud.h"
#include "configs.h"

#include "scoreboard.h"

static CHudScoreBoard g_HudScoreBoard;
CHudScoreBoard &HudScoreBoard()
{
	return g_HudScoreBoard;
}

int CHudScoreBoard::DrawHudsNumber(int x, int y, int iNumber, int r, int g, int b,int length)
{
	int k;
	if (length>3)
	{
		k = 0;
		if(iNumber >= 1000) k = (iNumber % 10000)/1000;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_SBHUD_snumber_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_SBHUD_snumber_0 + k));
		x += m_iSBsFontWidth;
	}

	if (length>2)
	{
		k = 0;
		if(iNumber >= 100) k = (iNumber % 1000)/100;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_SBHUD_snumber_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_SBHUD_snumber_0 + k));
		x += m_iSBsFontWidth;
		
	}

	if (length>1)
	{
		k = 0;
		if(iNumber >= 10) k = (iNumber % 100)/10;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_SBHUD_snumber_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_SBHUD_snumber_0 + k));
		x += m_iSBsFontWidth;
	}

	k = iNumber % 10;
	gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_SBHUD_snumber_0 + k), r, g, b);
	gEngfuncs.pfnSPR_DrawAdditive(0,  x, y, &Hud().GetSpriteRect(m_SBHUD_snumber_0 + k));
	x += m_iSBsFontWidth;

	return x;
}
int CHudScoreBoard::DrawHudNumber(int x, int y, int iNumber, int r, int g, int b,int length)
{
	int k;
	if (length>3)
	{
		k = 0; 
		if(iNumber>=1000) k = (iNumber % 10000)/1000;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_SBHUD_number_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_SBHUD_number_0 + k));
		x += m_iSBFontWidth;
	}

	if (length>2)
	{
		k = 0; 
		if(iNumber>=100) k = (iNumber % 1000)/100;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_SBHUD_number_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_SBHUD_number_0 + k));
		x += m_iSBFontWidth;
	}

	if (length>1)
	{
		k = 0; 
		if(iNumber>=10) k = (iNumber % 100)/10;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_SBHUD_number_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_SBHUD_number_0 + k));
		x += m_iSBFontWidth;
	}

	k = iNumber % 10;
	gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_SBHUD_number_0 + k), r, g, b);
	gEngfuncs.pfnSPR_DrawAdditive(0,  x, y, &Hud().GetSpriteRect(m_SBHUD_number_0 + k));
	x += m_iSBFontWidth;

	return x;
}
void CHudScoreBoard::Init()
{
	m_iFlags |= HUD_ACTIVE;
}
void CHudScoreBoard::VidInit()
{
	m_iLeft = 0;
	m_iCenter = 0;
	m_iRight = 0;
	m_iLeft2 = 0;
	m_iRight2 = 0;
	m_iMode = -1;

	m_SBHUD_number_0 = Hud().GetSpriteIndex("SBNum_0");
	m_iBgNone = Hud().GetSpriteIndex("SBOriginalBG");
	m_iBgTd = Hud().GetSpriteIndex("SBTeamDeathBG");
	m_iTextCT = Hud().GetSpriteIndex("SBText_CT");
	m_iTextTR = Hud().GetSpriteIndex("SBText_T");
	m_iTextRound = Hud().GetSpriteIndex("SBText_Round");
	m_iTextKill = Hud().GetSpriteIndex("SBText_Kill");
	m_iTextHM = Hud().GetSpriteIndex("SBText_HM");
	m_iTextZB = Hud().GetSpriteIndex("SBText_ZB");
	m_iText1st = Hud().GetSpriteIndex("SBText_1st");

	m_iSBFontWidth = Hud().GetSpriteRect(m_SBHUD_number_0).right - Hud().GetSpriteRect(m_SBHUD_number_0).left;
	m_iSBFontHeight = Hud().GetSpriteRect(m_SBHUD_number_0).bottom - Hud().GetSpriteRect(m_SBHUD_number_0).top;

	m_SBHUD_snumber_0 = Hud().GetSpriteIndex("SBSNum_0");
	m_iSBsFontWidth = Hud().GetSpriteRect(m_SBHUD_snumber_0).bottom - Hud().GetSpriteRect(m_SBHUD_snumber_0).top;
	m_iSBsFontHeight = Hud().GetSpriteRect(m_SBHUD_snumber_0).bottom - Hud().GetSpriteRect(m_SBHUD_snumber_0).top;
}
void CHudScoreBoard::Draw(float time)
{
	if (!mh_scoreboard->value)
		return;

	if (g_iMod == MOD_ZB4)
		return;

	m_iLeft2 = 0;
	m_iRight2 = 0;

	for (int i = 1; i<33; i++)
	{
		if (vPlayer[i].team == 2 && vPlayer[i].alive) m_iLeft2 += 1;
		if (vPlayer[i].team == 1 && vPlayer[i].alive) m_iRight2 += 1;
	}

	if (g_iMod == MOD_TDM || g_iMod == MOD_GD)
	{
		m_iLeft = 0;
		m_iRight = 0;

		for (int i = 1; i<33; i++)
		{
			if (vPlayer[i].team == 2)
				m_iLeft += g_PlayerExtraInfo[i].frags;
			else if (vPlayer[i].team == 1)
				m_iRight += g_PlayerExtraInfo[i].frags;
		}
	}
	else if (g_iMod == MOD_DM)
	{
		m_iLeft = 0;

		for (int i = 1; i<33; i++)
		{
			if (g_PlayerExtraInfo[i].frags > m_iLeft)
				m_iLeft = g_PlayerExtraInfo[i].frags;
		}

		int iLocal = gEngfuncs.GetLocalPlayer()->index;

		m_iRight = g_PlayerExtraInfo[iLocal].frags;
	}



	if (gConfigs.bEnableNewHud)
		return;

	if(g_iMod == MOD_NONE || g_iMod == MOD_GHOST)
	{
		//========None Mod========
		//=====Bg
		int x,y;
		int iWidth = Hud().GetSpriteRect(m_iBgNone).right -Hud().GetSpriteRect(m_iBgNone).left ;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iBgNone), 225, 225, 225);
		x = (ScreenWidth - iWidth) / 2;
		gEngfuncs.pfnSPR_DrawHoles(0, x, 3, &Hud().GetSpriteRect(m_iBgNone));

		//=====T,CT icon
		x += 65;
		y = 14;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextTR), 140,140,140);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextTR));
		x += 70;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextCT), 140,140,140);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextCT));
		//=====Draw Num Up 
		y -= 1;
		//Up Left
		x -= 115;
		DrawHudNumber(x, y,m_iLeft, 120, 120, 120, 2);
		//Up Right
		x += 145;
		DrawHudNumber(x, y, m_iRight, 120, 120, 120, 2);
		//Center
		x -=70;
		DrawHudNumber(x, y, m_iCenter, 120, 120, 120, 2);
		//=====Round Icon
		x -= 6;
		y += 20;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextRound), 120,120,120);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextRound));
		//=====Draw Num Down
		//Down Left
		x -= 50;
		DrawHudsNumber(x, y, m_iLeft2, 120, 120, 120, 2);	
		//Down Right
		x += 116;
		DrawHudsNumber(x, y,m_iRight2, 120, 120, 120, 2);

	}
	else if(g_iMod == MOD_TDM)
	{
		//========None Mod========
		//=====Bg
		int x,y;
		int iWidth = Hud().GetSpriteRect(m_iBgTd).right -Hud().GetSpriteRect(m_iBgTd).left ;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iBgTd), 225, 225, 225);
		x = (ScreenWidth - iWidth) / 2;
		gEngfuncs.pfnSPR_DrawHoles(0, x, 3, &Hud().GetSpriteRect(m_iBgTd));
		//=====T,CT icon
		x += 65;
		y = 14;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextTR), 140,140,140);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextTR));
		x += 70;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextCT), 140,140,140);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextCT));
		//=====Draw Num Up
		y -= 1;
		//Up Left
		x -= 120; // 120
		DrawHudNumber(x, y,m_iLeft, 120, 120, 120, 3);
		//Up Right
		x += 145;
		DrawHudNumber(x, y, m_iRight, 120, 120, 120, 3);
		//Center
		x -=74; //74
		DrawHudNumber(x, y, m_iCenter, 120, 120, 120, 3); // 120X3,3
		//=====Kill Icon
		x += 3; //3
		y += 20; //20
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextKill), 120,120,120);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextKill));
	}
	else if(g_iMod == MOD_DM)
	{
		//=====Bg
		int x,y;
		int iWidth = Hud().GetSpriteRect(m_iBgTd).right -Hud().GetSpriteRect(m_iBgTd).left ;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iBgTd), 225, 225, 225);
		x = (ScreenWidth - iWidth) / 2;
		gEngfuncs.pfnSPR_DrawHoles(0, x, 3, &Hud().GetSpriteRect(m_iBgTd));
		//=====T,CT icon
		x += 50;
		y = 14;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iText1st), 140,140,140);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iText1st));
		x += 85;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextKill), 140,140,140);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextKill));
		//=====Draw Num Up
		y -= 1;
		//Up Left
		x -= 118; // 120

		int iLocal =gEngfuncs.GetLocalPlayer()->index;
		m_iRight = g_PlayerExtraInfo[iLocal].frags;

		m_iLeft = 0;

		for(int i=1;i<33;i++)
		{
			if(g_PlayerExtraInfo[i].frags > m_iLeft)
				m_iLeft = g_PlayerExtraInfo[i].frags;
		}
		DrawHudNumber(x, y, m_iLeft, 120, 120, 120, 2);
		//Up Right
		x += 155;
		DrawHudNumber(x, y, m_iRight, 120, 120, 120, 2);
		//Center
		x -=77;
		DrawHudNumber(x, y, m_iCenter, 120, 120, 120, 2);
		//=====Kill Icon
		x -= 5;
		y += 20; //20
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextKill), 120,120,120);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextKill));
	}
	else if (g_iMod == MOD_ZE || g_iMod == MOD_ZB || g_iMod == MOD_ZB2 || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4 || g_iMod == MOD_ZB5 || g_iMod == MOD_ZSE)
	{
		//========None Mod========
		//=====Bg
		int x,y;
		int iWidth = Hud().GetSpriteRect(m_iBgNone).right -Hud().GetSpriteRect(m_iBgNone).left ;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iBgNone), 225, 225, 225);
		x = (ScreenWidth - iWidth) / 2;
		gEngfuncs.pfnSPR_DrawHoles(0, x, 3, &Hud().GetSpriteRect(m_iBgNone));

		//=====T,CT icon
		x += 58;
		y = 14;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextZB), 140,140,140);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextZB));
		x += 7;
		x += 70;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextHM), 140,140,140);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextHM));
		
		//=====Draw Num Up
		y -= 1;
		//Up Left
		x -= 115;
		DrawHudNumber(x, y,m_iLeft, 120, 120, 120, 2);
		//Up Right
		x += 145;
		DrawHudNumber(x, y, m_iRight, 120, 120, 120, 2);
		//Center
		x -=70;
		DrawHudNumber(x, y, m_iCenter, 120, 120, 120, 2);
		//=====Round Icon
		x -= 6;
		y += 20;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iTextRound), 120,120,120);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iTextRound));
		//=====Draw Num Down
		//Down Left
		x -= 50;
		DrawHudsNumber(x, y, m_iLeft2, 120, 120, 120, 2);	
		//Down Right
		x += 116;
		DrawHudsNumber(x, y,m_iRight2, 120, 120, 120, 2);
	}
}