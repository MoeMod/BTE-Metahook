#include "base.h" 
#include "TriAPI.h"
#include "Fonts.h" 
#include "BaseUI.h" 

Font g_Font;
Font g_FontBold;

int Font::Init(char *name)
{
	char szFileName[128];
	sprintf(szFileName, "resource/font/%s", name);
	/*
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(szFileName, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		sprintf(szFileName, "cstrike/resource/font/%s", name);
	else
		FindClose(hFind);
		*/

	g_pFileSystem->GetLocalPath(szFileName, szFileName, sizeof(szFileName));

	int iError = 0;
	iError = FT_Init_FreeType(&m_sLibrary);
	if (iError) return iError;

	iError = FT_New_Face(m_sLibrary, szFileName, 0, &m_sFace);
	if (iError) return iError;

	m_iCharCount = 0;
	m_iWidth = 16;
	m_iHeight = 16;
	m_iColor[0] = m_iColor[1] = m_iColor[2] = m_iColor[3] = 255;

	FT_Select_Charmap(m_sFace, FT_ENCODING_UNICODE);
	FT_Set_Pixel_Sizes(m_sFace, m_iWidth, m_iHeight);

	for (int i = 0; i < 63; i++)
	{
		for (int j = 0; j < FONT_PAGE_MAX; j++)
		{
			m_iPage[i][j] = 0;
			m_iPageCount[i][j] = 0;
			m_bPageFull[i][j] = false;
		}
	}

	for (int i = 0; i<FONTS_MAX_BUFFER; i++)
		m_tFontTexture[i].m_iTexture = 0;

	m_iLastDrawTexture = 0;
	return 0;
}

int Font::CheckCharExists(wchar_t ch)
{
	for (int i = 0; i < m_iCharCount; i++)
	{
		if (65536 * m_iWidth + ch == m_iBuffer[i])
			return i;
	}

	return -1;
}

int Font::AddCharInToPage(CHARINFO *pChar)
{
	int iRealWidth = m_iWidth + 4;
	// calc max chars
	int iLineChars = 256 / iRealWidth - 1;
	int iMaxChars = iLineChars * iLineChars;
	int i = 0;

	for (i = 0; i<FONT_PAGE_MAX; i++)
	{
		if (m_bPageFull[m_iWidth][i]) continue;
		if (!m_iPage[m_iWidth][i])
		{
			m_iPage[m_iWidth][i] = vgui::surface()->CreateNewTextureID();
			vgui::surface()->DrawSetTexture(m_iPage[m_iWidth][i]);
			// bind null texture
			static BYTE byteNull[256 * 256 * 4];
			memset(byteNull, 0, sizeof(byteNull));
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, byteNull);

			//LogToFile("Add new page: %d %d - %d", m_iWidth, i, m_iPage[m_iWidth][i]);
		}
		m_iPageCount[m_iWidth][i] ++;
		if (m_iPageCount[m_iWidth][i] == iMaxChars)
		{
			m_bPageFull[m_iWidth][i] = true;
		}
		//calc pos
		int V = (m_iPageCount[m_iWidth][i] - 1) / (iLineChars); // hard to calculate >.<
		int H = (m_iPageCount[m_iWidth][i] - 1) % iLineChars; // hard to calculate >.<
		float fTexCoordX = float(H*iRealWidth + 2) / 256.0;
		float fTexCoordY = float(V*iRealWidth + 2) / 256.0;
		float fTexCoordX2 = float(H*iRealWidth + 2 + pChar->m_iWidth) / 256.0;
		float fTexCoordY2 = float(V*iRealWidth + 2 + pChar->m_iHeight) / 256.0;
		pChar->m_fCoord[0][0] = fTexCoordX;
		pChar->m_fCoord[0][1] = fTexCoordX2;
		pChar->m_fCoord[1][0] = fTexCoordY;
		pChar->m_fCoord[1][1] = fTexCoordY2;
		//update texture
		//vgui::surface()->DrawSetTexture(m_iPage[m_iWidth][i]);
		glBindTexture(GL_TEXTURE_2D, m_iPage[m_iWidth][i]);
		pChar->m_iTexture = m_iPage[m_iWidth][i];
		pChar->m_iCharWidth = m_iWidth;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexSubImage2D(GL_TEXTURE_2D, 0, H*iRealWidth + 2, V*iRealWidth + 2, pChar->m_iWidth, pChar->m_iHeight, GL_RGBA, GL_UNSIGNED_BYTE, pChar->pTexture);
		//LogToFile("Add new char: %s %d [%d page:%d count:%d]", UnicodeToANSI(&pChar->wChar), m_iWidth, i, m_iPage[m_iWidth][i], m_iPageCount[m_iWidth][i]);
		//LogToFile("Add new char: %s texture: %d error: %d", UnicodeToANSI(&pChar->wChar), m_iPage[m_iWidth][i], glGetError());
		break;

	}
	return m_iPage[m_iWidth][i];
}

