#include <metahook.h>
#include "vgui_controls\Controls.h"
#include "FontTextureCache.h"
#include <IEngineSurface.h>
#include "Color.h"
#include "plugins.h"
#include "configs.h"

#include "tier1/utlvector.h"
#include "tier1/utlrbtree.h"
#include "tier1/UtlDict.h"

extern IEngineSurface *staticSurface;

using namespace vgui;

HFont g_hCurrentFont;
int g_iCurrentTextR, g_iCurrentTextG, g_iCurrentTextB, g_iCurrentTextA;

class IHTMLResponses;
class IHTMLChromeController;

void (__fastcall *g_pfnSurface_Shutdown)(void *pthis, int);
void (__fastcall *g_pfnDrawSetTextFont)(void *pthis, int, HFont font);
void (__fastcall *g_pfnDrawUnicodeChar)(void *pthis, int, wchar_t wch);
void (__fastcall *g_pfnDrawUnicodeCharAdd)(void *pthis, int, wchar_t wch);
bool (__fastcall *g_pfnAddGlyphSetToFont)(void *pthis, int, HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
int (__fastcall *g_pfnGetFontTall)(void *pthis, int, HFont font);
void (__fastcall *g_pfnGetCharABCwide)(void *pthis, int, HFont font, int ch, int &a, int &b, int &c);
int (__fastcall *g_pfnGetCharacterWidth)(void *pthis, int, HFont font, int ch);
void (__fastcall *g_pfnGetTextSize)(void *pthis, int, HFont font, const wchar_t *text, int &wide, int &tall);
int (__fastcall *g_pfnGetFontAscent)(void *pthis, int, HFont font, wchar_t wch);
HFont (__fastcall *g_pfnCreateFont)(void *pthis, int);
void (__fastcall *g_pfnDrawSetTextColor)(void *pthis, int, int r, int g, int b, int a);
void (__fastcall *g_pfnDrawSetTextColor2)(void *pthis, int, Color col);
void (__fastcall *g_pfnSetAllowHTMLJavaScript)(void *pthis, int, bool state);
void (__fastcall *g_pfnSetLanguage)(void *pthis, int, const char *pchLang);
const char *(__fastcall *g_pfnGetLanguage)(void *pthis, int);
void (__fastcall *g_pfnDeleteTextureByID)(void *pthis, int, int id);
void (__fastcall *g_pfnDrawUpdateRegionTextureBGRA)(void *pthis, int, int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall);
void (__fastcall *g_pfnDrawSetTextureBGRA)(void *pthis, int, int id, const unsigned char *rgba, int wide, int tall);
void (__fastcall *g_pfnCreateBrowser)(void *pthis, int, VPANEL panel, IHTMLResponses *pBrowser, bool bPopupWindow, const char *pchUserAgentIdentifier);
void (__fastcall *g_pfnRemoveBrowser)(void *pthis, int, VPANEL panel, IHTMLResponses *pBrowser);
IHTMLChromeController *(__fastcall *g_pfnAccessChromeHTMLController)(void *pthis, int);
void (__fastcall *g_pfnsetFullscreenMode)(void *pthis, int, int wide, int tall, int bpp);
void (__fastcall *g_pfnsetWindowedMode)(void *pthis, int);
void (__fastcall *g_pfnSetAsTopMost)(void *pthis, int, bool state);
void (__fastcall *g_pfnSetAsToolBar)(void *pthis, int, bool state);
void (__fastcall *g_pfnPanelRequestFocus)(void *pthis, int, VPANEL panel);
void (__fastcall *g_pfnEnableMouseCapture)(void *pthis, int, bool state);
void (__fastcall *g_pfnDrawPrintChar)(void *pthis, int, int x, int y, int wide, int tall, float s0, float t0, float s1, float t1);
void (__fastcall *g_pfnSetNotifyIcon)(void *pthis, int, Image *image, VPANEL panelToReceiveMessages, const char *text);
bool (__fastcall *g_pfnSetWatchForComputerUse)(void *pthis, int, bool state);
double (__fastcall *g_pfnGetTimeSinceLastUse)(void *pthis, int);
bool (__fastcall *g_pfnVGUI2MouseControl)(void *pthis, int);
void (__fastcall *g_pfnSetVGUI2MouseControl)(void *pthis, int, bool state);
void (__fastcall *g_pfnSurfaceSetCursorPos)(void *pthis, int, int x, int y);

class CSurface : public ISurface
{
public:
	virtual void Shutdown(void);
	virtual void RunFrame(void);
	virtual VPANEL GetEmbeddedPanel(void);
	virtual void SetEmbeddedPanel(VPANEL pPanel);
	virtual void PushMakeCurrent(VPANEL panel, bool useInsets);
	virtual void PopMakeCurrent(VPANEL panel);
	virtual void DrawSetColor(int r, int g, int b, int a);
	virtual void DrawSetColor(Color col);
	virtual void DrawFilledRect(int x0, int y0, int x1, int y1);
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1);
	virtual void DrawLine(int x0, int y0, int x1, int y1);
	virtual void DrawPolyLine(int *px, int *py, int numPoints);
	virtual void DrawSetTextFont(HFont font);
	virtual void DrawSetTextColor(int r, int g, int b, int a);
	virtual void DrawSetTextColor(Color col);
	virtual void DrawSetTextPos(int x, int y);
	virtual void DrawGetTextPos(int &x, int &y);
	virtual void DrawPrintText(const wchar_t *text, int textLen);
	virtual void DrawUnicodeChar(wchar_t wch);
	virtual void DrawUnicodeCharAdd(wchar_t wch);
	virtual void DrawFlushText(void);
	virtual IHTML *CreateHTMLWindow(IHTMLEvents *events, VPANEL context);
	virtual void PaintHTMLWindow(IHTML *htmlwin);
	virtual void DeleteHTMLWindow(IHTML *htmlwin);
	virtual void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload);
	virtual void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload);
	virtual void DrawSetTexture(int id);
	virtual void DrawGetTextureSize(int id, int &wide, int &tall);
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1);
	virtual bool IsTextureIDValid(int id);
	virtual int CreateNewTextureID(bool procedural = false);
	virtual void GetScreenSize(int &wide, int &tall);
	virtual void SetAsTopMost(VPANEL panel, bool state);
	virtual void BringToFront(VPANEL panel);
	virtual void SetForegroundWindow(VPANEL panel);
	virtual void SetPanelVisible(VPANEL panel, bool state);
	virtual void SetMinimized(VPANEL panel, bool state);
	virtual bool IsMinimized(VPANEL panel);
	virtual void FlashWindow(VPANEL panel, bool state);
	virtual void SetTitle(VPANEL panel, const wchar_t *title);
	virtual void SetAsToolBar(VPANEL panel, bool state);
	virtual void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true);
	virtual void SwapBuffers(VPANEL panel);
	virtual void Invalidate(VPANEL panel);
	virtual void SetCursor(HCursor cursor);
	virtual bool IsCursorVisible(void);
	virtual void ApplyChanges(void);
	virtual bool IsWithin(int x, int y);
	virtual bool HasFocus(void);
	virtual bool SupportsFeature(SurfaceFeature_e feature);
	virtual void RestrictPaintToSinglePanel(VPANEL panel);
	virtual void SetModalPanel(VPANEL panel);
	virtual VPANEL GetModalPanel(void);
	virtual void UnlockCursor(void);
	virtual void LockCursor(void);
	virtual void SetTranslateExtendedKeys(bool state);
	virtual VPANEL GetTopmostPopup(void);
	virtual void SetTopLevelFocus(VPANEL panel);
	virtual HFont CreateFont(void);
	virtual bool AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
	virtual bool AddCustomFontFile(const char *fontFileName);
	virtual int GetFontTall(HFont font);
	virtual void GetCharABCwide(HFont font, int ch, int &a, int &b, int &c);
	virtual int GetCharacterWidth(HFont font, int ch);
	virtual void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall);
	virtual VPANEL GetNotifyPanel(void);
	virtual void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text);
	virtual void PlaySound(const char *fileName);
	virtual int GetPopupCount(void);
	virtual VPANEL GetPopup(int index);
	virtual bool ShouldPaintChildPanel(VPANEL childPanel);
	virtual bool RecreateContext(VPANEL panel);
	virtual void AddPanel(VPANEL panel);
	virtual void ReleasePanel(VPANEL panel);
	virtual void MovePopupToFront(VPANEL panel);
	virtual void MovePopupToBack(VPANEL panel);
	virtual void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false);
	virtual void PaintTraverse(VPANEL panel);
	virtual void EnableMouseCapture(VPANEL panel, bool state);
	virtual void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall);
	virtual void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall);
	virtual void GetProportionalBase(int &width, int &height);
	virtual void CalculateMouseVisible(void);
	virtual bool NeedKBInput(void);
	virtual bool HasCursorPosFunctions(void);
	virtual void SurfaceGetCursorPos(int &x, int &y);
	virtual void SurfaceSetCursorPos(int x, int y);
	virtual void DrawTexturedPolygon(int *p, int n);
	virtual int GetFontAscent(HFont font, wchar_t wch);
	virtual void SetAllowHTMLJavaScript(bool state);
	virtual void SetLanguage(const char *pchLang);
	virtual const char *GetLanguage(void);
	virtual void DeleteTextureByID(int id);
	virtual void DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall);
	virtual void DrawSetTextureBGRA(int id, const unsigned char *rgba, int wide, int tall);
	virtual void CreateBrowser(VPANEL panel, IHTMLResponses *pBrowser, bool bPopupWindow, const char *pchUserAgentIdentifier);
	virtual void RemoveBrowser(VPANEL panel, IHTMLResponses *pBrowser);
	virtual IHTMLChromeController *AccessChromeHTMLController(void);
	virtual void setFullscreenMode(int wide, int tall, int bpp);
	virtual void setWindowedMode(void);
	virtual void SetAsTopMost(bool state);
	virtual void SetAsToolBar(bool state);
	virtual void PanelRequestFocus(VPANEL panel);
	virtual void EnableMouseCapture(bool state);
	virtual void DrawPrintChar(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1);
	virtual void SetNotifyIcon(Image *image, VPANEL panelToReceiveMessages, const char *text);
	virtual bool SetWatchForComputerUse(bool state);
	virtual double GetTimeSinceLastUse(void);
	virtual bool VGUI2MouseControl(void);
	virtual void SetVGUI2MouseControl(bool state);

