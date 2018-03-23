#include "base.h"
#include "hud.h"
#include "TexDraw.h"
#include "DrawRetina.h"

#include "Client/HUD/DrawTGA.h"

static CHudRetina g_HudRetina;
CHudRetina &HudRetina()
{
	return g_HudRetina;
}

void CHudRetina::Init()
{
	m_iFlags |= HUD_ACTIVE;
}

void CHudRetina::VidInit()
{
	Reset();
}

void CHudRetina::Reset()
{
	memset(m_iRetinaAlpha,0,sizeof(m_iRetinaAlpha));
	memset(m_iRetina,0,sizeof(m_iRetina));
	memset(m_iRetinaDraw,0,sizeof(m_iRetinaDraw));
	memset(m_iRetinaFull,0,sizeof(m_iRetinaFull));
	memset(m_iRetinaFlash,0,sizeof(m_iRetinaFlash));
	memset(m_flRetinaFinishTime,0,sizeof(m_flRetinaFinishTime));
}

void CHudRetina::Draw(float time)
{
	for (int i=0;i<=7;i++)
	{
		if (!g_bAlive)
		{
			m_iRetinaDraw[i] = 0;
			break;
		}
		
		if (!m_iRetinaDraw[i])
			continue;

		float alpha;

		float fSubTime = time;
		fSubTime -=(int)fSubTime;
		fSubTime -= 0.5;
		fSubTime= fabsf(fSubTime);	
		alpha = fSubTime + 0.5;

		alpha *= 255;

		if (!m_iRetinaFlash[i]) alpha = 255;
		
		if (m_flRetinaFinishTime[i] < time && !m_iRetinaFlash[i])
		{
			m_iRetinaDraw[i] = 0;
			continue;
		}

		if (m_flRetinaFinishTime[i] < time && m_iRetinaFlash[i])
		{
			if (!m_iRetinaAlpha[i])
				m_iRetinaAlpha[i] = alpha;

			m_iRetinaAlpha[i] -= 7;
			alpha = m_iRetinaAlpha[i];

			if (m_iRetinaAlpha[i] <= 0)
			{
				m_iRetinaAlpha[i] = 0;
				m_iRetinaDraw[i] = 0;
				continue;
			}
		}

		TEXD_BeginDraw();
		TEXD_SetRenderMode(kRenderTransTexture);
		TEXD_BindTexture(g_MHTga[m_iRetina[i]].texid);
		TEXD_SetColor(255,255,255, alpha);
		
		if (!m_iRetinaFull[i])
		{
			int iW = ScreenWidth / 2;
			int iH = ScreenHeight / 2;
			
			wrect_t rcTexture;
			rcTexture.left = 0;
			rcTexture.top = 0;
			rcTexture.bottom = iH;
			rcTexture.right = iW;
	
			TEXD_DrawTexture(0, 0, iW, iH, &rcTexture, 1.0, 0);
			TEXD_DrawTexture(iW, 0, iW, iH, &rcTexture, 1.0, 1);
			TEXD_DrawTexture(0, iH, iW, iH, &rcTexture, 1.0, 2);
			TEXD_DrawTexture(iW, iH, iW, iH, &rcTexture, 1.0, 3);
	
		}
		else
		{
			TEXD_DrawTexture(0, 0, ScreenWidth, ScreenHeight, NULL);
		}
	}
}

void CHudRetina::ParseData(int index, int iDraw, int iFullScreen, int iFlash, int iChan, float fTime)
{	
	m_iRetina[iChan] = index;	
	m_iRetinaDraw[iChan] = iDraw;
	m_iRetinaFull[iChan] = iFullScreen;	
	m_iRetinaFlash[iChan] = iFlash;
	m_flRetinaFinishTime[iChan] = fTime;
}

void CHudRetina::ParseData(char *szTexture, int iDraw, int iFullScreen, int iFlash, int iChan, float fTime)
{
	m_iRetina[iChan] = Hud().m_TGA.GetTGA(szTexture, true);
	m_iRetinaDraw[iChan] = iDraw;
	m_iRetinaFull[iChan] = iFullScreen;
	m_iRetinaFlash[iChan] = iFlash;
	m_flRetinaFinishTime[iChan] = fTime;
}