int Font::LoadChar(wchar_t ch)
{
	int iCallBack = CheckCharExists(ch);
	if (iCallBack > -1) return iCallBack;

	if (FT_Load_Char(m_sFace, ch, FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL)) return -1;

	FT_Glyph sGlyph;
	if (FT_Get_Glyph(m_sFace->glyph, &sGlyph)) return 0;
	FT_Render_Glyph(m_sFace->glyph, FT_RENDER_MODE_LCD);
	FT_Glyph_To_Bitmap(&sGlyph, ft_render_mode_normal, 0, 1);
	FT_BitmapGlyph sBitmapGlyph = (FT_BitmapGlyph)sGlyph;

	static int id;
	if (!id)
		id = vgui::surface()->CreateNewTextureID();

	m_tFontTexture[m_iCharCount].m_iTexture = id;
	FT_Bitmap sBitmap = sBitmapGlyph->bitmap;

	m_tFontTexture[m_iCharCount].m_iAdvWidth = m_sFace->glyph->advance.x / 64.0f;
	m_tFontTexture[m_iCharCount].m_iAdvHeight = m_sFace->size->metrics.y_ppem;
	m_tFontTexture[m_iCharCount].m_iDeltaWidth = (float)sBitmapGlyph->left;
	m_tFontTexture[m_iCharCount].m_iDeltaHeight = (float)sBitmapGlyph->top - sBitmap.rows;
	m_tFontTexture[m_iCharCount].m_iWidth = sBitmap.width;
	m_tFontTexture[m_iCharCount].m_iHeight = sBitmap.rows;
	m_tFontTexture[m_iCharCount].wChar = ch;

	BYTE* pBuf = new byte[m_tFontTexture[m_iCharCount].m_iWidth  * m_tFontTexture[m_iCharCount].m_iHeight * 4];
	for (int j = 0; j < m_tFontTexture[m_iCharCount].m_iHeight; j++)
	{
		for (int i = 0; i < m_tFontTexture[m_iCharCount].m_iWidth; i++)
		{
			unsigned char iByte = (i >= m_tFontTexture[m_iCharCount].m_iWidth || j >= m_tFontTexture[m_iCharCount].m_iHeight) ? 0 : sBitmap.buffer[i + m_tFontTexture[m_iCharCount].m_iWidth*j];
			pBuf[(4 * i + (m_tFontTexture[m_iCharCount].m_iHeight - j - 1) * m_tFontTexture[m_iCharCount].m_iWidth * 4)] = 255;
			pBuf[(4 * i + (m_tFontTexture[m_iCharCount].m_iHeight - j - 1) * m_tFontTexture[m_iCharCount].m_iWidth * 4) + 1] = 255;
			pBuf[(4 * i + (m_tFontTexture[m_iCharCount].m_iHeight - j - 1) * m_tFontTexture[m_iCharCount].m_iWidth * 4) + 2] = 255;
			pBuf[(4 * i + (m_tFontTexture[m_iCharCount].m_iHeight - j - 1) * m_tFontTexture[m_iCharCount].m_iWidth * 4) + 3] = iByte;
		}
	}
	m_tFontTexture[m_iCharCount].pTexture = pBuf;
	AddCharInToPage(&m_tFontTexture[m_iCharCount]);

	delete[] pBuf;
	m_iBuffer[m_iCharCount] = 65536 * m_iWidth + ch;
	m_iCharCount++;
	return (m_iCharCount - 1);
}
CHARINFO *Font::GetChar(wchar_t ch)
{
	return &m_tFontTexture[LoadChar(ch)];
}
int Font::GetLen(wchar_t* szText)
{
	int iRet = 0;
	int iLen = (int)wcslen(szText);

	for (int i = 0; i <iLen; i++)
		iRet += GetChar(szText[i])->m_iAdvWidth;

	return iRet;
}
int Font::GetHeight(wchar_t* szText, int iMaxWidth, int iHeightAdd)
{
	int iRet = 0, iHeight = m_iHeight;
	int iLen = (int)wcslen(szText);

	for (int i = 0; i <iLen; i++)
	{
		CHARINFO *pCharInfo = GetChar(szText[i]);

		if (szText[i] == '\n' || szText[i] == '\r')
		{
			iRet = 0;
			iHeight += m_iHeight + iHeightAdd;
			continue;
		}

		if (szText[i] <= 4)
			continue;

		iRet += pCharInfo->m_iAdvWidth;

		if (iRet > iMaxWidth && i != (iLen - 1))
		{
			iRet = 0;
			iHeight += m_iHeight + iHeightAdd;
		}

	}

	return iHeight;
}

