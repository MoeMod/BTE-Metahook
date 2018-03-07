#include "base.h"
#include "hud.h"
#include "TextureManager.h"
#include "calcscreen.h"
#include "Fonts.h"

void CHudFollowIconElements::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}

void CHudFollowIconElements::VidInit(void)
{
	memset(m_Icon, 0, sizeof(m_Icon));
}

void CHudFollowIconElements::Add(DrawIconItem rgTempDrawIcon)
{
	if (rgTempDrawIcon.iChannel <= 0 || rgTempDrawIcon.iChannel >= DRAWICON_MAX)
	{
		for (int i = 1; i < DRAWICON_MAX; i++)
		{
			if (!m_Icon[i].iChannel)
			{
				m_Icon[i] = rgTempDrawIcon;
				break;
			}
		}
	}
	else
		m_Icon[rgTempDrawIcon.iChannel] = rgTempDrawIcon;
}

void CHudFollowIconElements::Draw(float time)
{
	//int iColor =(int)pCvar_TipIcon->value;
	if (!pCvar_TipIcon->value)
		return;
	
	for(int j = 1;j < (DRAWICON_MAX+1) ; j++)
	{
		Vector vPlayerPos ;
		VectorCopy(gEngfuncs.GetLocalPlayer()->origin,vPlayerPos);
		if(!m_Icon[j].iChannel) continue;
		if(!m_Icon[j].iToggle) continue;
		
		
		float vecScreen[2];

		vec3_t EntViewOrg(m_Icon[j].x, m_Icon[j].y, m_Icon[j].z);

		if(!m_Icon[j].iBox) m_Icon[j].hl_hSprites = Hud().GetSprite(m_Icon[j].iSprIndex);

		if (CalcScreen(EntViewOrg, vecScreen))
		{
/*
			if(m_Icon[j].iBox == 1)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, g_iMod==MOD_ZB?g_MHTga[g_Tga_SpBox].texid:g_MHTga[g_Tga_SpuBox].texid);
				glColor4f(1,1,1,1);
				glEnable(GL_BLEND);
				glEnable(GL_ALPHA_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

				gEngfuncs.pTriAPI->Begin(TRI_QUADS);
				gEngfuncs.pTriAPI->TexCoord2f(0,0);
				gEngfuncs.pTriAPI->Vertex3f(vecScreen[0],vecScreen[1]+g_MHTga[g_Tga_SpBox].height,0);
				gEngfuncs.pTriAPI->TexCoord2f(1,0);
				gEngfuncs.pTriAPI->Vertex3f(vecScreen[0]+g_MHTga[g_Tga_SpBox].width,vecScreen[1]+g_MHTga[g_Tga_SpBox].height,0);		
				gEngfuncs.pTriAPI->TexCoord2f(1,1);
				gEngfuncs.pTriAPI->Vertex3f(vecScreen[0]+g_MHTga[g_Tga_SpBox].width,vecScreen[1],0);		
				gEngfuncs.pTriAPI->TexCoord2f(0,1);
				gEngfuncs.pTriAPI->Vertex3f(vecScreen[0],vecScreen[1],0);
				gEngfuncs.pTriAPI->End();
				glDisable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
			}
			else*/ 

			static auto &BombA = TextureManager()["resource\\hud\\icon\\bombtargetA.tga"];
			static auto &BombB = TextureManager()["resource\\hud\\icon\\bombtargetB.tga"];

			if(m_Icon[j].iBox == 2) //BombA
				BombA.Draw(vecScreen[0], vecScreen[1]);
			else if(m_Icon[j].iBox == 3 ) //BombB
				BombB.Draw(vecScreen[0], vecScreen[1]);
			else
			{
				gEngfuncs.pfnSPR_Set(m_Icon[j].hl_hSprites,m_Icon[j].r,m_Icon[j].g,m_Icon[j].b);
				gEngfuncs.pfnSPR_DrawAdditive(0,vecScreen[0], vecScreen[1], &Hud().GetSpriteRect(m_Icon[j].iSprIndex));
			}
			if(m_Icon[j].iDistance)
			{
				
				VectorSubtract(vPlayerPos,EntViewOrg,vPlayerPos);

				float distance = sqrt(vPlayerPos.x*vPlayerPos.x+vPlayerPos.y*vPlayerPos.y+vPlayerPos.z*vPlayerPos.z)/40.0f;
				char p1[256];
				wchar_t p2[256];
				sprintf(p1,"[%dm]",(short int)distance);

				g_FontBold.C2W(p2, 255, p1);

				if(m_Icon[j].iChannel<9)
				{
					g_FontBold.SetColor(251, 201, 96, 255);
					g_FontBold.SetWidth(16);
					if(m_Icon[j].iBox == 2 || m_Icon[j].iBox == 3)
						g_FontBold.DrawString(p2, int(vecScreen[0] + BombA.w() / 2 - g_FontBold.GetLen(p2) / 2 + 1), vecScreen[1] + BombB.h() + 20, 1000);
				}
			}
		}
	}
}