public:
	void DrawSetAlphaMultiplier(float alpha);
	float DrawGetAlphaMultiplier(void);
};

CSurface g_Surface;

void CSurface::Shutdown(void)
{
	g_pfnSurface_Shutdown(this, 0);

	FontManager().ClearAllFonts();
}

void CSurface::RunFrame(void)
{
	surface()->RunFrame();
}

VPANEL CSurface::GetEmbeddedPanel(void)
{
	return surface()->GetEmbeddedPanel();
}

void CSurface::SetEmbeddedPanel(VPANEL pPanel)
{
	surface()->SetEmbeddedPanel(pPanel);
}

void CSurface::PushMakeCurrent(VPANEL panel, bool useInsets)
{
	surface()->PushMakeCurrent(panel, useInsets);
}

void CSurface::PopMakeCurrent(VPANEL panel)
{
	surface()->PopMakeCurrent(panel);
}

void CSurface::DrawSetColor(int r, int g, int b, int a)
{
	surface()->DrawSetColor(r, g, b, a);
}

void CSurface::DrawSetColor(Color col)
{
	surface()->DrawSetColor(col);
}

void CSurface::DrawFilledRect(int x0, int y0, int x1, int y1)
{
	surface()->DrawFilledRect(x0, y0, x1, y1);
}

