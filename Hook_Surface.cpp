// inplement in Surface.cpp, no longer used here
/*
#include <metahook.h>
#include <plugins.h>
#include <vgui/ISurface.h>
#include <IEngineSurface.h>
#include "Color.h"
#include "qgl.h"

vgui::ISurface *g_pSurface = NULL;

using namespace vgui;

void(__fastcall *g_pfnCSurface_Shutdown)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCSurface_RunFrame)(void *pthis, int edx) = 0;
VPANEL(__fastcall *g_pfnCSurface_GetEmbeddedPanel)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCSurface_SetEmbeddedPanel)(void *pthis, int edx, VPANEL pPanel) = 0;
void(__fastcall *g_pfnCSurface_PushMakeCurrent)(void *pthis, int edx, VPANEL panel, bool useInsets) = 0;
void(__fastcall *g_pfnCSurface_PopMakeCurrent)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_DrawSetColor)(void *pthis, int edx, int r, int g, int b, int a) = 0;
void(__fastcall *g_pfnCSurface_DrawSetColor2)(void *pthis, int edx, Color col) = 0;
void(__fastcall *g_pfnCSurface_DrawFilledRect)(void *pthis, int edx, int x0, int y0, int x1, int y1) = 0;
void(__fastcall *g_pfnCSurface_DrawOutlinedRect)(void *pthis, int edx, int x0, int y0, int x1, int y1) = 0;
void(__fastcall *g_pfnCSurface_DrawLine)(void *pthis, int edx, int x0, int y0, int x1, int y1) = 0;
void(__fastcall *g_pfnCSurface_DrawPolyLine)(void *pthis, int edx, int *px, int *py, int numPoints) = 0;
void(__fastcall *g_pfnCSurface_DrawSetTextFont)(void *pthis, int edx, HFont font) = 0;
void(__fastcall *g_pfnCSurface_DrawSetTextColor)(void *pthis, int edx, int r, int g, int b, int a) = 0;
void(__fastcall *g_pfnCSurface_DrawSetTextColor2)(void *pthis, int edx, Color col) = 0;
void(__fastcall *g_pfnCSurface_DrawSetTextPos)(void *pthis, int edx, int x, int y) = 0;
void(__fastcall *g_pfnCSurface_DrawGetTextPos)(void *pthis, int edx, int &x, int &y) = 0;
void(__fastcall *g_pfnCSurface_DrawPrintText)(void *pthis, int edx, const wchar_t *text, int textLen) = 0;
void(__fastcall *g_pfnCSurface_DrawUnicodeChar)(void *pthis, int edx, wchar_t wch) = 0;
void(__fastcall *g_pfnCSurface_DrawUnicodeCharAdd)(void *pthis, int edx, wchar_t wch) = 0;
void(__fastcall *g_pfnCSurface_DrawFlushText)(void *pthis, int edx) = 0;
IHTML* (__fastcall *g_pfnCSurface_CreateHTMLWindow)(void *pthis, int edx, IHTMLEvents *events, VPANEL context) = 0;
void(__fastcall *g_pfnCSurface_PaintHTMLWindow)(void *pthis, int edx, IHTML *htmlwin) = 0;
void(__fastcall *g_pfnCSurface_DeleteHTMLWindow)(void *pthis, int edx, IHTML *htmlwin) = 0;
void(__fastcall *g_pfnCSurface_DrawSetTextureFile)(void *pthis, int edx, int id, const char *filename, int hardwareFilter, bool forceReload) = 0;
void(__fastcall *g_pfnCSurface_DrawSetTextureRGBA)(void *pthis, int edx, int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload) = 0;
void(__fastcall *g_pfnCSurface_DrawSetTexture)(void *pthis, int edx, int id) = 0;
void(__fastcall *g_pfnCSurface_DrawGetTextureSize)(void *pthis, int edx, int id, int &wide, int &tall) = 0;
void(__fastcall *g_pfnCSurface_DrawTexturedRect)(void *pthis, int edx, int x0, int y0, int x1, int y1) = 0;
bool(__fastcall *g_pfnCSurface_IsTextureIDValid)(void *pthis, int edx, int id) = 0;
int(__fastcall *g_pfnCSurface_CreateNewTextureID)(void *pthis, int edx, bool procedural) = 0;
void(__fastcall *g_pfnCSurface_GetScreenSize)(void *pthis, int edx, int &wide, int &tall) = 0;
void(__fastcall *g_pfnCSurface_SetAsTopMost)(void *pthis, int edx, VPANEL panel, bool state) = 0;
void(__fastcall *g_pfnCSurface_BringToFront)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_SetForegroundWindow)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_SetPanelVisible)(void *pthis, int edx, VPANEL panel, bool state) = 0;
void(__fastcall *g_pfnCSurface_SetMinimized)(void *pthis, int edx, VPANEL panel, bool state) = 0;
bool(__fastcall *g_pfnCSurface_IsMinimized)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_FlashWindow)(void *pthis, int edx, VPANEL panel, bool state) = 0;
void(__fastcall *g_pfnCSurface_SetTitle)(void *pthis, int edx, VPANEL panel, const wchar_t *title) = 0;
void(__fastcall *g_pfnCSurface_SetAsToolBar)(void *pthis, int edx, VPANEL panel, bool state) = 0;
void(__fastcall *g_pfnCSurface_CreatePopup)(void *pthis, int edx, VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput, bool kbInput) = 0;
void(__fastcall *g_pfnCSurface_SwapBuffers)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_Invalidate)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_SetCursor)(void *pthis, int edx, HCursor cursor) = 0;
bool(__fastcall *g_pfnCSurface_IsCursorVisible)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCSurface_ApplyChanges)(void *pthis, int edx) = 0;
bool(__fastcall *g_pfnCSurface_IsWithin)(void *pthis, int edx, int x, int y) = 0;
bool(__fastcall *g_pfnCSurface_HasFocus)(void *pthis, int edx) = 0;
bool(__fastcall *g_pfnCSurface_SupportsFeature)(void *pthis, int edx, ISurface::SurfaceFeature_e feature) = 0;
void(__fastcall *g_pfnCSurface_RestrictPaintToSinglePanel)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_SetModalPanel)(void *pthis, int edx, VPANEL panel) = 0;
VPANEL(__fastcall *g_pfnCSurface_GetModalPanel)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCSurface_UnlockCursor)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCSurface_LockCursor)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCSurface_SetTranslateExtendedKeys)(void *pthis, int edx, bool state) = 0;
VPANEL(__fastcall *g_pfnCSurface_GetTopmostPopup)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCSurface_SetTopLevelFocus)(void *pthis, int edx, VPANEL panel) = 0;
HFont(__fastcall *g_pfnCSurface_CreateFont)(void *pthis, int edx) = 0;
bool(__fastcall *g_pfnCSurface_AddGlyphSetToFont)(void *pthis, int edx, HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange) = 0;
bool(__fastcall *g_pfnCSurface_AddCustomFontFile)(void *pthis, int edx, const char *fontFileName) = 0;
int(__fastcall *g_pfnCSurface_GetFontTall)(void *pthis, int edx, HFont font) = 0;
void(__fastcall *g_pfnCSurface_GetCharABCwide)(void *pthis, int edx, HFont font, int ch, int &a, int &b, int &c) = 0;
int(__fastcall *g_pfnCSurface_GetCharacterWidth)(void *pthis, int edx, HFont font, int ch) = 0;
void(__fastcall *g_pfnCSurface_GetTextSize)(void *pthis, int edx, HFont font, const wchar_t *text, int &wide, int &tall) = 0;
VPANEL(__fastcall *g_pfnCSurface_GetNotifyPanel)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCSurface_SetNotifyIcon)(void *pthis, int edx, VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text) = 0;
void(__fastcall *g_pfnCSurface_PlaySound)(void *pthis, int edx, const char *fileName) = 0;
int(__fastcall *g_pfnCSurface_GetPopupCount)(void *pthis, int edx) = 0;
VPANEL(__fastcall *g_pfnCSurface_GetPopup)(void *pthis, int edx, int index) = 0;
bool(__fastcall *g_pfnCSurface_ShouldPaintChildPanel)(void *pthis, int edx, VPANEL childPanel) = 0;
bool(__fastcall *g_pfnCSurface_RecreateContext)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_AddPanel)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_ReleasePanel)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_MovePopupToFront)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_MovePopupToBack)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_SolveTraverse)(void *pthis, int edx, VPANEL panel, bool forceApplySchemeSettings) = 0;
void(__fastcall *g_pfnCSurface_PaintTraverse)(void *pthis, int edx, VPANEL panel) = 0;
void(__fastcall *g_pfnCSurface_EnableMouseCapture)(void *pthis, int edx, VPANEL panel, bool state) = 0;
void(__fastcall *g_pfnCSurface_GetWorkspaceBounds)(void *pthis, int edx, int &x, int &y, int &wide, int &tall) = 0;
void(__fastcall *g_pfnCSurface_GetAbsoluteWindowBounds)(void *pthis, int edx, int &x, int &y, int &wide, int &tall) = 0;
void(__fastcall *g_pfnCSurface_GetProportionalBase)(void *pthis, int edx, int &width, int &height) = 0;
void(__fastcall *g_pfnCSurface_CalculateMouseVisible)(void *pthis, int edx) = 0;
bool(__fastcall *g_pfnCSurface_NeedKBInput)(void *pthis, int edx) = 0;
bool(__fastcall *g_pfnCSurface_HasCursorPosFunctions)(void *pthis, int edx) = 0;
void(__fastcall *g_pfnCSurface_SurfaceGetCursorPos)(void *pthis, int edx, int &x, int &y) = 0;
void(__fastcall *g_pfnCSurface_SurfaceSetCursorPos)(void *pthis, int edx, int x, int y) = 0;
void(__fastcall *g_pfnCSurface_DrawTexturedPolygon)(void *pthis, int edx, int *p, int n) = 0;
int(__fastcall *g_pfnCSurface_GetFontAscent)(void *pthis, int edx, HFont font, wchar_t wch) = 0;
void(__fastcall *g_pfnCSurface_SetAllowHTMLJavaScript)(void *pthis, int edx, bool state) = 0;

class CNewSurface : public ISurface
{
public:
	void Shutdown(void);
	void RunFrame(void);
	VPANEL GetEmbeddedPanel(void);
	void SetEmbeddedPanel(VPANEL pPanel);
	void PushMakeCurrent(VPANEL panel, bool useInsets);
	void PopMakeCurrent(VPANEL panel);
	void DrawSetColor(int r, int g, int b, int a);
	void DrawSetColor(Color col);
	void DrawFilledRect(int x0, int y0, int x1, int y1);
	void DrawOutlinedRect(int x0, int y0, int x1, int y1);
	void DrawLine(int x0, int y0, int x1, int y1);
	void DrawPolyLine(int *px, int *py, int numPoints);
	void DrawSetTextFont(HFont font);
	void DrawSetTextColor(int r, int g, int b, int a);
	void DrawSetTextColor(Color col);
	void DrawSetTextPos(int x, int y);
	void DrawGetTextPos(int &x, int &y);
	void DrawPrintText(const wchar_t *text, int textLen);
	void DrawUnicodeChar(wchar_t wch);
	void DrawUnicodeCharAdd(wchar_t wch);
	void DrawFlushText(void);
	IHTML *CreateHTMLWindow(IHTMLEvents *events, VPANEL context);
	void PaintHTMLWindow(IHTML *htmlwin);
	void DeleteHTMLWindow(IHTML *htmlwin);
	void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload);
	void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload);
	void DrawSetTexture(int id);
	void DrawGetTextureSize(int id, int &wide, int &tall);
	void DrawTexturedRect(int x0, int y0, int x1, int y1);
	bool IsTextureIDValid(int id);
	int CreateNewTextureID(bool procedural = false);
	void GetScreenSize(int &wide, int &tall);
	void SetAsTopMost(VPANEL panel, bool state);
	void BringToFront(VPANEL panel);
	void SetForegroundWindow(VPANEL panel);
	void SetPanelVisible(VPANEL panel, bool state);
	void SetMinimized(VPANEL panel, bool state);
	bool IsMinimized(VPANEL panel);
	void FlashWindow(VPANEL panel, bool state);
	void SetTitle(VPANEL panel, const wchar_t *title);
	void SetAsToolBar(VPANEL panel, bool state);
	void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true);
	void SwapBuffers(VPANEL panel);
	void Invalidate(VPANEL panel);
	void SetCursor(HCursor cursor);
	bool IsCursorVisible(void);
	void ApplyChanges(void);
	bool IsWithin(int x, int y);
	bool HasFocus(void);
	bool SupportsFeature(SurfaceFeature_e feature);
	void RestrictPaintToSinglePanel(VPANEL panel);
	void SetModalPanel(VPANEL panel);
	VPANEL GetModalPanel(void);
	void UnlockCursor(void);
	void LockCursor(void);
	void SetTranslateExtendedKeys(bool state);
	VPANEL GetTopmostPopup(void);
	void SetTopLevelFocus(VPANEL panel);
	HFont CreateFont(void);
	bool AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
	bool AddCustomFontFile(const char *fontFileName);
	int GetFontTall(HFont font);
	void GetCharABCwide(HFont font, int ch, int &a, int &b, int &c);
	int GetCharacterWidth(HFont font, int ch);
	void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall);
	VPANEL GetNotifyPanel(void);
	void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text);
	void PlaySound(const char *fileName);
	int GetPopupCount(void);
	VPANEL GetPopup(int index);
	bool ShouldPaintChildPanel(VPANEL childPanel);
	bool RecreateContext(VPANEL panel);
	void AddPanel(VPANEL panel);
	void ReleasePanel(VPANEL panel);
	void MovePopupToFront(VPANEL panel);
	void MovePopupToBack(VPANEL panel);
	void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false);
	void PaintTraverse(VPANEL panel);
	void EnableMouseCapture(VPANEL panel, bool state);
	void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall);
	void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall);
	void GetProportionalBase(int &width, int &height);
	void CalculateMouseVisible(void);
	bool NeedKBInput(void);
	bool HasCursorPosFunctions(void);
	void SurfaceGetCursorPos(int &x, int &y);
	void SurfaceSetCursorPos(int x, int y);
	void DrawTexturedPolygon(int *p, int n);
	int GetFontAscent(HFont font, wchar_t wch);
	void SetAllowHTMLJavaScript(bool state);
};

void CNewSurface::Shutdown(void)
{
	return g_pfnCSurface_Shutdown(this, 0);
}

void CNewSurface::RunFrame(void)
{
	return g_pfnCSurface_RunFrame(this, 0);
}

VPANEL CNewSurface::GetEmbeddedPanel(void)
{
	return g_pfnCSurface_GetEmbeddedPanel(this, 0);
}

void CNewSurface::SetEmbeddedPanel(VPANEL pPanel)
{
	return g_pfnCSurface_SetEmbeddedPanel(this, 0, pPanel);
}

void CNewSurface::PushMakeCurrent(VPANEL panel, bool useInsets)
{
	return g_pfnCSurface_PushMakeCurrent(this, 0, panel, useInsets);
}

void CNewSurface::PopMakeCurrent(VPANEL panel)
{
	return g_pfnCSurface_PopMakeCurrent(this, 0, panel);
}

void CNewSurface::DrawSetColor(int r, int g, int b, int a)
{
	return g_pfnCSurface_DrawSetColor(this, 0, r, g, b, a);
}

void CNewSurface::DrawSetColor(Color col)
{
	return g_pfnCSurface_DrawSetColor2(this, 0, col);
}

void CNewSurface::DrawFilledRect(int x0, int y0, int x1, int y1)
{
	return g_pfnCSurface_DrawFilledRect(this, 0, x0, y0, x1, y1);
}

void CNewSurface::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
	return g_pfnCSurface_DrawOutlinedRect(this, 0, x0, y0, x1, y1);
}

void CNewSurface::DrawLine(int x0, int y0, int x1, int y1)
{
	return g_pfnCSurface_DrawLine(this, 0, x0, y0, x1, y1);
}

void CNewSurface::DrawPolyLine(int *px, int *py, int numPoints)
{
	return g_pfnCSurface_DrawPolyLine(this, 0, px, py, numPoints);
}

void CNewSurface::DrawSetTextFont(HFont font)
{
	return g_pfnCSurface_DrawSetTextFont(this, 0, font);
}

void CNewSurface::DrawSetTextColor(int r, int g, int b, int a)
{
	return g_pfnCSurface_DrawSetTextColor(this, 0, r, g, b, a);
}

void CNewSurface::DrawSetTextColor(Color col)
{
	return g_pfnCSurface_DrawSetTextColor2(this, 0, col);
}

void CNewSurface::DrawSetTextPos(int x, int y)
{
	return g_pfnCSurface_DrawSetTextPos(this, 0, x, y);
}

void CNewSurface::DrawGetTextPos(int &x, int &y)
{
	return g_pfnCSurface_DrawGetTextPos(this, 0, x, y);
}

void CNewSurface::DrawPrintText(const wchar_t *text, int textLen)
{
	return g_pfnCSurface_DrawPrintText(this, 0, text, textLen);
}

void CNewSurface::DrawUnicodeChar(wchar_t wch)
{
	return g_pfnCSurface_DrawUnicodeChar(this, 0, wch);
}

void CNewSurface::DrawUnicodeCharAdd(wchar_t wch)
{
}

void CNewSurface::DrawFlushText(void)
{
	return g_pfnCSurface_DrawFlushText(this, 0);
}

IHTML* CNewSurface::CreateHTMLWindow(IHTMLEvents *events, VPANEL context)
{
	return g_pfnCSurface_CreateHTMLWindow(this, 0, events, context);
}

void CNewSurface::PaintHTMLWindow(IHTML *htmlwin)
{
	return g_pfnCSurface_PaintHTMLWindow(this, 0, htmlwin);
}

void CNewSurface::DeleteHTMLWindow(IHTML *htmlwin)
{
	return g_pfnCSurface_DeleteHTMLWindow(this, 0, htmlwin);
}

void CNewSurface::DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload)
{
	return g_pfnCSurface_DrawSetTextureFile(this, 0, id, filename, hardwareFilter, forceReload);
}

void CNewSurface::DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	return g_pfnCSurface_DrawSetTextureRGBA(this, 0, id, rgba, wide, tall, hardwareFilter, forceReload);
}

void CNewSurface::DrawSetTexture(int id)
{
	return g_pfnCSurface_DrawSetTexture(this, 0, id);
}

void CNewSurface::DrawGetTextureSize(int id, int &wide, int &tall)
{
	return g_pfnCSurface_DrawGetTextureSize(this, 0, id, wide, tall);
}

void CNewSurface::DrawTexturedRect(int x0, int y0, int x1, int y1)
{
	return g_pfnCSurface_DrawTexturedRect(this, 0, x0, y0, x1, y1);
}

bool CNewSurface::IsTextureIDValid(int id)
{
	return g_pfnCSurface_IsTextureIDValid(this, 0, id);
}

int CNewSurface::CreateNewTextureID(bool procedural)
{
	return g_pfnCSurface_CreateNewTextureID(this, 0, procedural);
}

void CNewSurface::GetScreenSize(int &wide, int &tall)
{
	wide = g_iVideoWidth;
	tall = g_iVideoHeight;
	//return g_pfnCSurface_GetScreenSize(this, 0, wide, tall);
}

void CNewSurface::SetAsTopMost(VPANEL panel, bool state)
{
	return g_pfnCSurface_SetAsTopMost(this, 0, panel, state);
}

void CNewSurface::BringToFront(VPANEL panel)
{
	return g_pfnCSurface_BringToFront(this, 0, panel);
}

void CNewSurface::SetForegroundWindow(VPANEL panel)
{
	return g_pfnCSurface_SetForegroundWindow(this, 0, panel);
}

void CNewSurface::SetPanelVisible(VPANEL panel, bool state)
{
	return g_pfnCSurface_SetPanelVisible(this, 0, panel, state);
}

void CNewSurface::SetMinimized(VPANEL panel, bool state)
{
	return g_pfnCSurface_SetMinimized(this, 0, panel, state);
}

bool CNewSurface::IsMinimized(VPANEL panel)
{
	return g_pfnCSurface_IsMinimized(this, 0, panel);
}

void CNewSurface::FlashWindow(VPANEL panel, bool state)
{
	return g_pfnCSurface_FlashWindow(this, 0, panel, state);
}

void CNewSurface::SetTitle(VPANEL panel, const wchar_t *title)
{
	return g_pfnCSurface_SetTitle(this, 0, panel, title);
}

void CNewSurface::SetAsToolBar(VPANEL panel, bool state)
{
	return g_pfnCSurface_SetAsToolBar(this, 0, panel, state);
}

void CNewSurface::CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput, bool kbInput)
{
	return g_pfnCSurface_CreatePopup(this, 0, panel, minimised, showTaskbarIcon, disabled, mouseInput, kbInput);
}

void CNewSurface::SwapBuffers(VPANEL panel)
{
	return g_pfnCSurface_SwapBuffers(this, 0, panel);
}

void CNewSurface::Invalidate(VPANEL panel)
{
	return g_pfnCSurface_Invalidate(this, 0, panel);
}

void CNewSurface::SetCursor(HCursor cursor)
{
	return g_pfnCSurface_SetCursor(this, 0, cursor);
}

bool CNewSurface::IsCursorVisible(void)
{
	return g_pfnCSurface_IsCursorVisible(this, 0);
}

void CNewSurface::ApplyChanges(void)
{
	return g_pfnCSurface_ApplyChanges(this, 0);
}

bool CNewSurface::IsWithin(int x, int y)
{
	return g_pfnCSurface_IsWithin(this, 0, x, y);
}

bool CNewSurface::HasFocus(void)
{
	return g_pfnCSurface_HasFocus(this, 0);
}

bool CNewSurface::SupportsFeature(SurfaceFeature_e feature)
{
	return g_pfnCSurface_SupportsFeature(this, 0, feature);
}

void CNewSurface::RestrictPaintToSinglePanel(VPANEL panel)
{
	return g_pfnCSurface_RestrictPaintToSinglePanel(this, 0, panel);
}

void CNewSurface::SetModalPanel(VPANEL panel)
{
	return g_pfnCSurface_SetModalPanel(this, 0, panel);
}

VPANEL CNewSurface::GetModalPanel(void)
{
	return g_pfnCSurface_GetModalPanel(this, 0);
}

void CNewSurface::UnlockCursor(void)
{
	return g_pfnCSurface_UnlockCursor(this, 0);
}

void CNewSurface::LockCursor(void)
{
	return g_pfnCSurface_LockCursor(this, 0);
}

void CNewSurface::SetTranslateExtendedKeys(bool state)
{
	return g_pfnCSurface_SetTranslateExtendedKeys(this, 0, state);
}

VPANEL CNewSurface::GetTopmostPopup(void)
{
	return g_pfnCSurface_GetTopmostPopup(this, 0);
}

void CNewSurface::SetTopLevelFocus(VPANEL panel)
{
	return g_pfnCSurface_SetTopLevelFocus(this, 0, panel);
}

HFont CNewSurface::CreateFont(void)
{
	return g_pfnCSurface_CreateFont(this, 0);
}

bool CNewSurface::AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	return g_pfnCSurface_AddGlyphSetToFont(this, 0, font, windowsFontName, tall, weight, blur, scanlines, flags, lowRange, highRange);
}

bool CNewSurface::AddCustomFontFile(const char *fontFileName)
{
	return g_pfnCSurface_AddCustomFontFile(this, 0, fontFileName);
}

int CNewSurface::GetFontTall(HFont font)
{
	return g_pfnCSurface_GetFontTall(this, 0, font);
}

void CNewSurface::GetCharABCwide(HFont font, int ch, int &a, int &b, int &c)
{
	return g_pfnCSurface_GetCharABCwide(this, 0, font, ch, a, b, c);
}

int CNewSurface::GetCharacterWidth(HFont font, int ch)
{
	return g_pfnCSurface_GetCharacterWidth(this, 0, font, ch);
}

void CNewSurface::GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall)
{
	return g_pfnCSurface_GetTextSize(this, 0, font, text, wide, tall);
}

VPANEL CNewSurface::GetNotifyPanel(void)
{
	return g_pfnCSurface_GetNotifyPanel(this, 0);
}

void CNewSurface::SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text)
{
	return g_pfnCSurface_SetNotifyIcon(this, 0, context, icon, panelToReceiveMessages, text);
}

void CNewSurface::PlaySound(const char *fileName)
{
	return g_pfnCSurface_PlaySound(this, 0, fileName);
}

int CNewSurface::GetPopupCount(void)
{
	return g_pfnCSurface_GetPopupCount(this, 0);
}

VPANEL CNewSurface::GetPopup(int index)
{
	return g_pfnCSurface_GetPopup(this, 0, index);
}

bool CNewSurface::ShouldPaintChildPanel(VPANEL childPanel)
{
	return g_pfnCSurface_ShouldPaintChildPanel(this, 0, childPanel);
}

bool CNewSurface::RecreateContext(VPANEL panel)
{
	return g_pfnCSurface_RecreateContext(this, 0, panel);
}

void CNewSurface::AddPanel(VPANEL panel)
{
	return g_pfnCSurface_AddPanel(this, 0, panel);
}

void CNewSurface::ReleasePanel(VPANEL panel)
{
	return g_pfnCSurface_ReleasePanel(this, 0, panel);
}

void CNewSurface::MovePopupToFront(VPANEL panel)
{
	return g_pfnCSurface_MovePopupToFront(this, 0, panel);
}

void CNewSurface::MovePopupToBack(VPANEL panel)
{
	return g_pfnCSurface_MovePopupToBack(this, 0, panel);
}

void CNewSurface::SolveTraverse(VPANEL panel, bool forceApplySchemeSettings)
{
	return g_pfnCSurface_SolveTraverse(this, 0, panel, forceApplySchemeSettings);
}

void CNewSurface::PaintTraverse(VPANEL panel)
{
	return g_pfnCSurface_PaintTraverse(this, 0, panel);
}

void CNewSurface::EnableMouseCapture(VPANEL panel, bool state)
{
	return g_pfnCSurface_EnableMouseCapture(this, 0, panel, state);
}

void CNewSurface::GetWorkspaceBounds(int &x, int &y, int &wide, int &tall)
{
	return g_pfnCSurface_GetWorkspaceBounds(this, 0, x, y, wide, tall);
}

void CNewSurface::GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall)
{
	return g_pfnCSurface_GetAbsoluteWindowBounds(this, 0, x, y, wide, tall);
}

void CNewSurface::GetProportionalBase(int &width, int &height)
{
	return g_pfnCSurface_GetProportionalBase(this, 0, width, height);
}

void CNewSurface::CalculateMouseVisible(void)
{
	return g_pfnCSurface_CalculateMouseVisible(this, 0);
}

bool CNewSurface::NeedKBInput(void)
{
	return g_pfnCSurface_NeedKBInput(this, 0);
}

bool CNewSurface::HasCursorPosFunctions(void)
{
	return g_pfnCSurface_HasCursorPosFunctions(this, 0);
}

void CNewSurface::SurfaceGetCursorPos(int &x, int &y)
{
	return g_pfnCSurface_SurfaceGetCursorPos(this, 0, x, y);
}

void CNewSurface::SurfaceSetCursorPos(int x, int y)
{
	return g_pfnCSurface_SurfaceSetCursorPos(this, 0, x, y);
}

void CNewSurface::DrawTexturedPolygon(int *p, int n)
{
	return g_pfnCSurface_DrawTexturedPolygon(this, 0, p, n);
}

int CNewSurface::GetFontAscent(HFont font, wchar_t wch)
{
	return g_pfnCSurface_GetFontAscent(this, 0, font, wch);
}

void CNewSurface::SetAllowHTMLJavaScript(bool state)
{
	return g_pfnCSurface_SetAllowHTMLJavaScript(this, 0, state);
}

void Surface_InstallHook(vgui::ISurface *pSurface)
{
	CNewSurface NewSurface;
	DWORD *pVFTable = *(DWORD **)&NewSurface;

	CreateInterfaceFn engineFactory = g_pMetaHookAPI->GetEngineFactory();
	g_pSurface = pSurface;

	//g_pMetaHookAPI->VFTHook(g_pSurface, 0,  1, (void *)pVFTable[ 1], (void *&)g_pfnCSurface_Shutdown);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0,  2, (void *)pVFTable[ 2], (void *&)g_pfnCSurface_RunFrame);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0,  3, (void *)pVFTable[ 3], (void *&)g_pfnCSurface_GetEmbeddedPanel);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0,  4, (void *)pVFTable[ 4], (void *&)g_pfnCSurface_SetEmbeddedPanel);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0,  5, (void *)pVFTable[ 5], (void *&)g_pfnCSurface_PushMakeCurrent);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0,  6, (void *)pVFTable[ 6], (void *&)g_pfnCSurface_PopMakeCurrent);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0,  7, (void *)pVFTable[ 7], (void *&)g_pfnCSurface_DrawSetColor2);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0,  8, (void *)pVFTable[ 8], (void *&)g_pfnCSurface_DrawSetColor);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0,  9, (void *)pVFTable[ 9], (void *&)g_pfnCSurface_DrawFilledRect);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 10, (void *)pVFTable[10], (void *&)g_pfnCSurface_DrawOutlinedRect);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 11, (void *)pVFTable[11], (void *&)g_pfnCSurface_DrawLine);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 12, (void *)pVFTable[12], (void *&)g_pfnCSurface_DrawPolyLine);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 13, (void *)pVFTable[13], (void *&)g_pfnCSurface_DrawSetTextFont);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 14, (void *)pVFTable[14], (void *&)g_pfnCSurface_DrawSetTextColor2);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 15, (void *)pVFTable[15], (void *&)g_pfnCSurface_DrawSetTextColor);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 16, (void *)pVFTable[16], (void *&)g_pfnCSurface_DrawSetTextPos);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 17, (void *)pVFTable[17], (void *&)g_pfnCSurface_DrawGetTextPos);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 18, (void *)pVFTable[18], (void *&)g_pfnCSurface_DrawPrintText);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 19, (void *)pVFTable[19], (void *&)g_pfnCSurface_DrawUnicodeChar);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 20, (void *)pVFTable[20], (void *&)g_pfnCSurface_DrawUnicodeCharAdd);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 21, (void *)pVFTable[21], (void *&)g_pfnCSurface_DrawFlushText);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 22, (void *)pVFTable[22], (void *&)g_pfnCSurface_CreateHTMLWindow);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 23, (void *)pVFTable[23], (void *&)g_pfnCSurface_PaintHTMLWindow);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 24, (void *)pVFTable[24], (void *&)g_pfnCSurface_DeleteHTMLWindow);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 25, (void *)pVFTable[25], (void *&)g_pfnCSurface_DrawSetTextureFile);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 26, (void *)pVFTable[26], (void *&)g_pfnCSurface_DrawSetTextureRGBA);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 27, (void *)pVFTable[27], (void *&)g_pfnCSurface_DrawSetTexture);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 28, (void *)pVFTable[28], (void *&)g_pfnCSurface_DrawGetTextureSize);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 29, (void *)pVFTable[29], (void *&)g_pfnCSurface_DrawTexturedRect);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 30, (void *)pVFTable[30], (void *&)g_pfnCSurface_IsTextureIDValid);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 31, (void *)pVFTable[31], (void *&)g_pfnCSurface_CreateNewTextureID);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 32, (void *)pVFTable[32], (void *&)g_pfnCSurface_GetScreenSize);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 33, (void *)pVFTable[33], (void *&)g_pfnCSurface_SetAsTopMost);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 34, (void *)pVFTable[34], (void *&)g_pfnCSurface_BringToFront);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 35, (void *)pVFTable[35], (void *&)g_pfnCSurface_SetForegroundWindow);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 36, (void *)pVFTable[36], (void *&)g_pfnCSurface_SetPanelVisible);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 37, (void *)pVFTable[37], (void *&)g_pfnCSurface_SetMinimized);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 38, (void *)pVFTable[38], (void *&)g_pfnCSurface_IsMinimized);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 39, (void *)pVFTable[39], (void *&)g_pfnCSurface_FlashWindow);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 40, (void *)pVFTable[40], (void *&)g_pfnCSurface_SetTitle);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 41, (void *)pVFTable[41], (void *&)g_pfnCSurface_SetAsToolBar);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 42, (void *)pVFTable[42], (void *&)g_pfnCSurface_CreatePopup);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 43, (void *)pVFTable[43], (void *&)g_pfnCSurface_SwapBuffers);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 44, (void *)pVFTable[44], (void *&)g_pfnCSurface_Invalidate);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 45, (void *)pVFTable[45], (void *&)g_pfnCSurface_SetCursor);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 46, (void *)pVFTable[46], (void *&)g_pfnCSurface_IsCursorVisible);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 47, (void *)pVFTable[47], (void *&)g_pfnCSurface_ApplyChanges);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 48, (void *)pVFTable[48], (void *&)g_pfnCSurface_IsWithin);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 49, (void *)pVFTable[49], (void *&)g_pfnCSurface_HasFocus);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 50, (void *)pVFTable[50], (void *&)g_pfnCSurface_SupportsFeature);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 51, (void *)pVFTable[51], (void *&)g_pfnCSurface_RestrictPaintToSinglePanel);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 52, (void *)pVFTable[52], (void *&)g_pfnCSurface_SetModalPanel);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 53, (void *)pVFTable[53], (void *&)g_pfnCSurface_GetModalPanel);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 54, (void *)pVFTable[54], (void *&)g_pfnCSurface_UnlockCursor);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 55, (void *)pVFTable[55], (void *&)g_pfnCSurface_LockCursor);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 56, (void *)pVFTable[56], (void *&)g_pfnCSurface_SetTranslateExtendedKeys);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 57, (void *)pVFTable[57], (void *&)g_pfnCSurface_GetTopmostPopup);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 58, (void *)pVFTable[58], (void *&)g_pfnCSurface_SetTopLevelFocus);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 59, (void *)pVFTable[59], (void *&)g_pfnCSurface_CreateFont);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 60, (void *)pVFTable[60], (void *&)g_pfnCSurface_AddGlyphSetToFont);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 61, (void *)pVFTable[61], (void *&)g_pfnCSurface_AddCustomFontFile);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 62, (void *)pVFTable[62], (void *&)g_pfnCSurface_GetFontTall);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 63, (void *)pVFTable[63], (void *&)g_pfnCSurface_GetCharABCwide);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 64, (void *)pVFTable[64], (void *&)g_pfnCSurface_GetCharacterWidth);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 65, (void *)pVFTable[65], (void *&)g_pfnCSurface_GetTextSize);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 66, (void *)pVFTable[66], (void *&)g_pfnCSurface_GetNotifyPanel);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 67, (void *)pVFTable[67], (void *&)g_pfnCSurface_SetNotifyIcon);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 68, (void *)pVFTable[68], (void *&)g_pfnCSurface_PlaySound);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 69, (void *)pVFTable[69], (void *&)g_pfnCSurface_GetPopupCount);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 70, (void *)pVFTable[70], (void *&)g_pfnCSurface_GetPopup);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 71, (void *)pVFTable[71], (void *&)g_pfnCSurface_ShouldPaintChildPanel);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 72, (void *)pVFTable[72], (void *&)g_pfnCSurface_RecreateContext);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 73, (void *)pVFTable[73], (void *&)g_pfnCSurface_AddPanel);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 74, (void *)pVFTable[74], (void *&)g_pfnCSurface_ReleasePanel);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 75, (void *)pVFTable[75], (void *&)g_pfnCSurface_MovePopupToFront);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 76, (void *)pVFTable[76], (void *&)g_pfnCSurface_MovePopupToBack);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 77, (void *)pVFTable[77], (void *&)g_pfnCSurface_SolveTraverse);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 78, (void *)pVFTable[78], (void *&)g_pfnCSurface_PaintTraverse);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 79, (void *)pVFTable[79], (void *&)g_pfnCSurface_EnableMouseCapture);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 80, (void *)pVFTable[80], (void *&)g_pfnCSurface_GetWorkspaceBounds);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 81, (void *)pVFTable[81], (void *&)g_pfnCSurface_GetAbsoluteWindowBounds);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 82, (void *)pVFTable[82], (void *&)g_pfnCSurface_GetProportionalBase);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 83, (void *)pVFTable[83], (void *&)g_pfnCSurface_CalculateMouseVisible);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 84, (void *)pVFTable[84], (void *&)g_pfnCSurface_NeedKBInput);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 85, (void *)pVFTable[85], (void *&)g_pfnCSurface_HasCursorPosFunctions);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 86, (void *)pVFTable[86], (void *&)g_pfnCSurface_SurfaceGetCursorPos);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 87, (void *)pVFTable[87], (void *&)g_pfnCSurface_SurfaceSetCursorPos);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 88, (void *)pVFTable[88], (void *&)g_pfnCSurface_DrawTexturedPolygon);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 89, (void *)pVFTable[89], (void *&)g_pfnCSurface_GetFontAscent);
	//g_pMetaHookAPI->VFTHook(g_pSurface, 0, 90, (void *)pVFTable[90], (void *&)g_pfnCSurface_SetAllowHTMLJavaScript);
}

*/