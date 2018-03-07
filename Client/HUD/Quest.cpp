#include "base.h"
#include "hud.h"
#include "DrawTargaImage.h"
#include "Fonts.h"
#include "util.h"

#include "quest.h"

static CHudQuest g_HudQuest;
CHudQuest &HudQuest()
{
	return g_HudQuest;
}

void CHudQuest::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}
void CHudQuest::VidInit(void)
{
	m_iBG = Hud().m_TGA.FindTexture("resource\\hud\\quest\\quest_bg");
	m_iNPC = Hud().m_TGA.FindTexture("resource\\hud\\quest\\quest_npc1");
	m_iComplete = Hud().m_TGA.FindTexture("resource\\hud\\quest\\quest_complete");
}
void CHudQuest::Add(char *pNpcName, char *pNpcPic, char *pMission)
{
	m_iStartPos = 0- g_Texture[m_iBG].iWidth - 5;
	m_bDraw = 1;
	memset(m_szNPCName,0,sizeof(m_szNPCName));
	memset(m_szMission,0,sizeof(m_szMission));
	sprintf(m_szNPCName, "%s", pNpcName);
	sprintf(m_szMission, "%s", pMission);
	m_iNPC = Hud().m_TGA.FindTexture(pNpcPic);
}
void CHudQuest::Draw(float time)
{
	if(!m_bDraw) return;
	g_Font.SetWidth(16);
	
	static int iX,iY,iAlpha;
	iX = m_iStartPos;
	iY = ScreenHeight * 0.75 - g_Texture[m_iBG].iHeight*0.75;
	iAlpha = 255;
	if(m_iStartPos > 500)
	{
		if(m_iStartPos < 755)
		{
			int iDec = m_iStartPos - 500;
			iY += iDec;
			iAlpha = 255 - iDec;
		}
		else 
		{
			m_bDraw = 0;
			return;
		}
	}
	if(iX > 16) iX = 16;
	//iX = 15;
	g_Font.SetColor(255,255,255,iAlpha);
	GL_DrawTGA(g_Texture[m_iBG].iTexture,255,255,255,iAlpha,iX,iY,1.0);
	iY = iY + g_Texture[m_iBG].iHeight - g_Texture[m_iNPC].iHeight;
	GL_DrawTGA(g_Texture[m_iNPC].iTexture,255,255,255,iAlpha,iX,iY,1.0);
	int iY2 =  iY + g_Texture[m_iNPC].iHeight - 5;
	g_Font.DrawString(GetLangUni(m_szNPCName),iX+3,iY2,1500);
	g_Font.DrawString(GetLangUni(m_szMission),iX+140,iY2-65,1000);
	GL_DrawTGA(g_Texture[m_iComplete].iTexture,255,255,255,iAlpha,g_Texture[m_iBG].iWidth + iX - 20 - g_Texture[m_iComplete].iWidth,iY + 80,1.0);
	m_iStartPos += 3;
}