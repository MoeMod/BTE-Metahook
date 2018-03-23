
#include "hud.h"
#include "DrawTargaImage.h"
#include "Fonts.h"
#include "util.h"
#include "FontText.h"

#include "Client/HUD/DrawTGA.h"

static CHudFontTextElements g_HudFontTextElements;
CHudFontTextElements &HudFontTextElements()
{
	return g_HudFontTextElements;
}

DrawFontTextItem g_rgDrawFontText[MAX_DRAWFONTTEXT];
int m_TextBg_Right,m_TextBg_Left,m_TextBg_Center;

void CHudFontTextElements::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}

void CHudFontTextElements::VidInit(void)
{
	memset(g_rgDrawFontText, 0, sizeof(g_rgDrawFontText));
	m_TextBg_Right = Hud().m_TGA.FindTexture("resource\\hud\\other\\textbg_right");
	m_TextBg_Left = Hud().m_TGA.FindTexture("resource\\hud\\other\\textbg_left");
	m_TextBg_Center = Hud().m_TGA.FindTexture("resource\\hud\\other\\textbg_center");
}

void CHudFontTextElements::AddElement(DrawFontTextItem rgTempDrawText)
{
	if (rgTempDrawText.iChanne <= 0 || rgTempDrawText.iChanne >= MAX_DRAWFONTTEXT)
	{
		for (int i = 0; i < MAX_DRAWFONTTEXT; i++)
		{
			if (!g_rgDrawFontText[i].flDisplayTime)
			{
				g_rgDrawFontText[i] = rgTempDrawText;
				break;
			}
		}
	}
	else
		g_rgDrawFontText[rgTempDrawText.iChanne] = rgTempDrawText;
}

void CHudFontTextElements::Draw(float time)
{
	for (int i = 0; i < MAX_DRAWFONTTEXT; i++)
	{
		if (!g_rgDrawFontText[i].flDisplayTime)
			continue;

		if (time > g_rgDrawFontText[i].flDisplayTime)
		{
			g_rgDrawFontText[i].flDisplayTime = 0.0;
			continue;
		}
		int iAlpha;
		// Set Color
		if(g_rgDrawFontText[i].fFadeTime < 0.1f)
		{
			iAlpha = 255;
		}
		else
		{
			float fDes = time - g_rgDrawFontText[i].flStartTime;
			if(fDes < g_rgDrawFontText[i].fFadeTime)
			{
				// Set Alpha
				iAlpha = min(255,fDes / g_rgDrawFontText[i].fFadeTime * 255);
			}
			else if(g_rgDrawFontText[i].flDisplayTime - time<g_rgDrawFontText[i].fFadeTime)
			{
				float iLess = g_rgDrawFontText[i].flDisplayTime - time;
				iAlpha = iLess / g_rgDrawFontText[i].fFadeTime * 255;
			}
			else iAlpha = 255;
		}
		g_FontBold.SetWidth(g_rgDrawFontText[i].iScale);
		g_FontBold.SetColor(g_rgDrawFontText[i].color.r, g_rgDrawFontText[i].color.g, g_rgDrawFontText[i].color.b, iAlpha);
		wchar_t *pText;
		pText = UTF8ToUnicode(g_rgDrawFontText[i].szText);
		int iLen = g_FontBold.GetLen(pText);
		// Add Bg
		if(g_rgDrawFontText[i].iFillBg)
		{
			static int iX,iY;
			iX = ScreenWidth * 0.15;
			iY = g_rgDrawFontText[i].y - 5 - g_rgDrawFontText[i].iScale;
			// Left
			GL_DrawTGA2(g_Texture[m_TextBg_Left].iTexture,iX,iY,g_Texture[m_TextBg_Left].iWidth,20 + g_rgDrawFontText[i].iScale, iAlpha);
			// Center
			iX +=g_Texture[m_TextBg_Right].iWidth;
			GL_DrawTGA2(g_Texture[m_TextBg_Center].iTexture,iX,iY,ScreenWidth*0.85-g_Texture[m_TextBg_Right].iWidth-iX,20 + g_rgDrawFontText[i].iScale, iAlpha);
			// Right
			iX =ScreenWidth*0.85-g_Texture[m_TextBg_Right].iWidth;
			GL_DrawTGA2(g_Texture[m_TextBg_Right].iTexture,iX,iY,g_Texture[m_TextBg_Right].iWidth,20 + g_rgDrawFontText[i].iScale, iAlpha);
		}

		if(g_rgDrawFontText[i].iCenter)
			g_FontBold.DrawString(pText, g_rgDrawFontText[i].x - iLen / 2, g_rgDrawFontText[i].y, 1000);
		else
			g_FontBold.DrawString(pText, g_rgDrawFontText[i].x, g_rgDrawFontText[i].y, 1000);
	}
}