int Font::DrawString(wchar_t* szText, int x, int y, int iMaxWidth, int iHeightAdd, bool bDrawNewLine)
{
	int iDrawX = x;
	int iDrawY = y;
	int iLen = (int)wcslen(szText);

	/*Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);*/

	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColor4ub(m_iColor[0], m_iColor[1], m_iColor[2], m_iColor[3]);
	m_iLastDrawTexture = 0;

	for (int i = 0; i <iLen; i++)
	{
		if (bDrawNewLine)
		{
			if (szText[i] == '\n' || szText[i] == '\r')
			{
				iDrawX = x;
				iDrawY += iHeightAdd;
				continue;
			}
		}
		else
		{
			if (szText[i] == '\n' || szText[i] == '\r')
				continue;
		}

		CHARINFO* pCharInfo = GetChar(szText[i]);

		if (iHeightAdd < pCharInfo->m_iHeight) iHeightAdd = pCharInfo->m_iHeight; // Update iHeightAdd. Should be small than char

		if (m_iLastDrawTexture != pCharInfo->m_iTexture)
		{
			vgui::surface()->DrawSetTexture(pCharInfo->m_iTexture);
			m_iLastDrawTexture = pCharInfo->m_iTexture;
		}

		//glEnable(GL_BLEND);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);

		gEngfuncs.pTriAPI->TexCoord2f(pCharInfo->m_fCoord[0][0], pCharInfo->m_fCoord[1][1]); gEngfuncs.pTriAPI->Vertex3f(iDrawX + pCharInfo->m_iDeltaWidth, iDrawY - pCharInfo->m_iHeight - pCharInfo->m_iDeltaHeight, 0);
		gEngfuncs.pTriAPI->TexCoord2f(pCharInfo->m_fCoord[0][1], pCharInfo->m_fCoord[1][1]); gEngfuncs.pTriAPI->Vertex3f(iDrawX + pCharInfo->m_iDeltaWidth + pCharInfo->m_iWidth, iDrawY - pCharInfo->m_iHeight - pCharInfo->m_iDeltaHeight, 0);
		gEngfuncs.pTriAPI->TexCoord2f(pCharInfo->m_fCoord[0][1], pCharInfo->m_fCoord[1][0]); gEngfuncs.pTriAPI->Vertex3f(iDrawX + pCharInfo->m_iDeltaWidth + pCharInfo->m_iWidth, iDrawY - pCharInfo->m_iDeltaHeight, 0);
		gEngfuncs.pTriAPI->TexCoord2f(pCharInfo->m_fCoord[0][0], pCharInfo->m_fCoord[1][0]); gEngfuncs.pTriAPI->Vertex3f(iDrawX + pCharInfo->m_iDeltaWidth, iDrawY - pCharInfo->m_iDeltaHeight, 0);

		gEngfuncs.pTriAPI->End();

		iDrawX += pCharInfo->m_iAdvWidth;
		if (iDrawX > x + iMaxWidth)
		{
			iDrawX = x;
			iDrawY += iHeightAdd/*pCharInfo->m_iHeight*/;
		}
	}

	return iDrawX;
}

int Font::DrawString(wchar_t *szText, int x, int y, int iMaxWidth, int iMaxHeight, int iHeightAdd)
{
	int iDrawX = x;
	int iDrawY = y;
	int iLen = wcslen(szText);
	int iMaxH = iMaxHeight;

	for (int i = 0; i < iLen; i++)
	{
		if (szText[i] == '\n')
		{
			iDrawX = x;
			iDrawY += (m_iHeight + iHeightAdd);
			continue;
		}
		CHARINFO* pCharInfo = GetChar(szText[i]);

		glBindTexture(GL_TEXTURE_2D, pCharInfo->m_iTexture);
		Tri_SetTextureParam();
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE);

		int iW = pCharInfo->m_iWidth;
		int iH = pCharInfo->m_iHeight;
		int iCharX = iDrawX + pCharInfo->m_iDeltaWidth;
		int iCharY = iDrawY - iH - pCharInfo->m_iDeltaHeight;

		if (iMaxH < iH)
			iMaxH = iH;

		if (szText[i] == '$')
			iCharY -= 1;

		Tri_Enable(GL_BLEND);
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4ub(m_iColor[0], m_iColor[1], m_iColor[2], m_iColor[3]);

		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		{
			gEngfuncs.pTriAPI->TexCoord2f(0.0f, 1.0f); gEngfuncs.pTriAPI->Vertex3f(iCharX, iCharY, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1.0f, 1.0f); gEngfuncs.pTriAPI->Vertex3f(iCharX + iW, iCharY, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1.0f, 0.0f); gEngfuncs.pTriAPI->Vertex3f(iCharX + iW, iCharY + iH, 0);
			gEngfuncs.pTriAPI->TexCoord2f(0.0f, 0.0f); gEngfuncs.pTriAPI->Vertex3f(iCharX, iCharY + iH, 0);
		}
		gEngfuncs.pTriAPI->End();

		iDrawX += pCharInfo->m_iAdvWidth;

		if (iDrawX > x + iMaxWidth)
		{
			iDrawX = x;
			iDrawY += iMaxHeight + iHeightAdd;
		}
	}
	return iDrawX;
}

