#ifndef FREETYPEFONT_H
#define FREETYPEFONT_H

#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE
#include <windows.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H

#include "tier1/utlrbtree.h"

#ifdef GetCharABCWidths
#undef GetCharABCWidths
#endif

class CWin32Font;

class CFreeTypeFont : public CWin32Font
{
public:
	struct abc_t
	{
		char a;
		char b;
		char c;
	};

	struct abc_cache_t
	{
		abc_t abc;
		wchar_t wch;
	};

	enum { ABCWIDTHS_CACHE_SIZE = 256 };

public:
	CFreeTypeFont(void);
	~CFreeTypeFont(void);

public:
	bool Create(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	void GetCharRGBA(int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
	bool IsEqualTo(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	bool IsValid(void);
	void GetCharABCWidths(int ch, int &a, int &b, int &c);
	void SetAsActiveFont(HDC hdc);
	int GetHeight(void);
	int GetMaxCharWidth(void);
	int GetFlags(void);
	const char *GetName(void) { return m_szName; }
	bool GetUnderlined(void);
	int GetAscent(void);

private:
	void ApplyScanlineEffectToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
	void ApplyGaussianBlurToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
	void ApplyDropShadowToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba);
	void ApplyOutlineToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba);
	void ApplyRotaryEffectToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);

private:
	static bool ExtendedABCWidthsCacheLessFunc(const abc_cache_t &lhs, const abc_cache_t &rhs);
	static inline void GetBlurValueForPixel(unsigned char *src, int blur, float *gaussianDistribution, int x, int y, int wide, int tall, unsigned char *dest);

public:
	char m_szName[32];
	int m_iTall;
	int m_iWeight;
	int m_iFlags;
	bool m_bAntiAliased;
	bool m_bRotary;
	bool m_bAdditive;
	int m_iDropShadowOffset;
	bool m_bUnderlined;
	int m_iOutlineSize;
	int m_iHeight;
	int m_iMaxCharWidth;
	int m_iAscent;
	CUtlRBTree<abc_cache_t> m_ExtendedABCWidthsCache;
	int m_rgiBitmapSize[2];
	unsigned char *m_pBuf;
	int m_iScanLines;
	int m_iBlur;
	float *m_pGaussianDistribution;
	FT_Face face;
	int m_iFxpHeight;
	CWin32Font *m_pBaseFont;

public:
	static bool ms_bSetFriendlyNameCacheLessFunc;
};

#endif