void CSurface::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
	surface()->DrawOutlinedRect(x0, y0, x1, y1);
}

void CSurface::DrawLine(int x0, int y0, int x1, int y1)
{
	surface()->DrawLine(x0, y0, x1, y1);
}

void CSurface::DrawPolyLine(int *px, int *py, int numPoints)
{
	surface()->DrawPolyLine(px, py, numPoints);
}

void CSurface::DrawSetTextFont(HFont font)
{
	g_hCurrentFont = font;
	g_pfnDrawSetTextFont(this, 0, font);
}

void CSurface::DrawSetTextColor(int r, int g, int b, int a)
{
	g_iCurrentTextR = r;
	g_iCurrentTextG = g;
	g_iCurrentTextB = b;
	g_iCurrentTextA = a;

	g_pfnDrawSetTextColor(this, 0, r, g, b, a);
}

void CSurface::DrawSetTextColor(Color col)
{
	int r = col.r();
	int g = col.g();
	int b = col.b();
	int a = col.a();

	g_iCurrentTextR = r;
	g_iCurrentTextG = g;
	g_iCurrentTextB = b;
	g_iCurrentTextA = a;

	g_pfnDrawSetTextColor2(this, 0, col);
}

void CSurface::DrawSetTextPos(int x, int y)
{
	surface()->DrawSetTextPos(x, y);
}

