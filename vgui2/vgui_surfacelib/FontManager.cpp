#include <locale.h>
#include "FontManager.h"
#include <tier0/dbg.h>
#include <vgui/ISurface.h>
#include "vgui_internal.h"
#include "Encode.h"

typedef BOOL(WINAPI *fnGetFontResourceInfoW)(LPCWSTR, DWORD *, LPWSTR, DWORD);
static fnGetFontResourceInfoW GetFontResourceInfo;

static CFontManager s_FontManager;

CFontManager &FontManager(void)
{
	return s_FontManager;
}

#include <shlobj.h>

char *CrnGetSpecialFolder(int nFolder)
{
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	static char szDir[MAX_PATH];

	if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, nFolder, &pidl)))
		{
			if (SHGetPathFromIDList(pidl, szDir))
				strcat(szDir, "\\");

			pShellMalloc->Free(pidl);
		}

		pShellMalloc->Release();
	}

	return szDir;
}

CFontManager::CFontManager(void)
{
	GetFontResourceInfo = (fnGetFontResourceInfoW)GetProcAddress(LoadLibrary("GDI32.DLL"), "GetFontResourceInfoW");

	m_FontAmalgams.EnsureCapacity(100);
	m_FontAmalgams.AddToTail();
	m_Win32Fonts.EnsureCapacity(100);

	m_BaseFontCache.RemoveAll();
	m_GetFontFileCache.RemoveAll();
	m_CustomFontCache.RemoveAll();

	strcpy(m_szDefaultFont, GetIconTitleFontName());
	wcscpy(m_szFontPath, ANSIToUnicode(CrnGetSpecialFolder(CSIDL_FONTS)));

	setlocale(LC_CTYPE, "");
	setlocale(LC_TIME, "");
	setlocale(LC_COLLATE, "");
	setlocale(LC_MONETARY, "");

	FT_Init_FreeType(&m_Library);
}

CFontManager::~CFontManager(void)
{
	ClearAllFonts();
	FT_Done_FreeType(m_Library);
}

void CFontManager::SetLanguage(const char *lang)
{
	strncpy(m_szLanguage, lang, sizeof(m_szLanguage) - 1);
	m_szLanguage[sizeof(m_szLanguage) - 1] = 0;
}

void CFontManager::ClearAllFonts(void)
{
	for (int i = 0; i < m_Win32Fonts.Count(); i++)
		delete m_Win32Fonts[i];

	m_Win32Fonts.RemoveAll();
}

extern vgui::HFont(__fastcall *g_pfnCreateFont)(void *pthis, int);

vgui::HFont CFontManager::CreateFont(void)
{
	int i = m_FontAmalgams.AddToTail();
	return i;
}

bool CFontManager::AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	if (m_FontAmalgams[font].GetCount() > 0)
		return false;

	CWin32Font *winFont = CreateOrFindWin32Font(windowsFontName, tall, weight, blur, scanlines, flags);

	if (!(flags & vgui::ISurface::FONTFLAG_UNLIMITED))
	{
		do
		{
			if (IsFontForeignLanguageCapable(windowsFontName))
			{
				if (winFont)
				{
					m_FontAmalgams[font].AddFont(winFont, 0x0, 0xFFFF);
					return true;
				}
			}
			else
			{
				const char *localizedFontName = GetForeignFallbackFontName();

				if (winFont && !stricmp(localizedFontName, windowsFontName))
				{
					m_FontAmalgams[font].AddFont(winFont, 0x0, 0xFFFF);
					return true;
				}

				CWin32Font *asianFont = CreateOrFindWin32Font(localizedFontName, tall, weight, blur, scanlines, flags);

				if (winFont && asianFont)
				{
					m_FontAmalgams[font].AddFont(winFont, 0x0, 0xFF);
					m_FontAmalgams[font].AddFont(asianFont, 0x100, 0xFFFF);
					return true;
				}
				else if (asianFont)
				{
					m_FontAmalgams[font].AddFont(asianFont, 0x0, 0xFFFF);
					return true;
				}
			}
		} while (NULL != (windowsFontName = GetFallbackFontName(windowsFontName)));
	}
	else
	{
		if (winFont)
			m_FontAmalgams[font].AddFont(winFont, 0x0, 0xFFFF);

		return true;
	}

	return false;
}

