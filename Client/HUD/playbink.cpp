#include "base.h"
#include "hud.h"
#include "bink/bink.h"
#include "BaseUI.h"
#include "TriAPI.h"
#include "util.h"

#include "playbink.h"
static CHudBinkPlayer g_HudBinkPlayer;
CHudBinkPlayer &HudBinkPlayer()
{
	return g_HudBinkPlayer;
}

static int m_iFrame = 0;
int iBinkStop = 0;
int iBinkScopeStop = 0;

void CHudBinkPlayer::Init()
{
	if (m_iFlags & HUD_ACTIVE)
		m_iFlags &= ~HUD_ACTIVE;
	
	m_iBinkTextureID = vgui::surface()->CreateNewTextureID();
	m_pBinkMem = NULL;

	m_flStartTime = 0.0;
	m_iFrame = 1;
	m_hBink = NULL;
}

void CHudBinkPlayer::BinkInstance(char *pszName,int iStop,int iLoop)
{
	if(!pCvar_DrawBink || pCvar_DrawBink->value <1)
	{
		if (m_iFlags & HUD_ACTIVE)
			m_iFlags &= ~HUD_ACTIVE;
		LogToFile("CHudBinkPlayer::BinkInstance : Blocked by cvar",pszName);
		return;
	}

	if(!(m_iFlags & HUD_ACTIVE) && !iStop)
	{
		// Clear Mem
		if(m_pBinkMem)
		{
			free(m_pBinkMem);
			m_pBinkMem = NULL;
		}
		m_hBink = BinkOpen(pszName,135266304);
		if(!m_hBink)
		{
			gEngfuncs.Con_DPrintf("CHudBinkPlayer::BinkInstance£ºFailed to load %s",pszName);
			return;
		}
		m_flStartTime = 0;
		m_iFrame = 1;
		m_iFlags |= HUD_ACTIVE;
		m_bBinkLoop = iLoop;
		iBinkStop = iStop;
		// Alloc Memory
		int iPixelSize = 4 * m_hBink->Width * m_hBink->Height;
		m_pBinkMem = (BYTE *)malloc(iPixelSize);
		if (!m_pBinkMem)
			gEngfuncs.Con_DPrintf("CHudBinkPlayer::BinkInstance : Failed to alloc memory");
	}
	else
	{
		if (iStop)
			m_iFlags &= ~HUD_ACTIVE;
	}
}
void CHudBinkPlayer::SetPos(int X,int Y,int Center)
{
	m_iX = X;
	m_iY = Y;
	m_iCenter = Center;
}
void CHudBinkPlayer::SetColor(int R, int G, int B)
{
	m_iColor[0] = R;
	m_iColor[1] = G;
	m_iColor[2] = B;
}
void CHudBinkPlayer::Draw(float time)
{
	if(iBinkStop && m_bBinkLoop)
	{
		m_bBinkLoop = 0;
		m_iFlags &= ~HUD_ACTIVE;
		iBinkStop = 0;
		return;
	}

	if(time - m_flStartTime > ( float(m_hBink->FrameRateDiv)/ float(m_hBink->FrameRate))) 
	{ 
		m_iFrame ++;
		m_flStartTime = time;
		if(m_iFrame> m_hBink->Frames)
		{
			if(!m_bBinkLoop)
			{
				m_iFlags &= ~HUD_ACTIVE;
				BinkClose(m_hBink);
				return;
			}
			else
			{
				m_iFrame = 1;
			}
		}
		BinkGoto(m_hBink, m_iFrame, 0);
		BinkCopyToBuffer(m_hBink, m_pBinkMem, m_hBink->Width * 4, m_hBink->Height, 0, 0, BINKSURFACE32RA);
		Tri_BindTexture(GL_TEXTURE_2D, m_iBinkTextureID);
		Tri_SetTextureParam();		
		Tri_TexImage2D(GL_TEXTURE_2D, 0, 4, m_hBink->Width, m_hBink->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pBinkMem);
	}
	int iX ,iY,iWidth,iHeight;
	iWidth = m_hBink->Width;
	iHeight = m_hBink->Height;

	if(m_iCenter)
	{
		iX = m_iX - iWidth/2;
		iY = m_iY - iHeight/2;
	}
	else
	{
		iX = m_iX ;
		iY = m_iY ;
	}

	Tri_Enable(GL_TEXTURE_2D);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Tri_BindTexture(GL_TEXTURE_2D, m_iBinkTextureID);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->Color4ub(m_iColor[0],m_iColor[1],m_iColor[2],255);
	gEngfuncs.pTriAPI->TexCoord2f(0,1);
	gEngfuncs.pTriAPI->Vertex3f(iX,iY+iHeight,0);
	gEngfuncs.pTriAPI->TexCoord2f(1,1);
	gEngfuncs.pTriAPI->Vertex3f(iX+iWidth,iY+iHeight,0);
	gEngfuncs.pTriAPI->TexCoord2f(1,0);
	gEngfuncs.pTriAPI->Vertex3f(iX+iWidth,iY,0);
	gEngfuncs.pTriAPI->TexCoord2f(0,0);
	gEngfuncs.pTriAPI->Vertex3f(iX,iY,0);
	gEngfuncs.pTriAPI->End();
}