void CSurface::DrawGetTextPos(int &x, int &y)
{
	surface()->DrawGetTextPos(x, y);
}

void CSurface::DrawPrintText(const wchar_t *text, int textLen)
{
	surface()->DrawPrintText(text, textLen);
}

void CSurface::DrawUnicodeChar(wchar_t wch)
{
	if (g_hCurrentFont == INVALID_FONT)
		return;

	int x, y;
	surface()->DrawGetTextPos(x, y);

	int a, b, c;
	FontManager().GetCharABCwide(g_hCurrentFont, wch, a, b, c);

	int rgbaWide, rgbaTall;
	rgbaTall = GetFontTall(g_hCurrentFont);

	if (FontManager().GetFontUnderlined(g_hCurrentFont))
	{
		rgbaWide = c + b + a;
	}
	else
	{
		x += a;
		rgbaWide = b;
	}

	int textureID;
	float *texCoords = NULL;

	if (!g_FontTextureCache.GetTextureForChar(g_hCurrentFont, wch, &textureID, &texCoords))
		return;

	surface()->DrawSetTexture(textureID);
	g_pfnDrawSetTextColor(this, 0, g_iCurrentTextR, g_iCurrentTextG, g_iCurrentTextB, g_iCurrentTextA);
	staticSurface->drawPrintChar(x, y, rgbaWide, rgbaTall, texCoords[0], texCoords[1], texCoords[2], texCoords[3]);
	g_pfnDrawSetTextColor(this, 0, g_iCurrentTextR, g_iCurrentTextG, g_iCurrentTextB, g_iCurrentTextA);
	surface()->DrawSetTextPos(x + b + c, y);
}

