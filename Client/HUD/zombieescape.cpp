#include "base.h"
#include "hud.h"
#include "exportfuncs.h"
#include "zombieescape.h"
#include "DrawTargaImage.h"
#include "Fonts.h"
#include "util.h"

#include "DrawTabPanel.h"
#include "DrawTabPanel.h"

static CHudZombieEscape g_HudZombieEscape;
CHudZombieEscape &HudZombieEscape()
{
	return g_HudZombieEscape;
}

void CHudZombieEscape::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}

void CHudZombieEscape::VidInit(void)
{
	m_iHM = Hud().m_TGA.FindTexture("resource\\hud\\zombieescape\\ze_hm");
	m_iZB = Hud().m_TGA.FindTexture("resource\\hud\\zombieescape\\ze_zb");
	m_iFlag = Hud().m_TGA.FindTexture("resource\\hud\\zombieescape\\ze_flag");
	m_iSelf = Hud().m_TGA.FindTexture("resource\\hud\\zombieescape\\ze_self");
	m_iBar = Hud().m_TGA.FindTexture("resource\\hud\\zombieescape\\ze_bar");
}

void CHudZombieEscape::ResetUI(void)
{
	memset(m_iPlayerLastCount,0,sizeof(m_iPlayerLastCount));
	memset(m_iPoint,0,sizeof(m_iPoint));
	m_flZECheck = 0;
}
int CHudZombieEscape::LoadPoint(char *pName)
{
	char szFile[128];
	sprintf(szFile,"metahook\\zepoint\\%s.point",pName);
	
	FILE* pTgaList;
	static char szBuff[512];
	szBuff[0] = '\0';
	pTgaList = fopen(szFile, "rt");
	if(pTgaList == NULL)									
	{
		LogToFile( "Open Ze Point Failed[FileName:%s]",szFile);
		return 0;
	}
	int id = 0;

	while(!feof(pTgaList))
	{
		fgets(szBuff,sizeof(szBuff)-1,pTgaList);
		if(szBuff[strlen(szBuff)-1] =='\n') szBuff[strlen(szBuff)-1] = '\0';

		if(szBuff[0]==';'||!szBuff[0]) continue;

		static char szToken[1024];

		char *pText = gEngfuncs.COM_ParseFile(szBuff,szToken);
		m_flZEPoint[id][0] = (float)atof(szToken);
		char *pText2 = gEngfuncs.COM_ParseFile(pText,szToken);
		m_flZEPoint[id][1] = (float)atof(szToken);
		char *pText3 = gEngfuncs.COM_ParseFile(pText2,szToken);
		m_flZEPoint[id][2] = (float)atof(szToken);
		id++;
	}
	fclose(pTgaList);
	return 1;
}
void CHudZombieEscape::Draw(float time)
{
	// Check Mod
	if(g_iMod != MOD_ZE) return;

	// First Make Start Pos
	int iX,iY;
	iY = 100;
	iX = (ScreenWidth - g_Texture[m_iBar].iWidth)/2;

	// Draw Bar & Flag
	GL_DrawTGA(g_Texture[m_iBar].iTexture,255,255,255,255,iX,iY,1);

	int iTempX = iX;
	int iTempY = iY;
	iTempX = iX + g_Texture[m_iBar].iWidth - 53;
	iTempY -=26 ;
	

	GL_DrawTGA(g_Texture[m_iFlag].iTexture,255,255,255,255,iTempX,iTempY,1);
		
	gEngfuncs.pTriAPI->End();

	static int m_iPoint[20][3];
	static float fNearst = 9999.0f;
	static int iLocalPos;
	int iIndex = gEngfuncs.GetLocalPlayer()->index;
	

	if(m_flZECheck < Hud().m_flTime)
	{
		memset(m_iPoint,0,sizeof(m_iPoint));
		m_flZECheck = Hud().m_flTime + 0.1f;
		//
		for ( int i = 1; i < MAX_PLAYERS; i++ )
		{
			gEngfuncs.pfnGetPlayerInfo( i, &g_PlayerInfoList[i] );
		
			if(g_PlayerInfoList[i].name == NULL) 
			{
				iValidPlayer[i] = 0;
				g_PlayerExtraInfo[i].frags = 0;
				continue;
			}
			iValidPlayer[i] = 1;
		}
		// Make Info
		for(int i =1;i<33;i++)
		{
			fNearst = 9999.0f;
			if(iValidPlayer[i] && !(g_PlayerExtraInfo[i].iFlag & SCOREATTRIB_DEAD))
			{

				static Vector vOrigin;
				int iPoint;
				vOrigin = gEngfuncs.GetEntityByIndex(i)->origin;

				for(int k = 0;k<20;k++)
				{
					static Vector iTemp2;
					iTemp2[0] = vOrigin[0] - m_flZEPoint[k][0];
					iTemp2[1] = vOrigin[1] - m_flZEPoint[k][1];
					iTemp2[2] = vOrigin[2] - m_flZEPoint[k][2];
					float fDis = iTemp2.Length();
					if(fDis < fNearst)
					{
						fNearst = fDis;
						iPoint = k;
					}
				}
				if( (m_iPlayerLastCount[i]-iPoint)>1 || (m_iPlayerLastCount[i]-iPoint)<-1)
				{
					iPoint = m_iPlayerLastCount[i];
				}
				else m_iPlayerLastCount[i] = iPoint;
				// Check Team
				if(vPlayer[i].team == 1)
				{
					m_iPoint[iPoint][1]++;
				}
				else if(vPlayer[i].team == 2)
				{
					m_iPoint[iPoint][2]++;
				}
				if(iIndex == i) iLocalPos = iPoint;
			}
		}
	}

	for(int j =0 ;j<20;j++)
	{
		if(m_iPoint[j][1])
		{
			// Human
			iTempX = iX + 53;
			iTempY = iY - 18 ;

			iTempX = iTempX + 21 * j;

			GL_DrawTGA(g_Texture[m_iHM].iTexture,255,255,255,255,iTempX,iTempY,1);

			char p[3];
			sprintf(p,"%d",m_iPoint[j][1]);
			iTempX += 16;
			iTempY += 24;

			if(m_iPoint[j][1]>9)
			{
				iTempX -= 3;
			}
			g_Font.SetColor(255,255,255,255);
			g_Font.SetWidth(14);
			g_Font.DrawString(UTF8ToUnicode(p),iTempX, iTempY,100);
		}
		if(m_iPoint[j][2])
		{
			// Zombie
			iTempX = iX + 53;
			iTempY = 20 +iY ;

			iTempX = iTempX + 21 * j;

			GL_DrawTGA(g_Texture[m_iZB].iTexture,255,255,255,255,iTempX,iTempY,1);
			char p[3];
			sprintf(p,"%d",m_iPoint[j][2]);
			iTempX += 16;
			iTempY += 24;

			if(m_iPoint[j][2]>9)
			{
				iTempX -= 3;
			}
			g_Font.SetColor(255,255,255,255);
			g_Font.SetWidth(14);
			g_Font.DrawString(UTF8ToUnicode(p),iTempX, iTempY,100);
		}
	}
	// Draw Me
	
	if(iValidPlayer[iIndex] && !(g_PlayerExtraInfo[iIndex].iFlag & SCOREATTRIB_DEAD) && vPlayer[iIndex].team == 2)
	{
		iTempX = iX + 53;
		iTempY = 20 +iY ;
		iTempX = iTempX + 21 * iLocalPos;


		GL_DrawTGA(g_Texture[m_iSelf].iTexture,255,255,255,255,iTempX,iTempY,1);
	}
	if(iValidPlayer[iIndex] && !(g_PlayerExtraInfo[iIndex].iFlag & SCOREATTRIB_DEAD) && vPlayer[iIndex].team == 1)
	{
		iTempX = iX + 53;
		iTempY = iY - 18 ;

		iTempX = iTempX + 21 * iLocalPos;

		GL_DrawTGA(g_Texture[m_iSelf].iTexture,255,255,255,255,iTempX,iTempY,1);
	}
	return;
}