void Font::DrawString(wchar_t* szText, int x, int y, int iMaxWidth, int &rx, int & ry, int(*check) (wchar_t ch), int iHeightAdd)
{
	int iDrawX = x;
	int iDrawY = y;
	int iLen = (int)wcslen(szText);

	/*Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);*/

	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	m_iLastDrawTexture = 0;

	glColor4ub(m_iColor[0], m_iColor[1], m_iColor[2], m_iColor[3]);

	for (int i = 0; i <iLen; i++)
	{
		if (szText[i] == '\n' || szText[i] == '\r')
		{
			iDrawX = x;
			iDrawY += iHeightAdd;
			continue;
		}

		if (iDrawX > x + iMaxWidth)
		{
			iDrawX = x;
			iDrawY += iHeightAdd/*pCharInfo->m_iHeight*/;
		}

		if (check)
		{
			if (!check(szText[i]))
				continue;

			glColor4ub(m_iColor[0], m_iColor[1], m_iColor[2], m_iColor[3]);
		}

		CHARINFO* pCharInfo = GetChar(szText[i]);

		if (iHeightAdd < pCharInfo->m_iHeight) iHeightAdd = pCharInfo->m_iHeight; // Update iHeightAdd. Should be small than char

		if (m_iLastDrawTexture != pCharInfo->m_iTexture)
		{
			vgui::surface()->DrawSetTexture(pCharInfo->m_iTexture);
			m_iLastDrawTexture = pCharInfo->m_iTexture;
		}

		//glEnable(GL_BLEND);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);

		gEngfuncs.pTriAPI->TexCoord2f(pCharInfo->m_fCoord[0][0], pCharInfo->m_fCoord[1][1]); gEngfuncs.pTriAPI->Vertex3f(iDrawX + pCharInfo->m_iDeltaWidth, iDrawY - pCharInfo->m_iHeight - pCharInfo->m_iDeltaHeight, 0);
		gEngfuncs.pTriAPI->TexCoord2f(pCharInfo->m_fCoord[0][1], pCharInfo->m_fCoord[1][1]); gEngfuncs.pTriAPI->Vertex3f(iDrawX + pCharInfo->m_iDeltaWidth + pCharInfo->m_iWidth, iDrawY - pCharInfo->m_iHeight - pCharInfo->m_iDeltaHeight, 0);
		gEngfuncs.pTriAPI->TexCoord2f(pCharInfo->m_fCoord[0][1], pCharInfo->m_fCoord[1][0]); gEngfuncs.pTriAPI->Vertex3f(iDrawX + pCharInfo->m_iDeltaWidth + pCharInfo->m_iWidth, iDrawY - pCharInfo->m_iDeltaHeight, 0);
		gEngfuncs.pTriAPI->TexCoord2f(pCharInfo->m_fCoord[0][0], pCharInfo->m_fCoord[1][0]); gEngfuncs.pTriAPI->Vertex3f(iDrawX + pCharInfo->m_iDeltaWidth, iDrawY - pCharInfo->m_iDeltaHeight, 0);

		gEngfuncs.pTriAPI->End();

		iDrawX += pCharInfo->m_iAdvWidth;
	}

	rx = iDrawX;
	ry = iDrawY;
}


void Font::C2W(wchar_t *pwstr, size_t len, const char *str)
{
	if (str)
	{
		size_t nu = strlen(str);
		size_t n = (size_t)::MultiByteToWideChar(CP_ACP, 0, (const char *)str, (int)nu, NULL, 0);
		if (n >= len) n = len - 1;
		::MultiByteToWideChar(CP_ACP, 0, (const char *)str, (int)nu, pwstr, (int)n);
		pwstr[n] = 0;
	}
}

void Font::LoadStr(wchar_t* _strText)
{
	//if (!_strText)
	return;

	size_t nLen = wcslen(_strText);
	for (int i = 0; i <(int)nLen; i++)
		LoadChar(_strText[i]);
}

void Font::SetWidth(int iWidth)
{
	m_iWidth = m_iHeight = iWidth;
	FT_Set_Pixel_Sizes(m_sFace, iWidth, iWidth);
	//FT_Set_Char_Size(
}