void CSurface::DrawUnicodeCharAdd(wchar_t wch)
{
	if (g_hCurrentFont == INVALID_FONT)
		return;

	int x, y;
	surface()->DrawGetTextPos(x, y);

	int a, b, c;
	FontManager().GetCharABCwide(g_hCurrentFont, wch, a, b, c);

	int rgbaWide, rgbaTall;
	rgbaTall = GetFontTall(g_hCurrentFont);

	if (FontManager().GetFontUnderlined(g_hCurrentFont))
	{
		rgbaWide = c + b + a;
	}
	else
	{
		x += a;
		rgbaWide = b;
	}

	int textureID;
	float *texCoords = NULL;

	if (!g_FontTextureCache.GetTextureForChar(g_hCurrentFont, wch, &textureID, &texCoords))
		return;

	surface()->DrawSetTexture(textureID);
	g_pfnDrawSetTextColor(this, 0, g_iCurrentTextR, g_iCurrentTextG, g_iCurrentTextB, g_iCurrentTextA);
	staticSurface->drawPrintCharAdd(x, y, rgbaWide, rgbaTall, texCoords[0], texCoords[1], texCoords[2], texCoords[3]);
	g_pfnDrawSetTextColor(this, 0, g_iCurrentTextR, g_iCurrentTextG, g_iCurrentTextB, g_iCurrentTextA);
	surface()->DrawSetTextPos(x + b + c, y);
}

void CSurface::DrawFlushText(void)
{
	surface()->DrawFlushText();
}

IHTML *CSurface::CreateHTMLWindow(IHTMLEvents *events, VPANEL context)
{
	return surface()->CreateHTMLWindow(events, context);
}

void CSurface::PaintHTMLWindow(IHTML *htmlwin)
{
	surface()->PaintHTMLWindow(htmlwin);
}

void CSurface::DeleteHTMLWindow(IHTML *htmlwin)
{
	surface()->DeleteHTMLWindow(htmlwin);
}

void CSurface::DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload)
{
	surface()->DrawSetTextureFile(id, filename, hardwareFilter, forceReload);
}

void CSurface::DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	surface()->DrawSetTextureRGBA(id, rgba, wide, tall, hardwareFilter, forceReload);
}

void CSurface::DrawSetTexture(int id)
{
	surface()->DrawSetTexture(id);
}

void CSurface::DrawGetTextureSize(int id, int &wide, int &tall)
{
	surface()->DrawGetTextureSize(id, wide, tall);
}

void CSurface::DrawTexturedRect(int x0, int y0, int x1, int y1)
{
	surface()->DrawTexturedRect(x0, y0, x1, y1);
}

bool CSurface::IsTextureIDValid(int id)
{
	return surface()->IsTextureIDValid(id);
}

int CSurface::CreateNewTextureID(bool procedural)
{
	return surface()->CreateNewTextureID(procedural);
}

void CSurface::GetScreenSize(int &wide, int &tall)
{
	surface()->GetScreenSize(wide, tall);
}

void CSurface::SetAsTopMost(VPANEL panel, bool state)
{
	surface()->SetAsTopMost(panel, state);
}

void CSurface::BringToFront(VPANEL panel)
{
	surface()->BringToFront(panel);
}

void CSurface::SetForegroundWindow(VPANEL panel)
{
	surface()->SetForegroundWindow(panel);
}

void CSurface::SetPanelVisible(VPANEL panel, bool state)
{
	surface()->SetPanelVisible(panel, state);
}

void CSurface::SetMinimized(VPANEL panel, bool state)
{
	surface()->SetMinimized(panel, state);
}

bool CSurface::IsMinimized(VPANEL panel)
{
	return surface()->IsMinimized(panel);
}

void CSurface::FlashWindow(VPANEL panel, bool state)
{
	surface()->FlashWindow(panel, state);
}

void CSurface::SetTitle(VPANEL panel, const wchar_t *title)
{
	surface()->SetTitle(panel, title);
}

void CSurface::SetAsToolBar(VPANEL panel, bool state)
{
	surface()->SetAsToolBar(panel, state);
}

void CSurface::CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput, bool kbInput)
{
	surface()->CreatePopup(panel, minimised, showTaskbarIcon, disabled, mouseInput, kbInput);
}

void CSurface::SwapBuffers(VPANEL panel)
{
	surface()->SwapBuffers(panel);
}

void CSurface::Invalidate(VPANEL panel)
{
	surface()->Invalidate(panel);
}

void CSurface::SetCursor(HCursor cursor)
{
	surface()->SetCursor(cursor);
}

