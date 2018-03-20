#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "Win32Font.h"
#include <tier0/dbg.h>
#include <vgui/ISurface.h>
#include <tier0/mem.h>
#include <tier1/utlbuffer.h>
#include "IDA.h"
#include "Encode.h"
#include "FontManager.h"
#include <IFileSystem.h>
#include <tier2\tier2.h>

#pragma warning(push)
#pragma warning(disable:4748)

bool CFreeTypeFont::ExtendedABCWidthsCacheLessFunc(const abc_cache_t &lhs, const abc_cache_t &rhs)
{
	return lhs.wch < rhs.wch;
}

CFreeTypeFont::CFreeTypeFont(void): m_ExtendedABCWidthsCache(256, 0, &ExtendedABCWidthsCacheLessFunc)
{
	m_szName[0] = 0;
	m_iTall = 0;
	m_iWeight = 0;
	m_iFlags = 0;
	m_iMaxCharWidth = 0;
	m_bAntiAliased = false;
	m_iBlur = 0;
	m_pGaussianDistribution = NULL;
	m_iScanLines = 0;
	m_bRotary = false;
	m_bAdditive = false;
	m_pBaseFont = NULL;
}

CFreeTypeFont::~CFreeTypeFont(void)
{
}

int CALLBACK FontEnumProc(const LOGFONT *lpelfe, const TEXTMETRIC *lpntme, DWORD FontType, LPARAM lParam);
extern bool g_bFontFound;

#pragma optimize("", off)

bool CFreeTypeFont::Create(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	if (!strcmp(windowsFontName, "system"))
	{
		windowsFontName = FontManager().GetDefaultFontName();
	}
	else
	{
		if (m_pBaseFont == NULL && strcmp(windowsFontName, FontManager().GetDefaultFontName()))
		{
			m_pBaseFont = FontManager().GetBaseFont(tall - 1, weight, blur, scanlines, flags);

			if (!m_pBaseFont)
			{
				m_pBaseFont = new CWin32Font();

				if (m_pBaseFont->Create(FontManager().GetDefaultFontName(), tall, weight, blur, scanlines, flags))
					FontManager().AddBaseFont(m_pBaseFont, tall - 1, weight, blur, scanlines, flags);
			}
		}
	}

	int ascent = 0, height = 0;
	int maxCharWidth = 0;

	strncpy(m_szName, windowsFontName, sizeof(m_szName) - 1);
	m_szName[sizeof(m_szName) - 1] = 0;

	m_iTall = tall - 1;
	m_iWeight = weight;
	m_iFlags = flags;
	m_bAntiAliased = flags & vgui::ISurface::FONTFLAG_ANTIALIAS;
	m_bUnderlined = flags & vgui::ISurface::FONTFLAG_UNDERLINE;
	m_iDropShadowOffset = (flags & vgui::ISurface::FONTFLAG_DROPSHADOW) ? 1 : 0;
	m_iOutlineSize = (flags & vgui::ISurface::FONTFLAG_OUTLINE) ? 1 : 0;
	m_iBlur = blur;
	m_iScanLines = scanlines;
	m_bRotary = flags & vgui::ISurface::FONTFLAG_ROTARY;
	m_bAdditive = flags & vgui::ISurface::FONTFLAG_ADDITIVE;
	m_iFxpHeight = 0;

	int charset = (flags & vgui::ISurface::FONTFLAG_SYMBOL) ? SYMBOL_CHARSET : GB2312_CHARSET;

	const char *fontPath;
	int len = strlen(windowsFontName);

	if (!strnicmp(&windowsFontName[len - 4], ".tt", 3))
	{
		char fullPath[MAX_PATH];

		if (!g_pFullFileSystem->GetLocalPath(windowsFontName, fullPath, sizeof(fullPath)))
		{
			m_szName[0] = 0;
			return false;
		}

		fontPath = fullPath;
	}
	else
	{
		fontPath = FontManager().GetFontFilePath((char *)windowsFontName);

		if (!fontPath)
		{
			m_szName[0] = 0;
			return false;
		}
	}

	if (FT_New_Face(FontManager().GetFreeTypeLibrary(), fontPath, 0, &face))
		return false;

	if (flags & vgui::ISurface::FONTFLAG_SYMBOL)
		FT_Select_Charmap(face, FT_ENCODING_MS_SYMBOL);
	else
		FT_Select_Charmap(face, FT_ENCODING_UNICODE);

	FT_Set_Pixel_Sizes(face, 0, tall);

	if (face->size->metrics.ascender & 0x20)
		ascent = (face->size->metrics.ascender >> 31) | 1;

	m_iAscent = (face->size->metrics.ascender >> 6) + ascent;

	if (face->size->metrics.height & 0x20)
		height = (face->size->metrics.height >> 31) | 1;

	m_iHeight = (m_iOutlineSize * 2) + m_iDropShadowOffset + (face->size->metrics.height >> 6) + height;

	if (face->size->metrics.max_advance & 0x20)
		maxCharWidth = (face->size->metrics.max_advance >> 31) | 1;

	m_iMaxCharWidth = (face->size->metrics.max_advance >> 6) + maxCharWidth;
	m_iFxpHeight = face->size->metrics.height + (((m_iOutlineSize * 2) + m_iDropShadowOffset) << 6);

	if (m_iBlur > 1)
	{
		m_pGaussianDistribution = new float[m_iBlur * 2 + 1];
		double sigma = 0.683 * m_iBlur;

		for (int x = 0; x <= (m_iBlur * 2); x++)
		{
			int val = x - m_iBlur;
			m_pGaussianDistribution[x] = (float)(1.0f / sqrt(2 * 3.14 * sigma * sigma)) * pow(2.7, -1 * (val * val) / (2 * sigma * sigma));
			m_pGaussianDistribution[x] *= 1;
		}
	}

	return true;
}

