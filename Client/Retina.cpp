#include "base.h"
#include "hud.h"
#include "BaseUI.h"
#include "texdraw.h"
#include "Retina.h"

void CHudRetinaObjects::Init()
{
	//m_iTextureId = vgui::surface()->CreateNewTextureID();
}

void CHudRetinaObjects::VidInit(char *filename, bool drawmode, bool flash)
{
	m_bCanDraw = false;
	
	if(m_iTextureId)
		return;
	
	m_iTextureId = vgui::surface()->CreateNewTextureID();
	LoadTextureTga(filename, m_iTextureId, m_rcTexture.right, m_rcTexture.bottom);

	m_bDrawMode = drawmode;
	m_bFlash = flash;	
}

void CHudRetinaObjects::Redraw(float flTime)
{
	if(!m_bCanDraw)
		return;

	if (m_flEndTime < flTime && !m_bFlash)
	{
		m_flEndTime = 0;
		m_bCanDraw = false;
		return;
	}

	TEXD_SetRenderMode(kRenderTransTexture);
	TEXD_BindTexture(m_iTextureId);

	float fSubTime = Hud().m_flTime;
	fSubTime -=(int)fSubTime;
	fSubTime -= 0.5;
	fSubTime = fabsf(fSubTime);	
	float alpha = fSubTime + 0.5;

	alpha *= 255;
	
	if (m_bFlash && m_flEndTime < flTime)
	{
		m_color[3] -= 7;

		if (m_color[3] <= 0)
		{
			m_flEndTime = 0;
			m_bCanDraw = false;
			return;
		}
	}
	else if (m_bFlash)
	{
		m_color[3] = alpha;
	}

	if (m_flEndTime < flTime && m_bFlash && alpha < 10)
	{
		m_flEndTime = 0;
		m_bCanDraw = false;
		return;
	}	

	TEXD_SetColor(m_color[0], m_color[1], m_color[2], m_color[3]);

	if(!m_bDrawMode)
	{
		int iW = ScreenWidth;
		int iH = ScreenHeight;
		
		TEXD_DrawTexture(0, 0, iW, iH, NULL);
	}
	else
	{
		int iW = ScreenWidth / 2;
		int iH = ScreenHeight / 2;
		
		m_rcTexture.bottom = iH;
		m_rcTexture.right = iW;
	
		TEXD_DrawTexture(0, 0, iW, iH, &m_rcTexture, 1.0, 0);
		TEXD_DrawTexture(iW, 0, iW, iH, &m_rcTexture, 1.0, 1);
		TEXD_DrawTexture(0, iH, iW, iH, &m_rcTexture, 1.0, 2);
		TEXD_DrawTexture(iW, iH, iW, iH, &m_rcTexture, 1.0, 3);
	}
}