bool CSurface::IsCursorVisible(void)
{
	return surface()->IsCursorVisible();
}

void CSurface::ApplyChanges(void)
{
	surface()->ApplyChanges();
}

bool CSurface::IsWithin(int x, int y)
{
	return surface()->IsWithin(x, y);
}

bool CSurface::HasFocus(void)
{
	return surface()->HasFocus();
}

bool CSurface::SupportsFeature(vgui::ISurface::SurfaceFeature_e feature)
{
	return surface()->SupportsFeature(feature);
}

void CSurface::RestrictPaintToSinglePanel(VPANEL panel)
{
	surface()->RestrictPaintToSinglePanel(panel);
}

void CSurface::SetModalPanel(VPANEL panel)
{
	surface()->SetModalPanel(panel);
}

VPANEL CSurface::GetModalPanel(void)
{
	return surface()->GetModalPanel();
}

void CSurface::UnlockCursor(void)
{
	surface()->UnlockCursor();
}

void CSurface::LockCursor(void)
{
	surface()->LockCursor();
}

void CSurface::SetTranslateExtendedKeys(bool state)
{
	surface()->SetTranslateExtendedKeys(state);
}

VPANEL CSurface::GetTopmostPopup(void)
{
	return surface()->GetTopmostPopup();
}

void CSurface::SetTopLevelFocus(VPANEL panel)
{
	surface()->SetTopLevelFocus(panel);
}

HFont CSurface::CreateFont(void)
{
	return FontManager().CreateFont();
}

bool CSurface::AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	return FontManager().AddGlyphSetToFont(font, windowsFontName, tall, weight, blur, scanlines, flags, lowRange, highRange);
	return g_pfnAddGlyphSetToFont(this, 0, font, windowsFontName, tall, weight, blur, scanlines, flags, lowRange, highRange);
}

bool CSurface::AddCustomFontFile(const char *fontFileName)
{
	return surface()->AddCustomFontFile(fontFileName);
}

int CSurface::GetFontTall(HFont font)
{
	return FontManager().GetFontTall(font);
	return g_pfnGetFontTall(this, 0, font);
}

void CSurface::GetCharABCwide(HFont font, int ch, int &a, int &b, int &c)
{
	return FontManager().GetCharABCwide(font, ch, a, b, c);
	return g_pfnGetCharABCwide(this, 0, font, ch, a, b, c);
}

int CSurface::GetCharacterWidth(HFont font, int ch)
{
	return FontManager().GetCharacterWidth(font, ch);
	return g_pfnGetCharacterWidth(this, 0, font, ch);
}

void CSurface::GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall)
{
	return FontManager().GetTextSize(font, text, wide, tall);
	return g_pfnGetTextSize(this, 0, font, text, wide, tall);
}

VPANEL CSurface::GetNotifyPanel(void)
{
	return surface()->GetNotifyPanel();
}

void CSurface::SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text)
{
	surface()->SetNotifyIcon(context, icon, panelToReceiveMessages, text);
}

void CSurface::PlaySound(const char *fileName)
{
	surface()->PlaySound(fileName);
}

int CSurface::GetPopupCount(void)
{
	return surface()->GetPopupCount();
}

VPANEL CSurface::GetPopup(int index)
{
	return surface()->GetPopup(index);
}

bool CSurface::ShouldPaintChildPanel(VPANEL childPanel)
{
	return surface()->ShouldPaintChildPanel(childPanel);
}

bool CSurface::RecreateContext(VPANEL panel)
{
	return surface()->RecreateContext(panel);
}

void CSurface::AddPanel(VPANEL panel)
{
	surface()->AddPanel(panel);
}

void CSurface::ReleasePanel(VPANEL panel)
{
	surface()->ReleasePanel(panel);
}

void CSurface::MovePopupToFront(VPANEL panel)
{
	surface()->MovePopupToFront(panel);
}