void CFreeTypeFont::GetCharRGBA(int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba)
{
	wchar_t wch = (wchar_t)ch;
	FT_UInt index = FT_Get_Char_Index(face, ch);

	if (!index)
	{
		if (m_pBaseFont)
			m_pBaseFont->GetCharRGBA(ch, rgbaX, rgbaY, rgbaWide, rgbaTall, rgba);

		return;
	}

	if (FT_Load_Glyph(face, index, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP))
		return;

	if (m_iWeight)
		FT_Bitmap_Embolden(FontManager().GetFreeTypeLibrary(), &face->glyph->bitmap, m_iWeight * 0.075, 0);

	int wide = m_iMaxCharWidth;
	int tall = m_iHeight;

	int v11 = face->glyph->bitmap.width;
	int v12 = m_iAscent - face->glyph->bitmap_top;
	int v32 = face->glyph->bitmap.rows;
	int v9 = 0;
	unsigned __int8 *v13;
	unsigned __int8 *v14;
	int v15, v16, v17;
	int v30;

	if (v12 < 0)
		v9 = -v12;

	int rows = face->glyph->bitmap.rows;

	if (v12 + rows > rgbaTall)
		v32 += rgbaTall - (v12 + rows);

	if (v9 < v32)
	{
		v13 = &rgba[4 * rgbaWide * (v9 + v12)];
		v30 = v9;
		v14 = &face->glyph->bitmap.buffer[(DWORD)v11 * v9];

		do
		{
			v15 = v11;
			v16 = face->glyph->bitmap_left;
			v17 = -face->glyph->bitmap_left;

			if (v16 >= 0)
				v17 = 0;

			if (rgbaWide < (signed int)((char *)v11 + v16))
				v15 = (int)((char *)v11 + rgbaWide - (DWORD)((char *)v11 + v16));

			if (v17 < (signed int)v15)
			{
				unsigned char *dst = (unsigned char *)&v13[4 * (v17 + v16)];

				do
				{
					dst[0] = 0xFF;
					dst[1] = 0xFF;
					dst[2] = 0xFF;
					dst[3] = *(BYTE *)(v14 + v17++);
					dst += 4;
				}
				while (v17 != v15);
			}

			v14 += (int)v11;
			v30++;
			v13 += 4 * rgbaWide;
		}
		while (v30 != v32);
	}

	if (m_iDropShadowOffset)
		ApplyDropShadowToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, wide, tall, rgba);

	if (m_iOutlineSize)
		ApplyOutlineToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, wide, tall, rgba);

	ApplyGaussianBlurToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, rgba);
	ApplyScanlineEffectToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, rgba);
	ApplyRotaryEffectToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, rgba);
}

void CFreeTypeFont::ApplyRotaryEffectToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba)
{
	if (!m_bRotary)
		return;

	int y = rgbaTall * 0.5;
	unsigned char *line = &rgba[(rgbaX + ((y + rgbaY) * rgbaWide)) * 4];

	for (int x = 0; x < rgbaWide; x++, line += 4)
	{
		line[0] = 127;
		line[1] = 127;
		line[2] = 127;
		line[3] = 255;
	}
}