HFont CFontManager::GetFontByName(const char *name)
{
	for (int i = 1; i < m_FontAmalgams.Count(); i++)
	{
		if (!stricmp(name, m_FontAmalgams[i].Name()))
			return i;
	}

	return 0;
}

CWin32Font *CFontManager::GetFontForChar(HFont font, wchar_t wch)
{
	return m_FontAmalgams[font].GetFontForChar(wch);
}

void CFontManager::GetCharABCwide(HFont font, int ch, int &a, int &b, int &c)
{
	CWin32Font *winFont = m_FontAmalgams[font].GetFontForChar(ch);

	if (winFont)
	{
		winFont->GetCharABCWidths(ch, a, b, c);
	}
	else
	{
		a = c = 0;
		b = m_FontAmalgams[font].GetFontMaxWidth();
	}
}

int CFontManager::GetFontTall(HFont font)
{
	return m_FontAmalgams[font].GetFontHeight();
}

int CFontManager::GetFontAscent(HFont font, wchar_t wch)
{
	CWin32Font *winFont = m_FontAmalgams[font].GetFontForChar(wch);

	if (winFont)
		return winFont->GetAscent();
	else
		return 0;
}

bool CFontManager::IsFontAdditive(HFont font)
{
	return (m_FontAmalgams[font].GetFlags(0) & vgui::ISurface::FONTFLAG_ADDITIVE) ? true : false;
}

int CFontManager::GetCharacterWidth(vgui::HFont font, int ch)
{
	if (iswprint(ch))
	{
		int a, b, c;
		GetCharABCwide(font, ch, a, b, c);
		return (a + b + c);
	}

	return 0;
}

void CFontManager::GetTextSize(vgui::HFont font, const wchar_t *text, int &wide, int &tall)
{
	wide = 0;
	tall = 0;

	if (!text)
		return;

	tall = GetFontTall(font);

	int xx = 0;

	for (int i = 0;; i++)
	{
		wchar_t ch = text[i];

		if (ch == 0)
		{
			break;
		}
		else if (ch == '\n')
		{
			tall += GetFontTall(font);
			xx = 0;
		}
		else if (ch == '&')
		{
			// underscore character, so skip
		}
		else
		{
			xx += GetCharacterWidth(font, ch);

			if (xx > wide)
				wide = xx;
		}
	}
}

struct FallbackFont_t
{
	const char *font;
	const char *fallbackFont;
};

const char *g_szValidAsianFonts[] = { "Marlett", NULL };

FallbackFont_t g_FallbackFonts[] =
{
	{ "Times New Roman", "Courier New" },
	{ "Courier New", "Courier" },
	{ "Verdana", "Arial" },
	{ "Trebuchet MS", "Arial" },
	{ "Tahoma", NULL },
	{ NULL, "Tahoma" },
};

bool CFontManager::IsFontForeignLanguageCapable(const char *windowsFontName)
{
	for (int i = 0; g_szValidAsianFonts[i] != NULL; i++)
	{
		if (!stricmp(g_szValidAsianFonts[i], windowsFontName))
			return true;
	}

	return false;
}

const char *CFontManager::GetFallbackFontName(const char *windowsFontName)
{
	int i;

	for (i = 0; g_FallbackFonts[i].font != NULL; i++)
	{
		if (!stricmp(g_FallbackFonts[i].font, windowsFontName))
			return g_FallbackFonts[i].fallbackFont;
	}

	return g_FallbackFonts[i].fallbackFont;
}

struct Win98ForeignFallbackFont_t
{
	const char *language;
	const char *fallbackFont;
};

Win98ForeignFallbackFont_t g_Win98ForeignFallbackFonts[] =
{
	{ "russian", "system" },
	{ "japanese", "win98japanese" },
	{ "thai", "system" },
	{ NULL, "Tahoma" },
};