void CSurface::MovePopupToBack(VPANEL panel)
{
	surface()->MovePopupToBack(panel);
}

void CSurface::SolveTraverse(VPANEL panel, bool forceApplySchemeSettings)
{
	surface()->SolveTraverse(panel, forceApplySchemeSettings);
}

void CSurface::PaintTraverse(VPANEL panel)
{
	surface()->PaintTraverse(panel);
}

void CSurface::EnableMouseCapture(VPANEL panel, bool state)
{
	surface()->EnableMouseCapture(panel, state);
}

void CSurface::GetWorkspaceBounds(int &x, int &y, int &wide, int &tall)
{
	surface()->GetWorkspaceBounds(x, y, wide, tall);
}

void CSurface::GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall)
{
	surface()->GetAbsoluteWindowBounds(x, y, wide, tall);
}

void CSurface::GetProportionalBase(int &width, int &height)
{
	surface()->GetProportionalBase(width, height);
}

void CSurface::CalculateMouseVisible(void)
{
	surface()->CalculateMouseVisible();
}

bool CSurface::NeedKBInput(void)
{
	return surface()->NeedKBInput();
}

bool CSurface::HasCursorPosFunctions(void)
{
	return surface()->HasCursorPosFunctions();
}

void CSurface::SurfaceGetCursorPos(int &x, int &y)
{
	surface()->SurfaceGetCursorPos(x, y);
}

void CSurface::SurfaceSetCursorPos(int x, int y)
{
	return g_pfnSurfaceSetCursorPos(this, 0, x, y);
}

void CSurface::DrawTexturedPolygon(int *p, int n)
{
	surface()->DrawTexturedPolygon(p, n);
}

int CSurface::GetFontAscent(HFont font, wchar_t wch)
{
	return FontManager().GetFontAscent(font, wch);
	return g_pfnGetFontAscent(this, 0, font, wch);
}

void CSurface::SetAllowHTMLJavaScript(bool state)
{
	g_pfnSetAllowHTMLJavaScript(this, 0, state);
}

void CSurface::SetLanguage(const char *pchLang)
{
	g_pfnSetLanguage(this, 0, pchLang);
}

const char *CSurface::GetLanguage(void)
{
	return g_pfnGetLanguage(this, 0);
}

void CSurface::DeleteTextureByID(int id)
{
	g_pfnDeleteTextureByID(this, 0, id);
}

void CSurface::DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
	g_pfnDrawUpdateRegionTextureBGRA(this, 0, nTextureID, x, y, pchData, wide, tall);
}

void CSurface::DrawSetTextureBGRA(int id, const unsigned char *rgba, int wide, int tall)
{
	g_pfnDrawSetTextureBGRA(this, 0, id, rgba, wide, tall);
}

void CSurface::CreateBrowser(VPANEL panel, IHTMLResponses *pBrowser, bool bPopupWindow, const char *pchUserAgentIdentifier)
{
	g_pfnCreateBrowser(this, 0, panel, pBrowser, bPopupWindow, pchUserAgentIdentifier);
}

void CSurface::RemoveBrowser(VPANEL panel, IHTMLResponses *pBrowser)
{
	g_pfnRemoveBrowser(this, 0, panel, pBrowser);
}

IHTMLChromeController *CSurface::AccessChromeHTMLController(void)
{
	return g_pfnAccessChromeHTMLController(this, 0);
}

void CSurface::setFullscreenMode(int wide, int tall, int bpp)
{
	g_pfnsetFullscreenMode(this, 0, wide, tall, bpp);
}

void CSurface::setWindowedMode(void)
{
	g_pfnsetWindowedMode(this, 0);
}

void CSurface::SetAsTopMost(bool state)
{
	g_pfnSetAsTopMost(this, 0, state);
}

void CSurface::SetAsToolBar(bool state)
{
	g_pfnSetAsToolBar(this, 0, state);
}

void CSurface::PanelRequestFocus(VPANEL panel)
{
	g_pfnPanelRequestFocus(this, 0, panel);
}