void CFreeTypeFont::ApplyScanlineEffectToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba)
{
	if (m_iScanLines < 2)
		return;

	for (int y = 0; y < rgbaTall; y++)
	{
		if (y % m_iScanLines == 0)
			continue;

		unsigned char *line = &rgba[(rgbaX + ((y + rgbaY) * rgbaWide)) * 4];

		for (int x = 0; x < rgbaWide; x++, line += 4)
		{
			line[0] *= 0.7;
			line[1] *= 0.7;
			line[2] *= 0.7;
		}
	}
}

void CFreeTypeFont::ApplyDropShadowToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba)
{
	for (int y = charTall - 1; y >= m_iDropShadowOffset; y--)
	{
		for (int x = charWide - 1; x >= m_iDropShadowOffset; x--)
		{
			unsigned char *dest = &rgba[(x + rgbaX + ((rgbaY + y) * rgbaWide)) * 4];

			if (dest[3] == 0)
			{
				unsigned char *src = &rgba[(x + rgbaX - m_iDropShadowOffset + ((rgbaY + y - m_iDropShadowOffset) * rgbaWide)) * 4];
				dest[0] = 0;
				dest[1] = 0;
				dest[2] = 0;
				dest[3] = src[3];
			}
		}
	}
}

void CFreeTypeFont::ApplyOutlineToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba)
{
	int x, y;

	for (y = 0; y < charTall; y++)
	{
		for (x = 0; x < charWide; x++)
		{
			unsigned char *src = &rgba[(x + rgbaX + ((rgbaY + y) * rgbaWide)) * 4];

			if (src[3] == 0)
			{
				int shadowX, shadowY;

				for (shadowX = -m_iOutlineSize; shadowX <= m_iOutlineSize; shadowX++)
				{
					for (shadowY = -m_iOutlineSize; shadowY <= m_iOutlineSize; shadowY++)
					{
						if (shadowX == 0 && shadowY == 0)
							continue;

						int testX = shadowX + x;
						int testY = shadowY + y;

						if (testX < 0 || testX >= charWide || testY < 0 || testY >= charTall)
							continue;

						unsigned char *test = &rgba[(testX + rgbaX + ((rgbaY + testY) * rgbaWide)) * 4];

						if (test[0] != 0 && test[1] != 0 && test[2] != 0 && test[3] != 0)
						{
							src[0] = 0;
							src[1] = 0;
							src[2] = 0;
							src[3] = 255;
						}
					}
				}
			}
		}
	}
}

void CFreeTypeFont::ApplyGaussianBlurToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba)
{
	if (!m_pGaussianDistribution)
		return;

	unsigned char *src = (unsigned char *)_alloca(rgbaWide * rgbaTall * 4);
	memcpy(src, rgba, rgbaWide * rgbaTall * 4);
	unsigned char *dest = rgba;

	for (int y = 0; y < rgbaTall; y++)
	{
		for (int x = 0; x < rgbaWide; x++)
		{
			GetBlurValueForPixel(src, m_iBlur, m_pGaussianDistribution, x, y, rgbaWide, rgbaTall, dest);
			dest += 4;
		}
	}
}

void CFreeTypeFont::GetBlurValueForPixel(unsigned char *src, int blur, float *gaussianDistribution, int srcX, int srcY, int srcWide, int srcTall, unsigned char *dest)
{
	int r = 0, g = 0, b = 0, a = 0;
	float accum = 0.0f;
	int maxX = min(srcX + blur, srcWide);
	int minX = max(srcX - blur, 0);

	for (int x = minX; x <= maxX; x++)
	{
		int maxY = min(srcY + blur, srcTall - 1);
		int minY = max(srcY - blur, 0);

		for (int y = minY; y <= maxY; y++)
		{
			unsigned char *srcPos = src + ((x + (y * srcWide)) * 4);
			float weight = gaussianDistribution[x - srcX + blur];
			float weight2 = gaussianDistribution[y - srcY + blur];
			accum += (srcPos[0] * (weight * weight2));
		}
	}

	r = g = b = a = (int)accum;

	int pixelCount = 1;
	dest[0] = min(r / pixelCount, 255);
	dest[1] = min(g / pixelCount, 255);
	dest[2] = min(b / pixelCount, 255);
	dest[3] = min(a / pixelCount, 255);
	dest[3] = 255;
}

bool CFreeTypeFont::IsEqualTo(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	if (!stricmp(windowsFontName, m_szName) && m_iTall == tall && m_iWeight == weight && m_iBlur == blur && m_iFlags == flags)
		return true;

	return false;
}

