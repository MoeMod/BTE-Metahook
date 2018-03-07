#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define FONTS_MAX_BUFFER	1000
#define FONT_PAGE_MAX	8


struct CHARINFO
{
	int m_iTexture;
	int m_iWidth,m_iHeight;
	int m_iDeltaWidth,m_iDeltaHeight;
	int m_iAdvWidth,m_iAdvHeight;
	BYTE *pTexture;
	float m_fCoord[2][2];
	int m_iCharWidth;
	wchar_t wChar;
};

class Font
{
public:
	int Init(char *name);
	int CheckCharExists(wchar_t ch);
	int LoadChar(wchar_t ch);
	CHARINFO *GetChar(wchar_t ch);
	void SetColor(int r,int g,int b,int a) {m_iColor[0] = r; m_iColor[1] = g; m_iColor[2] = b; m_iColor[3] = a;}
	void SetColor(float r,float g,float b,float a) {m_iColor[0] = 255 * r; m_iColor[1] = 255 * g; m_iColor[2] = 255 * b; m_iColor[3] = 255 * a;}

	int DrawString(wchar_t* szText, int x, int y, int iMaxWidth, int iHeightAdd = 0, bool bDrawNewLine = true);
	int DrawString(wchar_t *szText, int x, int y, int iMaxWidth, int iMaxHeight, int iHeightAdd);
	void DrawString(wchar_t* szText, int x, int y, int iMaxWidth, int &rx, int & ry, int (check) (wchar_t ch), int iHeightAdd = 0);
	void SetWidth(int iWidth);
	void SetAlpha(int iAlpha){m_iColor[3] = iAlpha;}
	int GetLen(wchar_t* szText);
	int GetHeight(wchar_t* szText, int iMaxWidth, int iHeightAdd);
	int AddCharInToPage(CHARINFO *pChar);
	void C2W(wchar_t *pwstr, size_t len, const char *str);
	void LoadStr(wchar_t* _strText);
	
private:
	FT_Library  m_sLibrary;
	FT_Face     m_sFace;
private:
	int m_iColor[4];
	int m_iPage[64][FONT_PAGE_MAX];
	int m_iPageCount[64][FONT_PAGE_MAX];
	bool m_bPageFull[64][FONT_PAGE_MAX];

private:
	int m_iCharCount;
	int m_iWidth,m_iHeight;
	int m_iBuffer[FONTS_MAX_BUFFER];
	CHARINFO m_tFontTexture[FONTS_MAX_BUFFER];
	int m_iLastDrawTexture;

};

extern Font g_Font;
extern Font g_FontBold;