extern bool s_bSupportsUnicode;

const char *CFontManager::GetForeignFallbackFontName(void)
{
	if (s_bSupportsUnicode)
		return "Tahoma";

	int i;

	for (i = 0; g_Win98ForeignFallbackFonts[i].language != NULL; i++)
	{
		if (!stricmp(g_Win98ForeignFallbackFonts[i].language, m_szLanguage))
			return g_Win98ForeignFallbackFonts[i].fallbackFont;
	}

	return g_Win98ForeignFallbackFonts[i].fallbackFont;
}

bool CFontManager::GetFontUnderlined(vgui::HFont font)
{
	return m_FontAmalgams[font].GetUnderlined();
}

CWin32Font *CFontManager::CreateOrFindWin32Font(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	CWin32Font *winFont = NULL;

	for (int i = 0; i < m_Win32Fonts.Count(); i++)
	{
		if (m_Win32Fonts[i]->IsEqualTo(windowsFontName, tall, weight, blur, scanlines, flags))
		{
			winFont = m_Win32Fonts[i];
			break;
		}
	}

	if (!winFont)
	{
		int i = m_Win32Fonts.AddToTail();

		if (flags & vgui::ISurface::FONTFLAG_FREETYPE)
			m_Win32Fonts[i] = new CFreeTypeFont();
		else
			m_Win32Fonts[i] = new CWin32Font();

		if (m_Win32Fonts[i]->Create(windowsFontName, tall, weight, blur, scanlines, flags))
		{
			winFont = m_Win32Fonts[i];
		}
		else
		{
			delete m_Win32Fonts[i];
			m_Win32Fonts.Remove(i);
			return NULL;
		}
	}

	return winFont;
}

static LOGFONTW logFont[128];

bool CFontManager::AddCustomFontFile(const char *fontFileName)
{
	for (int i = 0; i < m_CustomFontCache.Size(); i++)
	{
		if (!strcmp(m_CustomFontCache[i].fontFilePath, fontFileName))
			return true;
	}

	if (!(::AddFontResource(fontFileName) > 0))
		return false;

	const char *facename = GetFontFaceName(fontFileName);

	if (!facename)
		return false;

	int i = m_CustomFontCache.AddToTail();
	strcpy(m_CustomFontCache[i].fontName, facename);
	strcpy(m_CustomFontCache[i].fontFilePath, fontFileName);
	return true;
}

const char *CFontManager::GetCustomFontFilePath(const char *fontName)
{
	for (int i = 0; i < m_CustomFontCache.Size(); i++)
	{
		if (!strcmp(m_CustomFontCache[i].fontName, fontName))
			return m_CustomFontCache[i].fontFilePath;
	}

	return NULL;
}

const char *CFontManager::GetIconTitleFontName(void)
{
	LOGFONT lf;
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
	return lf.lfFaceName;
}