bool CFreeTypeFont::IsValid(void)
{
	if (m_szName[0])
		return true;

	return false;
}

void CFreeTypeFont::SetAsActiveFont(HDC hdc)
{
}

void CFreeTypeFont::GetCharABCWidths(int ch, int &a, int &b, int &c)
{
	FT_UInt index = FT_Get_Char_Index(face, ch);

	if (!index)
	{
		if (m_pBaseFont)
			m_pBaseFont->GetCharABCWidths(ch, a, b, c);

		return;
	}

	abc_cache_t finder;
	finder.wch = (wchar_t)ch;
	unsigned short i = m_ExtendedABCWidthsCache.Find(finder);

	if (m_ExtendedABCWidthsCache.IsValidIndex(i))
	{
		a = m_ExtendedABCWidthsCache[i].abc.a;
		b = m_ExtendedABCWidthsCache[i].abc.b;
		c = m_ExtendedABCWidthsCache[i].abc.c;
		return;
	}

	int v9; // ebx@21
	signed int v11; // eax@22
	int v12; // edx@22
	signed int v13; // ecx@22
	int v14; // eax@24
	FT_GlyphSlot v15; // ecx@26
	signed int v16; // edx@26
	signed int v17; // eax@26
	signed int v18; // esi@26
	int v19; // ebp@28
	int v20; // esi@28
	int v21; // ecx@30
	__int16 v22; // bx@30
	signed int v23; // edx@30
	int v24; // eax@30
	signed int v25; // esi@30
	char v26; // [sp+15h] [bp-37h]@28
	char v27; // [sp+16h] [bp-36h]@28
	int v28; // [sp+18h] [bp-34h]@26
	int v29; // [sp+1Ch] [bp-30h]@26
	int v31; // [sp+24h] [bp-28h]@1

	c = 0;
	b = 0;
	a = 0;
	v9 = m_iBlur;

	if (FT_Load_Char(face, ch, 0))
	{
		BYTE2(v31) = 0;
		v11 = ((unsigned __int32)face->bbox.xMax >> 31) + face->bbox.xMax;
		LOWORD(v12) = 0;
		v13 = v11 >> 7;

		if (v11 & 0x40)
			v12 = ((unsigned __int64)v11 >> 32) | 1;

		v14 = m_iOutlineSize;
		BYTE3(v31) = 0;
		LOWORD(v31) = v13 + (WORD)v12 + (WORD)m_iDropShadowOffset + 2 * (v14 + (WORD)v9);
	}
	else
	{
		LOBYTE(v29) = 0;
		v15 = face->glyph;
		v16 = v15->metrics.horiBearingX;
		v17 = v15->metrics.width;
		v18 = v16 >> 6;
		v28 = v15->metrics.horiAdvance;

		if (v16 & 0x20)
			v29 = (v16 >> 31) | 1;

		v19 = m_iOutlineSize;
		v27 = v9;
		v26 = v18 + (BYTE)v29 - (BYTE)v9 - v19;
		LOWORD(v20) = 0;

		if (v17 & 0x20)
			v20 = (v17 >> 31) | 1;

		v21 = m_iDropShadowOffset;
		v22 = (v17 >> 6) + (WORD)v20 + (WORD)v21 + 2 * (v19 + (WORD)v9);
		v23 = v28 - v16 - v17;
		LOBYTE(v24) = 0;
		v25 = v23 >> 6;

		if (v23 & 0x20)
			v24 = (v23 >> 31) | 1;

		v31 = (unsigned __int16)((char)(v25 + (BYTE)v24 - v27 - (BYTE)v21 - (unsigned __int8)m_iOutlineSize) + v26 + v22);
	}

	a = SBYTE2(v31);
	b = (signed __int16)v31;
	c = SBYTE3(v31);

	finder.abc.a = a;
	finder.abc.b = b;
	finder.abc.c = c;

	m_ExtendedABCWidthsCache.Insert(finder);
}

int CFreeTypeFont::GetHeight(void)
{
	assert(IsValid());
	return m_iHeight;
}

int CFreeTypeFont::GetMaxCharWidth(void)
{
	assert(IsValid());
	return m_iMaxCharWidth;
}

int CFreeTypeFont::GetFlags(void)
{
	return m_iFlags;
}

bool CFreeTypeFont::GetUnderlined(void)
{
	return m_bUnderlined;
}

int CFreeTypeFont::GetAscent(void)
{
	return m_iAscent;
}

#pragma optimize("", on)
#pragma warning(pop)