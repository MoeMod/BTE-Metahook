#include "base.h"
#include "exportfuncs.h"
#include "hud.h"
#include "parsemsg.h"
#include "DrawTargaImage.h"
#include "TriAPI.h"
#include "Fonts.h"
#include "util.h"

#include "deathrun.h"
#include "Client/HUD/DrawTGA.h"

static CHudDeathRunUI g_HudDeathRunUI;
CHudDeathRunUI &HudDeathRunUI()
{
	return g_HudDeathRunUI;
}

void CHudDeathRunUI::InitUI(void)
{
	memset(m_iDRID,0,sizeof(m_iDRID));
	m_iStop = 1;
	m_iReset = 0;
	m_iRecord[0] = m_iRecord[1] = m_iRecord[2] = 0;
}
void CHudDeathRunUI::ResetUI(void)
{
	m_iReset = 1;
	m_iStop = 0;
}
void CHudDeathRunUI::ParseMessage(void)
{
	int iRank = READ_BYTE();
	int ID = READ_BYTE();
	iRank --;
	sprintf(m_szDRTime[iRank],"%s",READ_STRING());
	m_iDRID[iRank] = ID;

	if(!ID)
	{
		m_iDRID[iRank] = 0;
	}
	else
	{
		hud_player_info_t hInfo;
		gEngfuncs.pfnGetPlayerInfo(ID,&hInfo);
		memset(m_szDRName[iRank],0,sizeof(m_szDRName[iRank]));
		sprintf(m_szDRName[iRank],"%s",hInfo.name);
	}
	return;
}
void CHudDeathRunUI::Stop(void)
{
	m_iStop = 1;
	m_iReset = 0;
	m_iRecord[0] = m_iSeconds;
	m_iRecord[1] = m_iMinutes;
	m_iRecord[2] = m_iMilliSeconds;
}
void CHudDeathRunUI::Start(void)
{
	m_iStop = 0;
	m_iReset = 0;
	m_flStartTime = Hud().m_flTime;
}
void CHudDeathRunUI::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}
void CHudDeathRunUI::VidInit(void)
{
	m_iRank[0] = Hud().m_TGA.FindTexture("resource\\hud\\challenge\\1st");
	m_iRank[1] = Hud().m_TGA.FindTexture("resource\\hud\\challenge\\2nd");
	m_iRank[2] = Hud().m_TGA.FindTexture("resource\\hud\\challenge\\3rd");

	m_iMedalRank[0] = Hud().m_TGA.FindTexture("resource\\hud\\challenge\\medal_1st");
	m_iMedalRank[1] = Hud().m_TGA.FindTexture("resource\\hud\\challenge\\medal_2nd");
	m_iMedalRank[2] = Hud().m_TGA.FindTexture("resource\\hud\\challenge\\medal_3rd");

	m_iBoard = Hud().m_TGA.FindTexture("resource\\hud\\challenge\\timeboard");
	m_iNumber = Hud().m_TGA.GetTGA("mode\\dr\\timenumber");
	m_iColon = Hud().m_TGA.FindTexture("resource\\hud\\challenge\\timecolon");

	m_iNumberWidth = g_MHTga[m_iNumber].width / 10;
}