const char *CFontManager::GetFontFilePath(const char *fontName, BOOL isBold, BOOL isItalic)
{
	const char *fontPath = FontManager().GetCustomFontFilePath(fontName);

	if (fontPath)
		return fontPath;

	wchar_t wFontName[64];
	wcscpy(wFontName, ANSIToUnicode(fontName));

	for (int i = 0; i < m_GetFontFileCache.Count(); i++)
	{
		if (!strcmp(m_GetFontFileCache[i].fontName, fontName) && m_GetFontFileCache[i].isBold == isBold && m_GetFontFileCache[i].isItalic == isItalic)
			return m_GetFontFileCache[i].fontFilePath;
	}

	if (!stricmp(fontName, "Marlett"))
		fontName = "Marlett.ttf";

	wchar_t fontFileName[MAX_PATH];
	swprintf(fontFileName, L"%s*", m_szFontPath);

	wchar_t foundName[MAX_PATH];
	BOOL fontFound = FALSE;
	static wchar_t fullPath[MAX_PATH];
	DWORD dwDummy;

	WIN32_FIND_DATAW findData;
	HANDLE searchHandle = FindFirstFileW(fontFileName, &findData);

	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			if (findData.cFileName[0] == '.')
				continue;

			DWORD dwSize = 0;
			DWORD dwCount;
			swprintf(fontFileName, L"%s%s", m_szFontPath, findData.cFileName);

			if (GetFontResourceInfo(fontFileName, &dwSize, NULL, 2) == FALSE)
				continue;

			dwCount = dwSize / sizeof(LOGFONTW);
			memset(logFont, 0, dwSize);

			if (GetFontResourceInfo(findData.cFileName, &dwSize, (LPWSTR)logFont, 2) == FALSE)
				continue;

			for (DWORD i = 0; i < dwCount; i++)
			{
				BOOL isThis = !wcscmp(logFont[i].lfFaceName, wFontName);

				if (isThis)
				{
					if (!fontFound)
					{
						wcscpy(foundName, findData.cFileName);
						fontFound = TRUE;
					}
				}

				if ((isBold == -1 || (isBold != -1 && logFont[i].lfWeight == 700)) && (isItalic == -1 || (isItalic != -1 && logFont[i].lfItalic)) && isThis)
				{
					wcscpy(foundName, findData.cFileName);
					goto done;
				}
			}
		} while (FindNextFileW(searchHandle, &findData));
	}

	if (fontFound)
	{
	done:
		GetFontResourceInfo(foundName, &dwDummy, fullPath, 4);
		int j = m_GetFontFileCache.AddToTail();
		strcpy(m_GetFontFileCache[j].fontName, fontName);
		m_GetFontFileCache[j].isBold = isBold;
		m_GetFontFileCache[j].isItalic = isItalic;
		strcpy(m_GetFontFileCache[j].fontFilePath, UnicodeToANSI(fullPath));
		FindClose(searchHandle);
		return m_GetFontFileCache[j].fontFilePath;
	}

	FindClose(searchHandle);
	return NULL;
}

const char *CFontManager::GetFontFaceName(const char *fontPath)
{
	static wchar_t _fontPath[MAX_PATH];
	wcscpy(_fontPath, ANSIToUnicode(fontPath));

	DWORD dwSize = 0;
	DWORD dwCount;
	DWORD dwError = 0;

	if (GetFontResourceInfo(_fontPath, &dwSize, NULL, 2) == FALSE)
	{
		Assert((dwError = GetLastError()) == 0);
		return NULL;
	}

	dwCount = dwSize / sizeof(LOGFONTW);
	memset(logFont, 0, dwSize);

	if (GetFontResourceInfo(_fontPath, &dwSize, (LPWSTR)logFont, 2) == FALSE)
	{
		Assert((dwError = GetLastError()) == 0);
		return NULL;
	}

	return UnicodeToANSI(logFont[0].lfFaceName);
}

void CFontManager::AddBaseFont(CWin32Font *font, int tall, int weight, int blur, int scanlines, int flags)
{
	int i = m_BaseFontCache.AddToTail();

	m_BaseFontCache[i].font = font;
	m_BaseFontCache[i].tall = tall;
	m_BaseFontCache[i].weight = weight;
	m_BaseFontCache[i].blur = blur;
	m_BaseFontCache[i].scanlines = scanlines;
	m_BaseFontCache[i].flags = flags;
}

CWin32Font *CFontManager::GetBaseFont(int tall, int weight, int blur, int scanlines, int flags)
{
	for (int i = 0; i < m_BaseFontCache.Count(); i++)
	{
		baseFont_t *font = &m_BaseFontCache[i];

		if (font->tall == tall && font->weight == weight && font->blur == blur && font->scanlines == scanlines && font->flags == flags)
			return m_BaseFontCache[i].font;
	}

	return NULL;
}

const char *CFontManager::GetDefaultFontName(void)
{
	return m_szDefaultFont;
}

FT_Library CFontManager::GetFreeTypeLibrary(void)
{
	return m_Library;
}