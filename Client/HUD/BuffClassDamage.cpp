#include "base.h"
#include "hud.h"
#include "DrawTargaImage.h"
#include "texdraw.h"
#include "BaseUI.h"
#include "BuffClassDamage.h"

static CHudBuffClassDamage g_HudBuffClassDamage;
CHudBuffClassDamage &HudBuffClassDamage()
{
	return g_HudBuffClassDamage;
}

void CHudBuffClassDamage::Init(void)
{
	m_iFlags |= HUD_ACTIVE;

	Reset();
}

void CHudBuffClassDamage::VidInit(void)
{
	m_iTgaBG = Hud().m_TGA.FindTexture("resource\\helperhud\\classdamage");

	m_iNumber = g_Texture[Hud().m_TGA.FindTexture("resource\\hud\\zombie\\z4number")].iTexture;
	BuildNumberRC(m_rcNumber, 14, 32);
	/*
	for (int i = 0; i < 10; i++)
	{
		wchar_t a = L'0' + (wchar_t)i;
		m_iNumWidth[i] = g_Font.GetChar(a)->m_iWidth;
	}*/
}

void CHudBuffClassDamage::ShutDown(void)
{

}

void CHudBuffClassDamage::Reset(void)
{
	m_iLastDamage = 0;
	m_iTotalDamage = 0;
	m_bEnabled = false;
}

void CHudBuffClassDamage::UpdateDamage(int iDamage)
{
	m_iLastDamage = iDamage;
	m_iTotalDamage += iDamage;
	m_bEnabled = true;
}
/*
int CHudBuffClassDamage::CalcNumWidth(int x)
{
	int iResult = 0;
	while (x)
	{
		iResult += m_iNumWidth[x % 10];
		x /= 10;
	}
	return iResult;
}
*/
void DrawTexture(int index, int iX, int iY, int iW, int iH, int alpha);

void CHudBuffClassDamage::Draw(float flTime)
{
	if (!m_bEnabled)
		return;
	int x = ScreenWidth - g_Texture[m_iTgaBG].iWidth;
	int y = 20 + 17;
	int iWidth = g_Texture[m_iTgaBG].iWidth;
	int iHeight = g_Texture[m_iTgaBG].iHeight;
	// Background
	GL_DrawTGA2(g_Texture[m_iTgaBG].iTexture, x, y, iWidth, iHeight, 255);
	// Text
	/*
	wchar_t wszText[16];
	int iWidth2;

	g_FontBold.SetWidth(13);
	iWidth2 = CalcNumWidth(m_iLastDamage);
	g_FontBold.SetColor(255, 255, 255, 255);
	swprintf(wszText, L"%d", m_iLastDamage);
	g_FontBold.DrawString(wszText, x + iWidth - iWidth2 - 11, y + 26 - 5, 1000);

	g_FontBold.SetWidth(13);
	iWidth2 = CalcNumWidth(m_iTotalDamage);
	g_FontBold.SetColor(255, 73, 176, 255);
	swprintf(wszText, L"%d", m_iTotalDamage);
	
	g_FontBold.DrawString(wszText, x + iWidth - iWidth2 - 11, y + 41 - 5, 1000);*/

	TEXD_SetColor(255, 255, 255, 255);
	DrawNumbers(m_iLastDamage, x + iWidth - 11 - 50, y + 26 - 15, true, 0.5325);
	TEXD_SetColor(255, 73, 176, 255);
	DrawNumbers(m_iTotalDamage, x + iWidth - 11 - 50, y + 41 - 15, true, 0.5325);
}

void CHudBuffClassDamage::DrawNumber(int n, int x, int y, float size)
{
	TEXD_BindTexture(m_iNumber);
	TEXD_DrawTexture(x, y, 256, 32, &m_rcNumber[n], size);
}

void CHudBuffClassDamage::DrawNumbers(int n, int x, int y, int from_right, float size)
{
	int width = 14;
	width *= size;

	for (int i = 1000000; i > 0; i/=10)
	{
		int k;
		if (n >= i)
		{
			k = (n % (i * 10)) / i;
			DrawNumber(k, x, y, size);
			if (!from_right)
				x += width;
		}
		if (from_right)
			x += width;
	}
}

void CHudBuffClassDamage::BuildNumberRC(wrect_t *rgrc, int w, int h)
{
	int nw = 0;
	int nh = 0;

	for (int i = 0; i < 10; i++)
	{
		rgrc[i].left = nw;
		rgrc[i].top = 0;
		rgrc[i].right = rgrc[i].left + w;
		rgrc[i].bottom = h;

		nw += w;
		nh += h;
	}
}