void CHudDeathRunUI::Draw(float time)
{
	if(g_iMod !=MOD_DR) return;
	int iX = (ScreenWidth - g_Texture[m_iBoard].iWidth)/2;
	GL_DrawTGA(g_Texture[m_iBoard].iTexture,255,255,255,255,iX,10,1);

	// Draw Time
	if(m_iReset)
	{
		m_iSeconds = m_iMinutes = m_iMilliSeconds = 0;
	}
	else if(m_iStop)
	{
		m_iSeconds = m_iRecord[0];
		m_iMinutes = m_iRecord[1];
		m_iMilliSeconds = m_iRecord[2];
	}
	else
	{
		m_flDelta = time - m_flStartTime;
		if(m_flDelta<0)
		{
			m_iSeconds = m_iMinutes = m_iMilliSeconds = 0;
		}
		else
		{
			m_iSeconds = unsigned(m_flDelta);
			m_iMinutes = m_iSeconds / 60;
			m_iSeconds = m_iSeconds - 60*m_iMinutes;
			m_iMilliSeconds = int((time - int(time)) * 100);
		}
	}
	int iStartX,iStartY;
	iStartX = iX+6;
	iStartY = 5;
	// Draw Minues
	DrawNumber(iStartX+10,iStartY+10,m_iMinutes);
	GL_DrawTGA(g_Texture[m_iColon].iTexture,255,255,255,255,iStartX+10+2*m_iNumberWidth-2,iStartY+20,1);
	GL_DrawTGA(g_Texture[m_iColon].iTexture,255,255,255,255,iStartX+10+4*m_iNumberWidth+2,iStartY+20,1);

	DrawNumber(iStartX+10+2*m_iNumberWidth+4,iStartY+10,m_iSeconds);
	DrawNumber(iStartX+10+4*m_iNumberWidth+8,iStartY+10,m_iMilliSeconds);

	// Draw Rank
	iStartX = ScreenWidth - g_Texture[m_iMedalRank[0]].iWidth;
	iStartY = ScreenHeight*0.3f;
	GL_DrawTGA(g_Texture[m_iMedalRank[0]].iTexture,255,255,255,255,iStartX,iStartY,1);
	iStartY += g_Texture[m_iMedalRank[0]].iHeight+20;
	GL_DrawTGA(g_Texture[m_iMedalRank[1]].iTexture,255,255,255,255,iStartX,iStartY,1);
	iStartY += g_Texture[m_iMedalRank[0]].iHeight+20;
	GL_DrawTGA(g_Texture[m_iMedalRank[2]].iTexture,255,255,255,255,iStartX,iStartY,1);

	//RANK1
	g_Font.SetWidth(18);
	g_Font.SetColor(255,255,255,255);
	iStartX = ScreenWidth - g_Texture[m_iMedalRank[0]].iWidth;
	iStartY = ScreenHeight*0.3f;
	iStartY += 28;
	for(int i=0;i<3;i++)
	{
		if(m_iDRID[i])
		{
			g_Font.DrawString(UTF8ToUnicode(m_szDRName[i]),iStartX+70,iStartY,1000, 1000);
			g_Font.DrawString(UTF8ToUnicode(m_szDRTime[i]),iStartX+70,iStartY+26,1000, 1000);
			iStartY += (g_Texture[m_iMedalRank[0]].iHeight+20);
		}
	}
}
void CHudDeathRunUI::DrawNumber(int x, int y, int Num)
{
	int iNum_10 = Num / 10;
	float fRect = float(iNum_10) / 10.0f;
	int iX = x;
	int iY = y;
	Tri_Enable(GL_TEXTURE_2D);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	

	Tri_BindTexture(GL_TEXTURE_2D, g_MHTga[m_iNumber].texid);


	gEngfuncs.pTriAPI->Color4ub(255,255,255,255);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);	
	gEngfuncs.pTriAPI->TexCoord2f(fRect,1);
	gEngfuncs.pTriAPI->Vertex3f(iX,iY+g_MHTga[m_iNumber].height,0);

	gEngfuncs.pTriAPI->TexCoord2f(fRect+0.1f,1);
	gEngfuncs.pTriAPI->Vertex3f(iX+g_MHTga[m_iNumber].width*0.1f,iY+g_MHTga[m_iNumber].height,0);
		
	gEngfuncs.pTriAPI->TexCoord2f(fRect+0.1f,0);
	gEngfuncs.pTriAPI->Vertex3f(iX+g_MHTga[m_iNumber].width*0.1f,iY,0);
		
	gEngfuncs.pTriAPI->TexCoord2f(fRect,0);
	gEngfuncs.pTriAPI->Vertex3f(iX,iY,0);
	gEngfuncs.pTriAPI->End();

	iX += m_iNumberWidth;
	int iNum_1 = Num - 10*iNum_10;
	fRect = float(iNum_1) / 10.0f;
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	
		
	gEngfuncs.pTriAPI->TexCoord2f(fRect,1);
	gEngfuncs.pTriAPI->Vertex3f(iX,iY+g_MHTga[m_iNumber].height,0);

	gEngfuncs.pTriAPI->TexCoord2f(fRect+0.1f,1);
	gEngfuncs.pTriAPI->Vertex3f(iX+g_MHTga[m_iNumber].width*0.1f,iY+g_MHTga[m_iNumber].height,0);
		
	gEngfuncs.pTriAPI->TexCoord2f(fRect+0.1f,0);
	gEngfuncs.pTriAPI->Vertex3f(iX+g_MHTga[m_iNumber].width*0.1f,iY,0);
		
	gEngfuncs.pTriAPI->TexCoord2f(fRect,0);
	gEngfuncs.pTriAPI->Vertex3f(iX,iY,0);
	gEngfuncs.pTriAPI->End();
}