void CSurface::EnableMouseCapture(bool state)
{
	g_pfnEnableMouseCapture(this, 0, state);
}

void CSurface::DrawPrintChar(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1)
{
	g_pfnDrawPrintChar(this, 0, x, y, wide, tall, s0, s1, s1, t1);
}

void CSurface::SetNotifyIcon(Image *image, VPANEL panelToReceiveMessages, const char *text)
{
	g_pfnSetNotifyIcon(this, 0, image, panelToReceiveMessages, text);
}

bool CSurface::SetWatchForComputerUse(bool state)
{
	return g_pfnSetWatchForComputerUse(this, 0, state);
}

double CSurface::GetTimeSinceLastUse(void)
{
	return g_pfnGetTimeSinceLastUse(this, 0);
}

bool CSurface::VGUI2MouseControl(void)
{
	return g_pfnVGUI2MouseControl(this, 0);
}

void CSurface::SetVGUI2MouseControl(bool state)
{
	g_pfnSetVGUI2MouseControl(this, 0, state);
}

#define CWIN32FONT_GETCHARABCWIDTHS_SIG "\x55\x8B\xEC\x83\xEC\x70\x53\x56\x8B\xF1\x8D\x45\xD0\x57\x8D\x4D\xE4\x50\x8B\x45\x08\x8D\x55\xD4\x51\x52\x50\x8B\xCE"

void (__fastcall *g_pfnCWin32Font_GetCharRGBA)(void *pthis, int, int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
hook_t *g_hCWin32Font_GetCharRGBA;

char g_szFontPath[MAX_PATH];
int LogToFile(char *fmt, ...);

void Surface_InstallHook(vgui::ISurface *pSurface)
{
	DWORD *pVFTable = *(DWORD **)&g_Surface;

	g_pfnCWin32Font_GetCharRGBA = (void (__fastcall *)(void *, int, int, int, int, int, int, unsigned char *))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, CWIN32FONT_GETCHARABCWIDTHS_SIG, sizeof(CWIN32FONT_GETCHARABCWIDTHS_SIG) - 1);

	g_hCWin32Font_GetCharRGBA = g_pMetaHookAPI->InlineHook(g_pfnCWin32Font_GetCharRGBA, CWin32Font_GetCharRGBA, (void *&)g_pfnCWin32Font_GetCharRGBA);

	pSurface->AddCustomFontFile("resource\\font\\font.ttf");
	g_pFileSystem->GetLocalPath("resource\\font\\font.ttf", g_szFontPath, sizeof(g_szFontPath));

	g_pMetaHookAPI->VFTHook(pSurface, 0, 1, (void *)pVFTable[1], (void *&)g_pfnSurface_Shutdown);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 13, (void *)pVFTable[13], (void *&)g_pfnDrawSetTextFont);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 15, (void *)pVFTable[15], (void *&)g_pfnDrawSetTextColor);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 14, (void *)pVFTable[14], (void *&)g_pfnDrawSetTextColor2);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 19, (void *)pVFTable[19], (void *&)g_pfnDrawUnicodeChar);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 20, (void *)pVFTable[20], (void *&)g_pfnDrawUnicodeCharAdd);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 59, (void *)pVFTable[59], (void *&)g_pfnCreateFont);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 60, (void *)pVFTable[60], (void *&)g_pfnAddGlyphSetToFont);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 62, (void *)pVFTable[62], (void *&)g_pfnGetFontTall);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 63, (void *)pVFTable[63], (void *&)g_pfnGetCharABCwide);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 64, (void *)pVFTable[64], (void *&)g_pfnGetCharacterWidth);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 65, (void *)pVFTable[65], (void *&)g_pfnGetTextSize);
	g_pMetaHookAPI->VFTHook(pSurface, 0, 89, (void *)pVFTable[89], (void *&)g_pfnGetFontAscent);
}