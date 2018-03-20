#include <vgui/VGUI.h>
#include "FontAmalgam.h"

#ifdef CreateFont
#undef CreateFont
#endif

using vgui::HFont;

class CWin32Font;

typedef struct baseFont_s
{
	int tall;
	int weight;
	int blur;
	int scanlines;
	int flags;
	CWin32Font *font;
}
baseFont_t;

typedef struct
{
	char fontName[MAX_PATH];
	BOOL isBold;
	BOOL isItalic;
	char fontFilePath[MAX_PATH];
}
fontCache_t;

class CWin32Font;

class CFontManager
{
public:
	CFontManager(void);
	~CFontManager(void);

public:
	void ClearAllFonts(void);
	HFont CreateFont(void);
	bool AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
	HFont GetFontByName(const char *name);
	void GetCharABCwide(HFont font, int ch, int &a, int &b, int &c);
	int GetFontTall(HFont font);
	int GetFontAscent(HFont font, wchar_t wch);
	int GetCharacterWidth(HFont font, int ch);
	void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall);
	CWin32Font *GetFontForChar(HFont, wchar_t wch);
	bool IsFontAdditive(HFont font);
	void SetLanguage(const char *lang);
	bool IsFontForeignLanguageCapable(const char *windowsFontName);
	const char *GetFallbackFontName(const char *windowsFontName);
	const char *GetForeignFallbackFontName(void);
	bool GetFontUnderlined(HFont font);
	CWin32Font *CreateOrFindWin32Font(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	bool AddCustomFontFile(const char *fontFileName);
	const char *GetCustomFontFilePath(const char *fontName);
	const char *GetDefaultFontName(void);
	FT_Library GetFreeTypeLibrary(void);
	const char *GetFontFilePath(const char *fontName, BOOL isBold = -1, BOOL isItalic = -1);
	const char *GetFontFaceName(const char *fontPath);
	void AddBaseFont(CWin32Font *font, int tall, int weight, int blur, int scanlines, int flags);
	CWin32Font *GetBaseFont(int tall, int weight, int blur, int scanlines, int flags);

private:
	const char *GetIconTitleFontName(void);

private:
	CUtlVector<CFontAmalgam> m_FontAmalgams;
	CUtlVector<CWin32Font *> m_Win32Fonts;
	char m_szLanguage[64];
	FT_Library m_Library;
	CUtlVector<fontCache_t> m_CustomFontCache;
	CUtlVector<fontCache_t> m_GetFontFileCache;
	CUtlVector<baseFont_t> m_BaseFontCache;
	char m_szDefaultFont[64];
	wchar_t m_szFontPath[MAX_PATH];
};

extern CFontManager &FontManager(void);