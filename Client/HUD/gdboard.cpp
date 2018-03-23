#include "base.h"
#include "exportfuncs.h"
#include "hud.h"
#include "TriAPI.h"
#include "Fonts.h"
#include "util.h"

#include "gdboard.h"
#include "scoreboard.h"
#include "zbsboard.h"
#include "Client/HUD/DrawTGA.h"

static CHudGunDeathBoard g_HudGunDeathBoard;
CHudGunDeathBoard &HudGunDeathBoard()
{
	return g_HudGunDeathBoard;
}

int g_iGD_Left  = 5, g_iGD_Center ,g_iGD_Right,g_iGB_LV;
int iGDNumberIndex[10];
int iRank[3];
int iGDBoardIndex;
float fScale = 0.16f;
float fScaleB = 1.0f;

int KILL_AIM[18] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 47, 49, 51, 54, 57, 60};

int GetLevel(int kills);
void DrawSelfNumbers(int n, int x, int y, float size = 1.0);

void CHudGunDeathBoard::VidInit(void)
{
	char TgaName[256];
	for(int i=0;i<10;i++)
	{
		sprintf(TgaName,"other\\number\\number%d",i);
		iGDNumberIndex[i] = Hud().m_TGA.GetTGA(TgaName);
	}
	iGDBoardIndex = Hud().m_TGA.GetTGA("mode\\gd\\gdboard");
	iRank[0] = Hud().m_TGA.GetTGA("mode\\gd\\gd1st");
	iRank[1] = Hud().m_TGA.GetTGA("mode\\gd\\gd2nd");
	iRank[2] = Hud().m_TGA.GetTGA("mode\\gd\\gd3rd");
	memset(m_iFrag,0,sizeof(m_iFrag));
}
void CHudGunDeathBoard::Init()
{
	m_iFlags |= HUD_ACTIVE;
	
	g_iGD_Left = 889;
	g_iGD_Center = 777;
	g_iGD_Right = 998;
	g_iGB_LV = 1;
	m_iY = 0;
	memset(m_iRank,0,sizeof(m_iRank));
	memset(m_iFrag,0,sizeof(m_iFrag));
}
void CHudGunDeathBoard::Draw(float time)
{
	//Draw Board
	if(g_iMod != MOD_GD) return;
	int iStartX = (ScreenWidth - g_MHTga[iGDBoardIndex].width*fScaleB)/2;
	int iStartY = 10;
	fScale = 0.16f;	

	Tri_Enable(GL_TEXTURE_2D);
	Tri_BindTexture(GL_TEXTURE_2D, g_MHTga[iGDBoardIndex].texid);
	gEngfuncs.pTriAPI->Color4f(1,1,1,1);
	Tri_Enable(GL_BLEND);
	Tri_Enable(GL_ALPHA_TEST);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	Tri_AlphaFunc(GL_GREATER, 0.0);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			
	gEngfuncs.pTriAPI->TexCoord2f(0,0);
	gEngfuncs.pTriAPI->Vertex3f(iStartX,iStartY,0);

	gEngfuncs.pTriAPI->TexCoord2f(1,0);
	gEngfuncs.pTriAPI->Vertex3f(iStartX+g_MHTga[iGDBoardIndex].width*fScaleB,iStartY,0);	
	gEngfuncs.pTriAPI->TexCoord2f(1,1);
	gEngfuncs.pTriAPI->Vertex3f(iStartX+g_MHTga[iGDBoardIndex].width*fScaleB,iStartY+g_MHTga[iGDBoardIndex].height*fScaleB,0);
		
	gEngfuncs.pTriAPI->TexCoord2f(0,1);
	gEngfuncs.pTriAPI->Vertex3f(iStartX,iStartY+g_MHTga[iGDBoardIndex].height*fScaleB,0);
	gEngfuncs.pTriAPI->End();

	// Draw Number Left

	int iNumberX = iStartX + 33;
	int iNumberY = iStartY + 13;
	int iTemp = 0;
	fScale = 0.15f;

	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	DrawSelfNumbers(HudScoreBoard().m_iLeft, iNumberX, iNumberY, 1.0);

	// Draw Number Right

	iNumberX = iStartX + 178;
	iNumberY = iStartY + 13;
	iTemp = 0;

	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	DrawSelfNumbers(HudScoreBoard().m_iRight, iNumberX, iNumberY, 1.0);

	// Draw Number Center

	iNumberX = iStartX + 112;
	iNumberY = iStartY + 27;
	iTemp = 0;
	fScale = 0.12f;

	gEngfuncs.pTriAPI->Color4ub(239, 222, 179, 255);
	DrawSelfNumbers(800/*HudScoreBoard().m_iCenter*/, iNumberX, iNumberY, 0.85);

	// Draw Player Level
	char p1[256];
	char *pP1 = p1;

	int kills = m_iFrag[gEngfuncs.GetLocalPlayer()->index];
	int level = GetLevel(kills) + 1;

	if(level >17)
	{
		sprintf(p1,"Lv.Max");
		wchar_t pSet[128];
		g_Font.C2W(pSet,127,pP1);
		g_Font.SetWidth(18);
		g_Font.SetColor(255,255,255,255);
		g_Font.DrawString(pSet,iStartX + 111, iStartY + 78,1000);
	}
	else
	{
		sprintf(p1,"Lv.%d",level);
		wchar_t pSet[128];
		g_Font.C2W(pSet,127,pP1);
		g_Font.SetWidth(18);
		g_Font.SetColor(255,255,255,255);

		if(level >= 10)
			g_Font.DrawString(pSet,iStartX + 115, iStartY + 78,1000);
		else
			g_Font.DrawString(pSet,iStartX + 119, iStartY + 78,1000);
	}
	

	// Draw Rank
	int iRankX = ScreenWidth - 20;
	iRankX -= g_MHTga[iRank[0]].width;
	int iRankY = iStartY + 15;

	for(int k=0;k<3;k++)
	{
		Tri_Enable(GL_TEXTURE_2D);
		gEngfuncs.pTriAPI->Color4f(1,1,1,1);
		Tri_Enable(GL_BLEND);
		Tri_Enable(GL_ALPHA_TEST);
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		Tri_AlphaFunc(GL_GREATER, 0.0);
		Tri_BindTexture(GL_TEXTURE_2D, g_MHTga[iRank[k]].texid);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			
		gEngfuncs.pTriAPI->TexCoord2f(0,0);
		gEngfuncs.pTriAPI->Vertex3f(iRankX,iRankY,0);

		gEngfuncs.pTriAPI->TexCoord2f(1,0);
		gEngfuncs.pTriAPI->Vertex3f(iRankX+g_MHTga[iRank[k]].width,iRankY,0);	
		gEngfuncs.pTriAPI->TexCoord2f(1,1);
		gEngfuncs.pTriAPI->Vertex3f(iRankX+g_MHTga[iRank[k]].width,iRankY+g_MHTga[iRank[k]].height,0);
	
		gEngfuncs.pTriAPI->TexCoord2f(0,1);
		gEngfuncs.pTriAPI->Vertex3f(iRankX,iRankY+g_MHTga[iRank[k]].height,0);
		gEngfuncs.pTriAPI->End();

		g_Font.SetWidth(14);

		if(m_iRank[k] && m_iFrag[m_iRank[k]])
		{
			hud_player_info_t hPlayer;
			gEngfuncs.pfnGetPlayerInfo(m_iRank[k],&hPlayer);

			int kills = m_iFrag[m_iRank[k]];
			int level = GetLevel(kills) + 1;

			if(level>17)
			{
				sprintf(p1,"Lv.Max %s",hPlayer.name);
			}
			else
			{
				sprintf(p1,"Lv.%d %s",level,hPlayer.name);
			}
			if(vPlayer[m_iRank[k]].team == 1)
			{
				g_Font.SetColor(162,213,255,255);
			}
			else
			{
				g_Font.SetColor(255,50,50,255);
			}
			
			g_Font.DrawString(UTF8ToUnicode(p1),iRankX + 63, iRankY+22,1000);
		}
		iRankY += g_MHTga[iRank[0]].height + 3;
		m_iY = iRankY;
	}
}

int GetLevel(int kills)
{
	for(int i=17;i>=0;i--)
	{
		if(kills >= KILL_AIM[i])
			return i;
	}
	return 0;
}

void DrawSelfNumbers(int n, int x, int y, float size)
{
	int width = 18 + 4;
	width *= size;

	int k;

	k = (n % 1000) / 100;
	HudZBSBoard().DrawSelfNumber(k, x, y, size);
	x += width;

	k = (n % 100) / 10;
	HudZBSBoard().DrawSelfNumber(k, x, y, size);
	x += width;

	k = n % 10;
	HudZBSBoard().DrawSelfNumber(k, x, y, size);
	x += width;
}
