#define ENABLE_HTMLWINDOW

#include <metahook.h>
#include <VGUI/ISurface.h>
#include <VGUI/IInput.h>
#include <VGUI/IInputInternal.h>
#include <VGUI/IScheme.h>
#include <VGUI/Cursor.h>
#include "vguimatsurface/FontTextureCache.h"
#include "configs.h"
#include <IEngineSurface.h>
#include "Color.h"
#include "vgui_internal.h"
#include "EngineInterface.h"
#include "plugins.h"
#include "HtmlWindow.h"
#include "LoadTGA.h"
#include "LoadBMP.h"
#include "Window.h"
#include "qgl.h"
#include "CVARs.h"
#include "vguimatsurface/Clip2D.h"
#include <VGUI/Vector.h>
#include <VGUI/IEngineVGui.h>

#include "tier1/utlvector.h"
#include "tier1/utlrbtree.h"
#include "tier1/UtlDict.h"

#define MAX_BATCHED_CHAR_VERTS 4096

using namespace vgui;

struct PaintState_t
{
	VPANEL m_pPanel;
	int	m_iTranslateX;
	int m_iTranslateY;
	int	m_iScissorLeft;
	int	m_iScissorRight;
	int	m_iScissorTop;
	int	m_iScissorBottom;
};

struct CharRenderInfo
{
	int drawType;
	wchar_t ch;

	bool valid;
	bool additive;
	bool shouldclip;

	int x, y;

	Vertex_t verts[2];
	int textureId;
	int abcA;
	int abcB;
	int abcC;
	int fontTall;
	HFont currentFont;
};

struct CBatchedCharInfo
{
	Vertex_t verts[2];
	int textureId;
};

#define CWIN32FONT_GETCHARABCWIDTHS_SIG "\x55\x8B\xEC\x83\xEC\x70\x53\x56\x8B\xF1\x8D\x45\xD0\x57\x8D\x4D\xE4\x50\x8B\x45\x08\x8D\x55\xD4\x51\x52\x50\x8B\xCE"

void (__fastcall *g_pfnCWin32Font_GetCharRGBA)(void *pthis, int, int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
hook_t *g_hCWin32Font_GetCharRGBA;

class Texture
{
public:
	int _id;
	int _wide;
	int _tall;
	float _s0;
	float _t0;
	float _s1;
	float _t1;
	const char *_filename;
};

void (__fastcall *g_pfnSurface_Shutdown)(void *pthis, int);
void (__fastcall *g_pfnSurface_RunFrame)(void *pthis, int);
void (__fastcall *g_pfnDrawSetTextFont)(void *pthis, int, HFont font);
void (__fastcall *g_pfnDrawUnicodeChar)(void *pthis, int, wchar_t wch);
void (__fastcall *g_pfnDrawUnicodeCharAdd)(void *pthis, int, wchar_t wch);
void (__fastcall *g_pfnDrawSetTextureFile)(void *pthis, int, int id, const char *filename, int hardwareFilter, bool forceReload);
void (__fastcall *g_pfnDrawSetTextureRGBA)(void *pthis, int, int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload);
void (__fastcall *g_pfnDrawSetTexture)(void *pthis, int, int id);
bool (__fastcall *g_pfnAddGlyphSetToFont)(void *pthis, int, HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
int (__fastcall *g_pfnGetFontTall)(void *pthis, int, HFont font);
void (__fastcall *g_pfnGetCharABCwide)(void *pthis, int, HFont font, int ch, int &a, int &b, int &c);
int (__fastcall *g_pfnGetCharacterWidth)(void *pthis, int, HFont font, int ch);
void (__fastcall *g_pfnGetTextSize)(void *pthis, int, HFont font, const wchar_t *text, int &wide, int &tall);
int (__fastcall *g_pfnGetFontAscent)(void *pthis, int, HFont font, wchar_t wch);
HFont (__fastcall *g_pfnCreateFont)(void *pthis, int);
void (__fastcall *g_pfnDrawSetColor)(void *pthis, int, int r, int g, int b, int a);
void (__fastcall *g_pfnDrawSetColor2)(void *pthis, int, Color col);
void (__fastcall *g_pfnDrawSetTextColor)(void *pthis, int, int r, int g, int b, int a);
void (__fastcall *g_pfnDrawSetTextColor2)(void *pthis, int, Color col);
void (__fastcall *g_pfnDrawFilledRect)(void *pthis, int, int x0, int y0, int x1, int y1);
void (__fastcall *g_pfnDrawOutlinedRect)(void *pthis, int, int x0, int y0, int x1, int y1);
void (__fastcall *g_pfnDrawLine)(void *pthis, int, int x0, int y0, int x1, int y1);
void (__fastcall *g_pfnDrawPolyLine)(void *pthis, int, int *px, int *py, int numPoints);
void (__fastcall *g_pfnDrawTexturedRect)(void *pthis, int, int x0, int y0, int x1, int y1);
void (__fastcall *g_pfnGetScreenSize)(void *pthis, int, int &wide, int &tall);
IHTML *(__fastcall *g_pfnCreateHTMLWindow)(void *pthis, int, IHTMLEvents *events, VPANEL context);
void (__fastcall *g_pfnPaintHTMLWindow)(void *pthis, int, IHTML *htmlwin);
void (__fastcall *g_pfnDeleteHTMLWindow)(void *pthis, int, IHTML *htmlwin);
bool (__fastcall *g_pfnHasFocus)(void *pthis, int);
void (__fastcall *g_pfnSetAllowHTMLJavaScript)(void *pthis, int, bool state);
bool (__fastcall *g_pfnSupportsFeature)(void *pthis, int, ISurface::SurfaceFeature_e feature);
bool (__fastcall *g_pfnAddCustomFontFile)(void *pthis, int, const char *fontFileName);
void (__fastcall *g_pfnSurfaceSetCursorPos)(void *pthis, int, int x, int y);
void (__fastcall *g_pfnUnlockCursor)(void *pthis, int);
void (__fastcall *g_pfnLockCursor)(void *pthis, int);
void (__fastcall *g_pfnSetCursor)(void *pthis, int, HCursor cursor);
void (__fastcall *g_pfnCalculateMouseVisible)(void *pthis, int);
bool (__fastcall *g_pfnIsCursorVisible)(void *pthis, int);
void (__fastcall *g_pfnSetEmbeddedPanel)(void *pthis, int, VPANEL pPanel);
void (__fastcall *g_pfnPaintTraverse)(void *pthis, int, VPANEL panel);
void (__fastcall *g_pfnPushMakeCurrent)(void *pthis, int, VPANEL panel, bool useInsets);
void (__fastcall *g_pfnPopMakeCurrent)(void *pthis, int, VPANEL panel);
void (__fastcall *g_pfnDrawFlushText)(void *pthis, int);
void (__fastcall *g_pfnRestrictPaintToSinglePanel)(void *pthis, int, VPANEL panel);
void (__fastcall *g_pfnDrawPrintText)(void *pthis, int, const wchar_t *text, int textLen);
int (__fastcall *g_pfnCreateNewTextureID)(void *pthis, int, bool procedural);
bool (__fastcall *g_pfnIsTextureIDValid)(void *pthis, int, int id);
void (__fastcall *g_pfnDrawGetTextureSize)(void *pthis, int, int id, int &wide, int &tall);

class CSurface : public ISurface
{
public:
	CSurface(void);
	~CSurface(void);

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

public:
	void DrawSetAlphaMultiplier(float alpha);
	float DrawGetAlphaMultiplier(void);
	void DumpFontTextures(void);
	void StartDrawing(void);
	void FinishDrawing(void);
	void SetupPaintState(const PaintState_t &paintState);
	void InitVertex(Vertex_t &vertex, int x, int y, float u, float v);
	void DrawTexturedLineInternal(const Vertex_t &a, const Vertex_t &b);
	void DrawTexturedLine(const Vertex_t &a, const Vertex_t &b);
	void DrawTexturedPolyLine(const Vertex_t *p, int n);
	void DrawQuad(const Vertex_t &ul, const Vertex_t &lr, unsigned char *pColor);
	void DrawQuadBlend(const Vertex_t &ul, const Vertex_t &lr, unsigned char *pColor);
	void DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal);
	bool DrawGetUnicodeCharRenderInfo(wchar_t ch, CharRenderInfo &info);
	void DrawRenderCharInternal(const CharRenderInfo &info);
	void DrawRenderCharFromInfo(const CharRenderInfo &info);
	void DrawQuadArray(int quadCount, Vertex_t *pVerts, unsigned char *pColor);
	bool IsPanelUnderRestrictedPanel(VPANEL panel);
	void PaintTraverseEx(VPANEL panel, bool paintPopups = false, bool enableStencil = false);
	void DrawTexturedPolygon(int n, Vertex_t *pVertices);
	void DrawSetTextureRGB(int id, const unsigned char *rgb, int wide, int tall, int hardwareFilter, bool forceReload);
	void DrawSetTextureBGR(int id, const unsigned char *rgb, int wide, int tall, int hardwareFilter, bool forceReload);
	void DrawSetTextureBGRA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload);
	void DrawUpdateRegionTextureRGB(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall);
	void DrawUpdateRegionTextureRGBA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall);
	void DrawUpdateRegionTextureBGR(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall);
	void DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall);
	void DeleteTextureByID(int id);
};

CSurface g_Surface;
bool g_bInDrawing;

bool TextureLessFunc(const Texture &lhs, const Texture &rhs);

float m_flAlphaMultiplier;
bool m_bAllowJavaScript;
CUtlRBTree<Texture, int> m_VGuiSurfaceTextures(0, 128, TextureLessFunc);
CUtlDict<IImage *, unsigned short> m_FileTypeImages;
CUtlVector<HtmlWindow *> m_HtmlWindows;
byte m_TextureBuffer[1024 * 1024 * 4];
HCursor m_hCurrentCursor;
int m_nTranslateX, m_nTranslateY;
int m_pSurfaceExtents[4];
unsigned char m_DrawColor[4];
unsigned char m_DrawTextColor[4];
int m_pDrawTextPos[2];
int m_iBoundTexture;
Texture *m_pBoundTexture;
CUtlVector<PaintState_t> m_PaintStateStack;
CUtlVector<CBatchedCharInfo> m_BatchedCharInfos(0, 256);
HFont m_hCurrentFont;
VPANEL m_pRestrictedPanel;
float m_flZPos;
IImage *m_pMouseTexture;

// BaseUI.h
extern cvar_t *vgui_stencil_test;
extern cvar_t *vgui_message_dialog_modal;
extern cvar_t *vgui_emulatemouse;

CSurface::CSurface(void)
{
	CoInitialize(NULL);

	m_DrawColor[0] = m_DrawColor[1] = m_DrawColor[2] = m_DrawColor[3] = 255;
	m_nTranslateX = m_nTranslateY = 0;

	EnableScissor(false);
	SetScissorRect(0, 0, 100000, 100000);

	m_flAlphaMultiplier = 1.0;

	m_bAllowJavaScript = false;
	m_iBoundTexture = 0;

	m_pDrawTextPos[0] = m_pDrawTextPos[1] = 0;
	m_DrawTextColor[0] = m_DrawTextColor[1] = m_DrawTextColor[2] = m_DrawTextColor[3] = 255;

	m_hCurrentCursor = dc_none;
	m_hCurrentFont = NULL;
	m_pRestrictedPanel = NULL;
	m_pMouseTexture = NULL;
	m_pBoundTexture = NULL;
}

CSurface::~CSurface(void)
{
	for (int i = 0; i < m_HtmlWindows.Size(); i++)
		delete m_HtmlWindows[i];

	for (int i = m_FileTypeImages.First(); i != m_FileTypeImages.InvalidIndex(); i = m_FileTypeImages.Next(i))
		delete m_FileTypeImages[i];

	m_HtmlWindows.RemoveAll();
	m_FileTypeImages.RemoveAll();
	m_iBoundTexture = 0;

	for (int i = 0; i < m_VGuiSurfaceTextures.MaxElement(); i++)
	{
		if (!m_VGuiSurfaceTextures.IsValidIndex(i))
			continue;
	}

	CoUninitialize();
}

bool TextureLessFunc(const Texture &lhs, const Texture &rhs)
{
	return lhs._id < rhs._id;
}

Texture *GetTextureById(int id)
{
	Texture findTex = { id };
	int index = m_VGuiSurfaceTextures.Find(findTex);

	if (m_VGuiSurfaceTextures.IsValidIndex(index))
		return &m_VGuiSurfaceTextures[index];

	return NULL;
}

Texture *AllocTextureForId(int id)
{
	Texture newTex = { id };
	int index = m_VGuiSurfaceTextures.Insert(newTex);
	return &m_VGuiSurfaceTextures[index];
}

int GetNumTextures(void)
{
	return m_VGuiSurfaceTextures.Count();
}

void CSurface::Shutdown(void)
{
	g_pfnSurface_Shutdown(this, 0);

	FontManager().ClearAllFonts();
}

void CSurface::RunFrame(void)
{
	g_pfnSurface_RunFrame(this, 0);
}

void CSurface::StartDrawing(void)
{
	g_bInDrawing = true;
	m_iBoundTexture = 0;

	m_pSurfaceExtents[0] = 0;
	m_pSurfaceExtents[1] = 0;
	m_pSurfaceExtents[2] = g_iVideoWidth;
	m_pSurfaceExtents[3] = g_iVideoHeight;

	EnableScissor(true);

	m_nTranslateX = 0;
	m_nTranslateY = 0;
}

void CSurface::FinishDrawing(void)
{
	EnableScissor(false);

	g_bInDrawing = false;
}

VPANEL CSurface::GetEmbeddedPanel(void)
{
	return g_pSurface->GetEmbeddedPanel();
}

void CSurface::SetEmbeddedPanel(VPANEL pPanel)
{
	g_pfnSetEmbeddedPanel(this, 0, pPanel);
}

void CSurface::SetupPaintState(const PaintState_t &paintState)
{
	m_nTranslateX = paintState.m_iTranslateX;
	m_nTranslateY = paintState.m_iTranslateY;

	SetScissorRect(paintState.m_iScissorLeft, paintState.m_iScissorTop, paintState.m_iScissorRight, paintState.m_iScissorBottom);
}

void CSurface::PushMakeCurrent(VPANEL panel, bool useInsets)
{
	int insets[4] = { 0, 0, 0, 0 };
	int absExtents[4];
	int clipRect[4];

	if (insets)
		g_pVGuiPanel->GetInset(panel, insets[0], insets[1], insets[2], insets[3]);

	g_pVGuiPanel->GetAbsPos(panel, absExtents[0], absExtents[1]);

	int wide, tall;
	g_pVGuiPanel->GetSize(panel, wide, tall);
	absExtents[2] = absExtents[0] + wide;
	absExtents[3] = absExtents[1] + tall;

	g_pVGuiPanel->GetClipRect(panel, clipRect[0], clipRect[1], clipRect[2], clipRect[3]);

	int i = m_PaintStateStack.AddToTail();
	PaintState_t &paintState = m_PaintStateStack[i];
	paintState.m_pPanel = panel;

	paintState.m_iTranslateX = 0;
	paintState.m_iTranslateY = 0;

	paintState.m_iScissorLeft = clipRect[0] - m_pSurfaceExtents[0] - (insets[0] + (absExtents[0] - m_pSurfaceExtents[0]));
	paintState.m_iScissorTop = clipRect[1] - m_pSurfaceExtents[1] - (insets[1] + (absExtents[1] - m_pSurfaceExtents[1]));
	paintState.m_iScissorRight = clipRect[2] - m_pSurfaceExtents[0] - (insets[0] + (absExtents[0] - m_pSurfaceExtents[0]));
	paintState.m_iScissorBottom = clipRect[3] - m_pSurfaceExtents[1] - (insets[1] + (absExtents[1] - m_pSurfaceExtents[1]));

	SetupPaintState(paintState);
	g_pfnPushMakeCurrent(this, 0, panel, useInsets);
}

void CSurface::PopMakeCurrent(VPANEL panel)
{
	if (m_BatchedCharInfos.Count())
	{
		DrawFlushText();
	}

	int top = m_PaintStateStack.Count() - 1;

	Assert(top >= 0);
	Assert(m_PaintStateStack[top].m_pPanel == panel);

	m_PaintStateStack.Remove(top);

	if (top > 0)
		SetupPaintState(m_PaintStateStack[top - 1]);

	g_pfnPopMakeCurrent(this, 0, panel);
}

void CSurface::DrawSetColor(int r, int g, int b, int a)
{
	m_DrawColor[0] = (unsigned char)r;
	m_DrawColor[1] = (unsigned char)g;
	m_DrawColor[2] = (unsigned char)b;
	m_DrawColor[3] = (unsigned char)(a * m_flAlphaMultiplier);

	g_pfnDrawSetColor(this, 0, r, g, b, a);
}

void CSurface::DrawSetColor(Color col)
{
	DrawSetColor(col[0], col[1], col[2], col[3]);
}

void CSurface::InitVertex(Vertex_t &vertex, int x, int y, float u, float v)
{
	vertex.m_Position.Init(x + m_nTranslateX, y + m_nTranslateY);
	vertex.m_TexCoord.Init(u, v);
}

void CSurface::DrawTexturedLineInternal(const Vertex_t &a, const Vertex_t &b)
{
	if (m_DrawColor[3] == 0)
		return;

	Vertex_t verts[2] = { a, b };
	Vertex_t clippedVerts[2];

	verts[0].m_Position.x += m_nTranslateX;
	verts[0].m_Position.y += m_nTranslateY;

	verts[1].m_Position.x += m_nTranslateX;
	verts[1].m_Position.y += m_nTranslateY;

	if (!ClipLine(verts, clippedVerts))
		return;

	if (m_iBoundTexture)
		qglEnable(GL_TEXTURE_2D);
	else
		qglDisable(GL_TEXTURE_2D);

	qglColor4ub(m_DrawColor[0], m_DrawColor[1], m_DrawColor[2], m_DrawColor[3]);
	qglBegin(GL_LINES);
	qglTexCoord2fv(clippedVerts[0].m_TexCoord.Base());
	qglVertex3f(clippedVerts[0].m_Position.x, clippedVerts[0].m_Position.y, m_flZPos);
	qglTexCoord2fv(clippedVerts[1].m_TexCoord.Base());
	qglVertex3f(clippedVerts[1].m_Position.x, clippedVerts[1].m_Position.y, m_flZPos);
	qglEnd();
	qglEnable(GL_TEXTURE_2D);
}

void CSurface::DrawQuadArray(int quadCount, Vertex_t *pVerts, unsigned char *pColor)
{
	if (m_iBoundTexture)
		qglEnable(GL_TEXTURE_2D);
	else
		qglDisable(GL_TEXTURE_2D);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglEnable(GL_BLEND);
	qglColor4ub(pColor[0], pColor[1], pColor[2], pColor[3]);
	qglBegin(GL_QUADS);

	for (int i = 0; i < quadCount; ++i)
	{
		Vertex_t ulc, lrc;
		Vertex_t &ul = pVerts[2 * i];
		Vertex_t &lr = pVerts[2 * i + 1];

		if (!ClipRect(ul, lr, &ulc, &lrc))
			continue;

		qglTexCoord2f(ulc.m_TexCoord.x, ulc.m_TexCoord.y);
		qglVertex3f(ulc.m_Position.x, ulc.m_Position.y, m_flZPos);
		qglTexCoord2f(lrc.m_TexCoord.x, ulc.m_TexCoord.y);
		qglVertex3f(lrc.m_Position.x, ulc.m_Position.y, m_flZPos);
		qglTexCoord2f(lrc.m_TexCoord.x, lrc.m_TexCoord.y);
		qglVertex3f(lrc.m_Position.x, lrc.m_Position.y, m_flZPos);
		qglTexCoord2f(ulc.m_TexCoord.x, lrc.m_TexCoord.y);
		qglVertex3f(ulc.m_Position.x, lrc.m_Position.y, m_flZPos);
	}

	qglEnd();
	qglEnable(GL_TEXTURE_2D);
	qglDisable(GL_BLEND);
}

void CSurface::DrawFilledRect(int x0, int y0, int x1, int y1)
{
	if (m_DrawColor[3] == 0)
		return;

	Vertex_t rect[2];
	Vertex_t clippedRect[2];

	InitVertex(rect[0], x0, y0, 0, 0);
	InitVertex(rect[1], x1, y1, 0, 0);

	if (!ClipRect(rect[0], rect[1], &clippedRect[0], &clippedRect[1]))
		return;

	DrawSetTexture(0);
	DrawQuad(clippedRect[0], clippedRect[1], m_DrawColor);
}

void CSurface::DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal)
{
	float alphaScale = m_DrawColor[3] / 255.f;
	unsigned char colors[4][4] = { 0 };

	alpha0 *= alphaScale;
	alpha1 *= alphaScale;

	if (alpha0 == 0 && alpha1 == 0)
		return;

	Vertex_t rect[2];
	Vertex_t clippedRect[2];

	InitVertex(rect[0], x0, y0, 0, 0);
	InitVertex(rect[1], x1, y1, 0, 0);

	if (!ClipRect(rect[0], rect[1], &clippedRect[0], &clippedRect[1]))
		return;

	DrawSetTexture(0);

	for (int i = 0; i < 4; i++)
	{
		Q_memcpy(colors[i], m_DrawColor, 3);
	}

	unsigned char nAlpha0 = (alpha0 & 0xFF);
	unsigned char nAlpha1 = (alpha1 & 0xFF);

	if (bHorizontal)
	{
		colors[0][3] = nAlpha0;
		colors[1][3] = nAlpha1;
		colors[2][3] = nAlpha1;
		colors[3][3] = nAlpha0;
	}
	else
	{
		colors[0][3] = nAlpha0;
		colors[1][3] = nAlpha0;
		colors[2][3] = nAlpha1;
		colors[3][3] = nAlpha1;
	}

	qglDisable(GL_TEXTURE_2D);
	qglBegin(GL_QUADS);
	qglColor4ub(colors[0][0], colors[0][1], colors[0][2], colors[0][3]);
	qglTexCoord2f(clippedRect[0].m_TexCoord.x, clippedRect[0].m_TexCoord.y);
	qglVertex3f(clippedRect[0].m_Position.x, clippedRect[0].m_Position.y, m_flZPos);
	qglColor4ub(colors[1][0], colors[1][1], colors[1][2], colors[1][3]);
	qglTexCoord2f(clippedRect[1].m_TexCoord.x, clippedRect[1].m_TexCoord.y);
	qglVertex3f(clippedRect[1].m_Position.x, clippedRect[1].m_Position.y, m_flZPos);
	qglColor4ub(colors[2][0], colors[2][1], colors[2][2], colors[2][3]);
	qglTexCoord2f(clippedRect[2].m_TexCoord.x, clippedRect[2].m_TexCoord.y);
	qglVertex3f(clippedRect[2].m_Position.x, clippedRect[2].m_Position.y, m_flZPos);
	qglColor4ub(colors[3][0], colors[3][1], colors[3][2], colors[3][3]);
	qglTexCoord2f(clippedRect[3].m_TexCoord.x, clippedRect[3].m_TexCoord.y);
	qglVertex3f(clippedRect[3].m_Position.x, clippedRect[3].m_Position.y, m_flZPos);
	qglEnd();
	qglEnable(GL_TEXTURE_2D);
}

void CSurface::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
	if (m_DrawColor[3] == 0)
		return;

	DrawFilledRect(x0, y0, x1, y0 + 1);
	DrawFilledRect(x0, y1 - 1, x1, y1);
	DrawFilledRect(x0, y0 + 1, x0 + 1, y1 - 1);
	DrawFilledRect(x1 - 1, y0 + 1, x1, y1 - 1);
}

void CSurface::DrawLine(int x0, int y0, int x1, int y1)
{
	if (m_DrawColor[3] == 0)
		return;

	Vertex_t verts[2];
	verts[0].Init(Vector2D(x0, y0), Vector2D(0, 0));
	verts[1].Init(Vector2D(x1, y1), Vector2D(1, 1));

	DrawSetTexture(0);
	DrawTexturedLineInternal(verts[0], verts[1]);
}

void CSurface::DrawPolyLine(int *px, int *py, int numPoints)
{
	if (m_DrawColor[3] == 0)
		return;

	qglDisable(GL_TEXTURE_2D);
	qglColor4ub(m_DrawColor[0], m_DrawColor[1], m_DrawColor[2], m_DrawColor[3]);
	qglBegin(GL_LINES);

	for (int i = 0; i < numPoints ; i++)
	{
		int inext = (i + 1) % numPoints;

		Vertex_t verts[2];
		Vertex_t clippedVerts[2];

		int x0, y0, x1, y1;

		x0 = px[i];
		x1 = px[inext];
		y0 = py[i];
		y1 = py[inext];

		InitVertex(verts[0], x0, y0, 0, 0);
		InitVertex(verts[1], x1, y1, 1, 1);

		if (!ClipLine(verts, clippedVerts))
			continue;

		qglTexCoord2fv(clippedVerts[0].m_TexCoord.Base());
		qglVertex3f(clippedVerts[0].m_Position.x, clippedVerts[0].m_Position.y, m_flZPos);
		qglTexCoord2fv(clippedVerts[1].m_TexCoord.Base());
		qglVertex3f(clippedVerts[1].m_Position.x, clippedVerts[1].m_Position.y, m_flZPos);
	}

	qglEnd();
	qglEnable(GL_TEXTURE_2D);
}

void CSurface::DrawQuad(const Vertex_t &ul, const Vertex_t &lr, unsigned char *pColor)
{
	if (m_iBoundTexture)
		qglEnable(GL_TEXTURE_2D);
	else
		qglDisable(GL_TEXTURE_2D);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglEnable(GL_BLEND);
	qglColor4ub(pColor[0], pColor[1], pColor[2], pColor[3]);
	qglBegin(GL_QUADS);
	qglTexCoord2f(ul.m_TexCoord.x, ul.m_TexCoord.y);
	qglVertex3f(ul.m_Position.x, ul.m_Position.y, m_flZPos);
	qglTexCoord2f(lr.m_TexCoord.x, ul.m_TexCoord.y);
	qglVertex3f(lr.m_Position.x, ul.m_Position.y, m_flZPos);
	qglTexCoord2f(lr.m_TexCoord.x, lr.m_TexCoord.y);
	qglVertex3f(lr.m_Position.x, lr.m_Position.y, m_flZPos);
	qglTexCoord2f(ul.m_TexCoord.x, lr.m_TexCoord.y);
	qglVertex3f(ul.m_Position.x, lr.m_Position.y, m_flZPos);
	qglEnd();
	qglEnable(GL_TEXTURE_2D);
	qglDisable(GL_BLEND);
}

void CSurface::DrawQuadBlend(const Vertex_t &ul, const Vertex_t &lr, unsigned char *pColor)
{
	if (m_iBoundTexture)
		qglEnable(GL_TEXTURE_2D);
	else
		qglDisable(GL_TEXTURE_2D);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE);
	qglEnable(GL_BLEND);
	qglEnable(GL_ALPHA_TEST);
	qglColor4ub(pColor[0], pColor[1], pColor[2], pColor[3]);
	qglBegin(GL_QUADS);
	qglTexCoord2f(ul.m_TexCoord.x, ul.m_TexCoord.y);
	qglVertex3f(ul.m_Position.x, ul.m_Position.y, m_flZPos);
	qglTexCoord2f(lr.m_TexCoord.x, ul.m_TexCoord.y);
	qglVertex3f(lr.m_Position.x, ul.m_Position.y, m_flZPos);
	qglTexCoord2f(lr.m_TexCoord.x, lr.m_TexCoord.y);
	qglVertex3f(lr.m_Position.x, lr.m_Position.y, m_flZPos);
	qglTexCoord2f(ul.m_TexCoord.x, lr.m_TexCoord.y);
	qglVertex3f(ul.m_Position.x, lr.m_Position.y, m_flZPos);
	qglEnd();
	qglEnable(GL_TEXTURE_2D);
	qglDisable(GL_BLEND);
	qglDisable(GL_ALPHA_TEST);
}

void CSurface::DrawTexturedPolygon(int n, Vertex_t *pVertices)
{
	if ((n == 0) || (m_DrawColor[3] == 0))
		return;

	Vertex_t **ppClippedVerts;
	int iCount = ClipPolygon(n, pVertices, m_nTranslateX, m_nTranslateY, &ppClippedVerts);

	if (iCount <= 0)
		return;

	qglColor4ub(m_DrawColor[0], m_DrawColor[1], m_DrawColor[2], m_DrawColor[3]);
	qglBegin(GL_POLYGON);
	qglEnable(GL_TEXTURE_2D);

	for (int i = 0; i < iCount; ++i)
	{
		qglTexCoord2fv(ppClippedVerts[i]->m_TexCoord.Base());
		qglVertex3f(ppClippedVerts[i]->m_Position.x, ppClippedVerts[i]->m_Position.y, m_flZPos);
	}

	qglEnd();
	qglEnable(GL_TEXTURE_2D);
}

void CSurface::DrawSetTextFont(HFont font)
{
	m_hCurrentFont = font;
	g_pfnDrawSetTextFont(this, 0, font);
}

void CSurface::DrawSetTextColor(int r, int g, int b, int a)
{
	int adjustedAlpha = (a * m_flAlphaMultiplier);

	if (r != m_DrawTextColor[0] || g != m_DrawTextColor[1] || b != m_DrawTextColor[2] || adjustedAlpha != m_DrawTextColor[3])
	{
		DrawFlushText();

		m_DrawTextColor[0] = (unsigned char)r;
		m_DrawTextColor[1] = (unsigned char)g;
		m_DrawTextColor[2] = (unsigned char)b;
		m_DrawTextColor[3] = (unsigned char)adjustedAlpha;
	}

	g_pfnDrawSetTextColor(this, 0, r, g, b, a);
}

void CSurface::DrawSetTextColor(Color col)
{
	DrawSetTextColor(col[0], col[1], col[2], col[3]);
}

void CSurface::DrawSetTextPos(int x, int y)
{
	m_pDrawTextPos[0] = x;
	m_pDrawTextPos[1] = y;

	g_pSurface->DrawSetTextPos(x, y);
}

void CSurface::DrawGetTextPos(int &x, int &y)
{
	g_pSurface->DrawGetTextPos(x, y);
}

void CSurface::DrawPrintText(const wchar_t *text, int iTextLen)
{
	if (!text)
		return;

	if (!m_hCurrentFont)
		return;

	int x, y;
	DrawGetTextPos(x, y);

	x += m_nTranslateX;
	y += m_nTranslateY;

	int iTall = GetFontTall(m_hCurrentFont);
	int iLastTexId = -1;

	int iCount = 0;
	Vertex_t *pQuads = (Vertex_t *)stackalloc((2 * iTextLen) * sizeof(Vertex_t));

	int iTotalWidth = 0;

	for (int i = 0; i < iTextLen; ++i)
	{
		wchar_t ch = text[i];

		bool bUnderlined = FontManager().GetFontUnderlined(m_hCurrentFont);
		int abcA, abcB, abcC;
		GetCharABCwide(m_hCurrentFont, ch, abcA, abcB, abcC);
		iTotalWidth += abcA;

		int iWide = abcB;

		if (bUnderlined)
		{
			iWide += (abcA + abcC);
			x-= abcA;
		}

		if (!iswspace(ch) || bUnderlined)
		{
			int iTexId = 0;
			float *texCoords = NULL;

			if (!g_FontTextureCache.GetTextureForChar(m_hCurrentFont, ch, &iTexId, &texCoords))
				continue;

			Assert(texCoords);

			if (iTexId != iLastTexId)
			{
				if (iCount)
				{
					DrawSetTexture(iLastTexId);
					DrawQuadArray(iCount, pQuads, m_DrawTextColor);
					iCount = 0;
				}

				iLastTexId = iTexId;
			}

 			Vertex_t &ul = pQuads[2 * iCount];
 			Vertex_t &lr = pQuads[2 * iCount + 1];
			iCount++;

			ul.m_Position.x = x + iTotalWidth;
			ul.m_Position.y = y;
			lr.m_Position.x = ul.m_Position.x + iWide;
			lr.m_Position.y = ul.m_Position.y + iTall;

			ul.m_TexCoord[0] = texCoords[0];
			ul.m_TexCoord[1] = texCoords[1];
			lr.m_TexCoord[0] = texCoords[2];
			lr.m_TexCoord[1] = texCoords[3];
		}

		iTotalWidth += iWide + abcC;

		if (bUnderlined)
		{
			iTotalWidth -= abcC;
		}
	}

	if (iCount)
	{
		DrawSetTexture(iLastTexId);
		DrawQuadArray(iCount, pQuads, m_DrawTextColor);
	}

	DrawSetTextPos(x + iTotalWidth, y);
	stackfree(pQuads);
}

void CSurface::DrawUnicodeChar(wchar_t wch)
{
	CharRenderInfo info;
	info.additive = false;

	if (DrawGetUnicodeCharRenderInfo(wch, info))
		DrawRenderCharFromInfo(info);
}

void CSurface::DrawUnicodeCharAdd(wchar_t wch)
{
	CharRenderInfo info;
	info.additive = true;

	if (DrawGetUnicodeCharRenderInfo(wch, info))
		DrawRenderCharFromInfo(info);
}

bool CSurface::DrawGetUnicodeCharRenderInfo(wchar_t ch, CharRenderInfo &info)
{
	info.valid = false;

	if (!m_hCurrentFont)
	{
		return info.valid;
	}

	info.valid = true;
	info.ch = ch;

	info.currentFont = m_hCurrentFont;
	info.fontTall = GetFontTall(m_hCurrentFont);

	DrawGetTextPos(info.x, info.y);
	GetCharABCwide(m_hCurrentFont, ch, info.abcA, info.abcB, info.abcC);

	bool bUnderlined = FontManager().GetFontUnderlined(m_hCurrentFont);

	if (!bUnderlined)
	{
		info.x += info.abcA;
	}

	info.textureId = 0;
	float *texCoords = NULL;

	if (!g_FontTextureCache.GetTextureForChar(m_hCurrentFont, ch, &info.textureId, &texCoords))
	{
		info.valid = false;
		return info.valid;
	}

	int fontWide = info.abcB;

	if (bUnderlined)
	{
		fontWide += (info.abcA + info.abcC);
		info.x-= info.abcA;
	}

	InitVertex(info.verts[0], info.x, info.y, texCoords[0], texCoords[1]);
	InitVertex(info.verts[1], info.x + fontWide, info.y + info.fontTall, texCoords[2], texCoords[3]);

	info.shouldclip = true;
	return info.valid;
}

void CSurface::DrawRenderCharInternal(const CharRenderInfo &info)
{
	if (m_DrawTextColor[3] == 0)
		return;

	Vertex_t clippedRect[2];

	if (info.shouldclip)
	{
		if (!ClipRect(info.verts[0], info.verts[1], &clippedRect[0], &clippedRect[1]))
			return;
	}
	else
	{
		clippedRect[0] = info.verts[0];
		clippedRect[1] = info.verts[1];
	}

	if (m_BatchedCharInfos.Count() > MAX_BATCHED_CHAR_VERTS)
		DrawFlushText();

	if (info.additive)
	{
		DrawSetTexture(info.textureId);
		DrawQuadBlend(clippedRect[0], clippedRect[1], m_DrawTextColor);
	}
	else
	{
		CBatchedCharInfo batchedCharInfo;
		batchedCharInfo.verts[0] = clippedRect[0];
		batchedCharInfo.verts[1] = clippedRect[1];
		batchedCharInfo.textureId = info.textureId;
		m_BatchedCharInfos.AddToTail(batchedCharInfo);
	}
}

void CSurface::DrawRenderCharFromInfo(const CharRenderInfo &info)
{
	if (!info.valid)
		return;

	DrawRenderCharInternal(info);
	DrawSetTextPos(info.x + (info.abcB + info.abcC), info.y);
}

void CSurface::DrawFlushText(void)
{
	if (!m_BatchedCharInfos.Count())
		return;

	for (int i = 0; i < m_BatchedCharInfos.Count(); i++)
	{
		DrawSetTexture(m_BatchedCharInfos[i].textureId);
		DrawQuad(m_BatchedCharInfos[i].verts[0], m_BatchedCharInfos[i].verts[1], m_DrawTextColor);
	}

	m_BatchedCharInfos.RemoveAll();
}

IHTML *CSurface::CreateHTMLWindow(IHTMLEvents *events, VPANEL context)
{
#if defined (ENABLE_HTMLWINDOW)
	VPANEL parent = g_pIPanel->GetParent(context);

	if (!parent)
		return NULL;

	HtmlWindow *IE = new HtmlWindow(events, context, g_hWnd, m_bAllowJavaScript, SupportsFeature(DIRECT_HWND_RENDER));
	IE->SetVisible(g_pIPanel->IsVisible(parent));

	m_HtmlWindows.AddToTail(IE);
	return dynamic_cast<IHTML *>(IE);
#else
	Assert(0);
	return NULL;
#endif
}

void CSurface::PaintHTMLWindow(IHTML *htmlwin)
{
#if defined (ENABLE_HTMLWINDOW)
	HtmlWindow *IE = static_cast<HtmlWindow *>(htmlwin);

	if (IE)
	{
		HDC hdc = ::GetDC(reinterpret_cast<HWND>(g_hWnd));
		IE->OnPaint(hdc);
		::ReleaseDC(reinterpret_cast<HWND>(g_hWnd), hdc);
	}
#endif
}

void CSurface::DeleteHTMLWindow(IHTML *htmlwin)
{
#if defined (ENABLE_HTMLWINDOW)
	HtmlWindow *IE = dynamic_cast<HtmlWindow *>(htmlwin);

	if (IE)
	{
		m_HtmlWindows.FindAndRemove(IE);
		delete IE;
	}
#else
#error "GameUI now NEEDS the HTML component!!"
#endif
}

void CSurface::DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload)
{
	Texture *texture = GetTextureById(id);
	
	if (!texture || !texture->_filename || (texture->_filename && stricmp(filename, texture->_filename)) || forceReload)
	{
		char name[MAX_PATH];
		Q_snprintf(name, sizeof(name), "%s.tga", filename);

		int width, height;
		bool success = LoadTGA(name, m_TextureBuffer, sizeof(m_TextureBuffer), &width, &height);

		if (!success)
		{
			const char *psz = Q_stristr(name, "vgui/");

			if (psz)
				success = LoadTGA(name + strlen("vgui/"), m_TextureBuffer, sizeof(m_TextureBuffer), &width, &height);
		}

		if (!success)
		{
			Q_snprintf(name, sizeof(name), "%s.bmp", filename);

			if (!LoadBMP(name, m_TextureBuffer, sizeof(m_TextureBuffer), &width, &height))
			{
				const char *psz = Q_stristr(name, "vgui/");

				if (psz)
					success = LoadBMP(name + strlen("vgui/"), m_TextureBuffer, sizeof(m_TextureBuffer), &width, &height);
			}
			else
			{
				success = true;
			}
		}

		if (success)
		{
			if (IsPowerOfTwo(width) == false && IsPowerOfTwo(height) == false && engine->pfnGetCvarFloat("fs_perf_warnings"))
				engine->Con_DPrintf("fs_perf_warnings: Resampling non-power-of-2 image '%s' (%dx%d)\n", filename, width, height);

			DrawSetTextureRGBA(id, m_TextureBuffer, width, height, hardwareFilter, forceReload);
		}
		else
		{
			Msg("Error: texture file '%s' does not exist or is invalid\n", filename);
			return;
		}

		if (!texture)
		{
			texture = AllocTextureForId(id);
			memset(texture, 0, sizeof(Texture));
		}

		texture->_id = id;
		texture->_filename = filename;
		texture->_wide = width;
		texture->_tall = height;
		texture->_s0 = 0;
		texture->_t0 = 0;
		texture->_s1 = 1;
		texture->_t1 = 1;
	}
}

void CSurface::DrawSetTextureRGB(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	Texture *texture = GetTextureById(id);

	if (!texture || texture->_filename || forceReload)
	{
		if (!texture)
			texture = AllocTextureForId(id);

		texture->_id = id;
		texture->_filename = NULL;
		texture->_wide = wide;
		texture->_tall = tall;
		texture->_s0 = 0;
		texture->_t0 = 0;
		texture->_s1 = 1;
		texture->_t1 = 1;

		qglBindTexture(GL_TEXTURE_2D, id);

		if (hardwareFilter)
		{
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (g_iBPP == 32)
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_EXT, wide, tall, 0, GL_RGB, GL_UNSIGNED_BYTE, rgba);
		else
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1_EXT, wide, tall, 0, GL_RGB, GL_UNSIGNED_BYTE, rgba);
	}
}

void CSurface::DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	Texture *texture = GetTextureById(id);

	if (!texture || texture->_filename || forceReload)
	{
		if (!texture)
			texture = AllocTextureForId(id);

		texture->_wide = wide;
		texture->_tall = tall;
		texture->_s0 = 0;
		texture->_t0 = 0;
		texture->_s1 = 1;
		texture->_t1 = 1;

		qglBindTexture(GL_TEXTURE_2D, id);

		if (hardwareFilter)
		{
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (g_iBPP == 32)
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_EXT, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		else
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1_EXT, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
	}
}

void CSurface::DrawSetTextureBGR(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	Texture *texture = GetTextureById(id);

	if (!texture || texture->_filename || forceReload)
	{
		if (!texture)
			texture = AllocTextureForId(id);

		texture->_wide = wide;
		texture->_tall = tall;
		texture->_s0 = 0;
		texture->_t0 = 0;
		texture->_s1 = 1;
		texture->_t1 = 1;

		qglBindTexture(GL_TEXTURE_2D, id);

		if (hardwareFilter)
		{
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (g_iBPP == 32)
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_EXT, wide, tall, 0, GL_BGR, GL_UNSIGNED_BYTE, rgba);
		else
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1_EXT, wide, tall, 0, GL_BGR, GL_UNSIGNED_BYTE, rgba);
	}
}

void CSurface::DrawSetTextureBGRA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	Texture *texture = GetTextureById(id);

	if (!texture || texture->_filename || forceReload)
	{
		if (!texture)
			texture = AllocTextureForId(id);

		texture->_wide = wide;
		texture->_tall = tall;
		texture->_s0 = 0;
		texture->_t0 = 0;
		texture->_s1 = 1;
		texture->_t1 = 1;

		qglBindTexture(GL_TEXTURE_2D, id);

		if (hardwareFilter)
		{
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (g_iBPP == 32)
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_EXT, wide, tall, 0, GL_BGRA, GL_UNSIGNED_BYTE, rgba);
		else
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1_EXT, wide, tall, 0, GL_BGRA, GL_UNSIGNED_BYTE, rgba);
	}
}

void CSurface::DrawUpdateRegionTextureRGB(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
	Texture *texture = GetTextureById(nTextureID);

	qglBindTexture(GL_TEXTURE_2D, nTextureID);
	qglPixelStorei(GL_UNPACK_ROW_LENGTH, texture->_wide);
	qglTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, texture->_wide, tall, GL_RGBA, GL_UNSIGNED_BYTE, pchData + y * texture->_wide * 4);
	qglPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

void CSurface::DrawUpdateRegionTextureRGBA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
	Texture *texture = GetTextureById(nTextureID);

	qglBindTexture(GL_TEXTURE_2D, nTextureID);
	qglPixelStorei(GL_UNPACK_ROW_LENGTH, texture->_wide);
	qglTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, texture->_wide, tall, GL_RGBA, GL_UNSIGNED_BYTE, pchData + y * texture->_wide * 4);
	qglPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

void CSurface::DrawUpdateRegionTextureBGR(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
	Texture *texture = GetTextureById(nTextureID);

	qglBindTexture(GL_TEXTURE_2D, nTextureID);
	qglPixelStorei(GL_UNPACK_ROW_LENGTH, texture->_wide);
	qglTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, texture->_wide, tall, GL_BGR, GL_UNSIGNED_BYTE, pchData + y * texture->_wide * 4);
	qglPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

void CSurface::DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
	Texture *texture = GetTextureById(nTextureID);

	qglBindTexture(GL_TEXTURE_2D, nTextureID);
	qglPixelStorei(GL_UNPACK_ROW_LENGTH, texture->_wide);
	qglTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, texture->_wide, tall, GL_BGRA, GL_UNSIGNED_BYTE, pchData + y * texture->_wide * 4);
	qglPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

void CSurface::DrawSetTexture(int id)
{
	if (id != m_iBoundTexture)
	{
		m_iBoundTexture = id;
		m_pBoundTexture = GetTextureById(id);
	}

	g_pfnDrawSetTexture(this, 0, id);
}

void CSurface::DrawGetTextureSize(int id, int &wide, int &tall)
{
	Texture *texture = GetTextureById(id);

	if (!texture)
		return;

	wide = texture->_wide;
	tall = texture->_tall;
}

void CSurface::DrawTexturedRect(int x0, int y0, int x1, int y1)
{
	if (m_DrawColor[3] == 0)
		return;

	if (!m_pBoundTexture)
		return;

	Vertex_t rect[2];
	Vertex_t clippedRect[2];

	InitVertex(rect[0], x0, y0, m_pBoundTexture->_s0, m_pBoundTexture->_t0);
	InitVertex(rect[1], x1, y1, m_pBoundTexture->_s1, m_pBoundTexture->_t1);

	if (!ClipRect(rect[0], rect[1], &clippedRect[0], &clippedRect[1]))
		return;

	if (g_iVideoMode == VIDEOMODE_D3D)
	{
		clippedRect[0].m_TexCoord.x += 0.5 / m_pBoundTexture->_wide;
		clippedRect[0].m_TexCoord.y += 0.5 / m_pBoundTexture->_tall;
		clippedRect[1].m_TexCoord.x -= 0.5 / m_pBoundTexture->_wide;
		clippedRect[1].m_TexCoord.y -= 0.5 / m_pBoundTexture->_tall;
	}

	DrawQuad(clippedRect[0], clippedRect[1], m_DrawColor);
}

bool CSurface::IsTextureIDValid(int id)
{
	return (GetTextureById(id) != NULL);
}

int CSurface::CreateNewTextureID(bool procedural)
{
	return g_pfnCreateNewTextureID(this, 0, procedural);
}

void CSurface::DeleteTextureByID(int id)
{
	qglDeleteTextures(1, (const GLuint *)&id);
}

void CSurface::GetScreenSize(int &wide, int &tall)
{
	wide = g_iVideoWidth;
	tall = g_iVideoHeight;
}

void CSurface::SetAsTopMost(VPANEL panel, bool state)
{
	g_pSurface->SetAsTopMost(panel, state);
}

void CSurface::BringToFront(VPANEL panel)
{
	g_pSurface->BringToFront(panel);
}

void CSurface::SetForegroundWindow(VPANEL panel)
{
	g_pSurface->SetForegroundWindow(panel);
}

void CSurface::SetPanelVisible(VPANEL panel, bool state)
{
	g_pSurface->SetPanelVisible(panel, state);
}

void CSurface::SetMinimized(VPANEL panel, bool state)
{
	g_pSurface->SetMinimized(panel, state);
}

bool CSurface::IsMinimized(VPANEL panel)
{
	return g_pSurface->IsMinimized(panel);
}

void CSurface::FlashWindow(VPANEL panel, bool state)
{
	g_pSurface->FlashWindow(panel, state);
}

void CSurface::SetTitle(VPANEL panel, const wchar_t *title)
{
	g_pSurface->SetTitle(panel, title);
}

void CSurface::SetAsToolBar(VPANEL panel, bool state)
{
	g_pSurface->SetAsToolBar(panel, state);
}

void CSurface::CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput, bool kbInput)
{
	g_pSurface->CreatePopup(panel, minimised, showTaskbarIcon, disabled, mouseInput, kbInput);
}

void CSurface::SwapBuffers(VPANEL panel)
{
	g_pSurface->SwapBuffers(panel);
}

void CSurface::Invalidate(VPANEL panel)
{
	g_pSurface->Invalidate(panel);
}

void CSurface::SetCursor(HCursor cursor)
{
	if (m_hCurrentCursor != cursor)
	{
		m_hCurrentCursor = cursor;
		g_pfnSetCursor(this, 0, cursor);
	}
}

bool CSurface::IsCursorVisible(void)
{
	return m_hCurrentCursor != dc_none;
}

void CSurface::ApplyChanges(void)
{
	g_pSurface->ApplyChanges();
}

bool CSurface::IsWithin(int x, int y)
{
	return g_pSurface->IsWithin(x, y);
}

bool CSurface::HasFocus(void)
{
	bool result = g_pfnHasFocus(this, 0);

	if (!result)
	{
		HWND focus = ::GetFocus();

		for (int i = 0; i < m_HtmlWindows.Count(); i++)
		{
			if (m_HtmlWindows[i]->GetIEHWND() == focus)
				return true;
		}
	}

	return result;
}

bool CSurface::SupportsFeature(ISurface::SurfaceFeature_e feature)
{
	switch (feature)
	{
		case OUTLINE_FONTS: return true;
	}

	return g_pfnSupportsFeature(this, 0, feature);
}

void CSurface::RestrictPaintToSinglePanel(VPANEL panel)
{
	m_pRestrictedPanel = panel;
	g_pfnRestrictPaintToSinglePanel(this, 0, panel);
}

bool CSurface::IsPanelUnderRestrictedPanel(VPANEL panel)
{
	if (!m_pRestrictedPanel)
		return true;

	while (panel)
	{
		if (panel == m_pRestrictedPanel)
			return true;

		panel = g_pIPanel->GetParent(panel);
	}

	return false;
}

void CSurface::SetModalPanel(VPANEL panel)
{
	g_pSurface->SetModalPanel(panel);
}

VPANEL CSurface::GetModalPanel(void)
{
	return g_pSurface->GetModalPanel();
}

void CSurface::UnlockCursor(void)
{
	g_pfnUnlockCursor(this, 0);
}

void CSurface::LockCursor(void)
{
	g_pfnLockCursor(this, 0);
}

void CSurface::SetTranslateExtendedKeys(bool state)
{
	g_pSurface->SetTranslateExtendedKeys(state);
}

VPANEL CSurface::GetTopmostPopup(void)
{
	return g_pSurface->GetTopmostPopup();
}

void CSurface::SetTopLevelFocus(VPANEL panel)
{
	g_pSurface->SetTopLevelFocus(panel);
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
	char fullPath[MAX_PATH];
	g_pFullFileSystem->GetLocalPath(fontFileName, fullPath, sizeof(fullPath));
	return FontManager().AddCustomFontFile(fullPath);
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
	return g_pSurface->GetNotifyPanel();
}

void CSurface::SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text)
{
	g_pSurface->SetNotifyIcon(context, icon, panelToReceiveMessages, text);
}

void CSurface::PlaySound(const char *fileName)
{
	g_pSurface->PlaySound(fileName);
}

int CSurface::GetPopupCount(void)
{
	return g_pSurface->GetPopupCount();
}

VPANEL CSurface::GetPopup(int index)
{
	return g_pSurface->GetPopup(index);
}

bool CSurface::ShouldPaintChildPanel(VPANEL childPanel)
{
	return g_pSurface->ShouldPaintChildPanel(childPanel);
}

bool CSurface::RecreateContext(VPANEL panel)
{
	return g_pSurface->RecreateContext(panel);
}

void CSurface::AddPanel(VPANEL panel)
{
	g_pSurface->AddPanel(panel);
}

void CSurface::ReleasePanel(VPANEL panel)
{
	g_pSurface->ReleasePanel(panel);
}

void CSurface::MovePopupToFront(VPANEL panel)
{
	g_pSurface->MovePopupToFront(panel);
}

void CSurface::MovePopupToBack(VPANEL panel)
{
	g_pSurface->MovePopupToBack(panel);
}

void CSurface::SolveTraverse(VPANEL panel, bool forceApplySchemeSettings)
{
	g_pSurface->SolveTraverse(panel, forceApplySchemeSettings);
}

void CSurface::PaintTraverseEx(VPANEL panel, bool paintPopups, bool enableStencil)
{
	if (!g_pIPanel->IsVisible(panel))
		return;

	bool bTopLevelDraw = false;

	if (enableStencil)
	{
		if (g_iVideoMode != VIDEOMODE_OPENGL || !vgui_stencil_test->value)
			enableStencil = false;
	}

	if (g_bInDrawing == false)
	{
		bTopLevelDraw = true;
		StartDrawing();

		if (enableStencil)
		{
			qglClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			qglEnable(GL_STENCIL_TEST);
			qglStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			qglStencilFunc(GL_GEQUAL, 0, 0xFFFFFFFF);
			qglStencilMask(0xFFFFFFFF);
		}
	}

	int popups = GetPopupCount();

	if (panel == GetEmbeddedPanel())
	{
		for (int i = 0; i < popups; i++)
			g_pIPanel->Render_SetPopupVisible(GetPopup(i), false);

		g_pIPanel->PaintTraverse(panel, true);

		if (paintPopups && !enableStencil)
		{
			for (int i = 0; i < popups; i++)
			{
				if (g_pIPanel->Render_GetPopupVisible(GetPopup(i)))
					g_pIPanel->PaintTraverse(GetPopup(i), true);
			}
		}

		if (IsCursorVisible() && vgui_emulatemouse && vgui_emulatemouse->value)
		{
			int x, y;
			g_pInput->GetCursorPos(x, y);

			if (!m_pMouseTexture)
				m_pMouseTexture = g_pScheme->GetImage("gfx/vgui/mouse", false);

			m_pMouseTexture->SetPos(x, y);
			m_pMouseTexture->Paint();
		}
	}
	else
	{
		if (!paintPopups || !g_pIPanel->IsPopup(panel))
		{
			g_pIPanel->PaintTraverse(panel, true);
		}
	}

	float flOldZPos = m_flZPos;
	m_flZPos = 0.0;

	if (paintPopups && enableStencil)
	{
		if (popups > 254)
			Warning("Too many popups! Rendering will be bad!\n");

		int nStencilRef = 254;

		for (int i = popups - 1; i >= 0; --i)
		{
			VPANEL popupPanel = GetPopup(i);

			if (!popupPanel)
				continue;

			if (!g_pIPanel->IsFullyVisible(popupPanel))
				continue;

			if (!IsPanelUnderRestrictedPanel(popupPanel))
				continue;

			if (enableStencil)
				qglStencilFunc(GL_GEQUAL, nStencilRef, 0xFFFFFFFF);

			nStencilRef--;
			m_flZPos = ((float)(i) / (float)popups);
			g_pIPanel->PaintTraverse(popupPanel, true);
		}
	}

	m_flZPos = flOldZPos;

	if (enableStencil)
		qglDisable(GL_STENCIL_TEST);

	if (bTopLevelDraw)
		FinishDrawing();
}

void CSurface::PaintTraverse(VPANEL panel)
{
	return PaintTraverseEx(panel, true, true);
	return g_pfnPaintTraverse(this, 0, panel);
}

void CSurface::EnableMouseCapture(VPANEL panel, bool state)
{
	g_pSurface->EnableMouseCapture(panel, state);
}

void CSurface::GetWorkspaceBounds(int &x, int &y, int &wide, int &tall)
{
	g_pSurface->GetWorkspaceBounds(x, y, wide, tall);
}

void CSurface::GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall)
{
	g_pSurface->GetAbsoluteWindowBounds(x, y, wide, tall);
}

void CSurface::GetProportionalBase(int &width, int &height)
{
	g_pSurface->GetProportionalBase(width, height);
}

void CSurface::CalculateMouseVisible(void)
{
	return g_pfnCalculateMouseVisible(this, 0);
}

bool CSurface::NeedKBInput(void)
{
	return g_pSurface->NeedKBInput();
}

bool CSurface::HasCursorPosFunctions(void)
{
	return g_pSurface->HasCursorPosFunctions();
}

void CSurface::SurfaceGetCursorPos(int &x, int &y)
{
	g_pSurface->SurfaceGetCursorPos(x, y);
}

void CSurface::SurfaceSetCursorPos(int x, int y)
{
	return g_pfnSurfaceSetCursorPos(this, 0, x, y);
}

void CSurface::DrawTexturedPolygon(int *p, int n)
{
	g_pSurface->DrawTexturedPolygon(p, n);
}

int CSurface::GetFontAscent(HFont font, wchar_t wch)
{
	return FontManager().GetFontAscent(font, wch);
	return g_pfnGetFontAscent(this, 0, font, wch);
}

void CSurface::SetAllowHTMLJavaScript(bool state)
{
	m_bAllowJavaScript = state;
}

void CSurface::DrawSetAlphaMultiplier(float alpha)
{
	m_flAlphaMultiplier = alpha;
}

float CSurface::DrawGetAlphaMultiplier(void)
{
	return m_flAlphaMultiplier;
}

void CSurface::DumpFontTextures(void)
{
	g_FontTextureCache.DumpPageTextures();
}

void Surface_InstallHook(vgui::ISurface *pSurface)
{
	DWORD *pVFTable = *(DWORD **)&g_Surface;

	g_pSurface = pSurface;
	g_pfnCWin32Font_GetCharRGBA = (void (__fastcall *)(void *, int, int, int, int, int, int, unsigned char *))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, CWIN32FONT_GETCHARABCWIDTHS_SIG, sizeof(CWIN32FONT_GETCHARABCWIDTHS_SIG) - 1);
	g_hCWin32Font_GetCharRGBA = g_pMetaHookAPI->InlineHook(g_pfnCWin32Font_GetCharRGBA, CWin32Font_GetCharRGBA, (void *&)g_pfnCWin32Font_GetCharRGBA);

	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 1, (void *)pVFTable[1], (void *&)g_pfnSurface_Shutdown);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 2, (void *)pVFTable[2], (void *&)g_pfnSurface_RunFrame);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 5, (void *)pVFTable[5], (void *&)g_pfnPushMakeCurrent);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 6, (void *)pVFTable[6], (void *&)g_pfnPopMakeCurrent);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 8, (void *)pVFTable[8], (void *&)g_pfnDrawSetColor);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 7, (void *)pVFTable[7], (void *&)g_pfnDrawSetColor2);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 9, (void *)pVFTable[9], (void *&)g_pfnDrawFilledRect);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 10, (void *)pVFTable[10], (void *&)g_pfnDrawOutlinedRect);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 11, (void *)pVFTable[11], (void *&)g_pfnDrawLine);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 12, (void *)pVFTable[12], (void *&)g_pfnDrawPolyLine);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 13, (void *)pVFTable[13], (void *&)g_pfnDrawSetTextFont);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 15, (void *)pVFTable[15], (void *&)g_pfnDrawSetTextColor);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 14, (void *)pVFTable[14], (void *&)g_pfnDrawSetTextColor2);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 18, (void *)pVFTable[18], (void *&)g_pfnDrawPrintText);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 19, (void *)pVFTable[19], (void *&)g_pfnDrawUnicodeChar);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 20, (void *)pVFTable[20], (void *&)g_pfnDrawUnicodeCharAdd);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 21, (void *)pVFTable[21], (void *&)g_pfnDrawFlushText);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 22, (void *)pVFTable[22], (void *&)g_pfnCreateHTMLWindow);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 23, (void *)pVFTable[23], (void *&)g_pfnPaintHTMLWindow);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 24, (void *)pVFTable[24], (void *&)g_pfnDeleteHTMLWindow);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 25, (void *)pVFTable[25], (void *&)g_pfnDrawSetTextureFile);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 26, (void *)pVFTable[26], (void *&)g_pfnDrawSetTextureRGBA);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 27, (void *)pVFTable[27], (void *&)g_pfnDrawSetTexture);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 28, (void *)pVFTable[28], (void *&)g_pfnDrawGetTextureSize);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 29, (void *)pVFTable[29], (void *&)g_pfnDrawTexturedRect);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 30, (void *)pVFTable[30], (void *&)g_pfnIsTextureIDValid);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 31, (void *)pVFTable[31], (void *&)g_pfnCreateNewTextureID);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 32, (void *)pVFTable[32], (void *&)g_pfnGetScreenSize);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 49, (void *)pVFTable[49], (void *&)g_pfnHasFocus);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 50, (void *)pVFTable[50], (void *&)g_pfnSupportsFeature);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 51, (void *)pVFTable[51], (void *&)g_pfnRestrictPaintToSinglePanel);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 59, (void *)pVFTable[59], (void *&)g_pfnCreateFont);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 60, (void *)pVFTable[60], (void *&)g_pfnAddGlyphSetToFont);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 61, (void *)pVFTable[61], (void *&)g_pfnAddCustomFontFile);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 62, (void *)pVFTable[62], (void *&)g_pfnGetFontTall);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 63, (void *)pVFTable[63], (void *&)g_pfnGetCharABCwide);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 64, (void *)pVFTable[64], (void *&)g_pfnGetCharacterWidth);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 65, (void *)pVFTable[65], (void *&)g_pfnGetTextSize);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 78, (void *)pVFTable[78], (void *&)g_pfnPaintTraverse);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 89, (void *)pVFTable[89], (void *&)g_pfnGetFontAscent);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 90, (void *)pVFTable[90], (void *&)g_pfnSetAllowHTMLJavaScript);
#if 0
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 4, (void *)pVFTable[4], (void *&)g_pfnSetEmbeddedPanel);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 54, (void *)pVFTable[54], (void *&)g_pfnUnlockCursor);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 55, (void *)pVFTable[55], (void *&)g_pfnLockCursor);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 45, (void *)pVFTable[45], (void *&)g_pfnSetCursor);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 46, (void *)pVFTable[46], (void *&)g_pfnIsCursorVisible);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 83, (void *)pVFTable[83], (void *&)g_pfnCalculateMouseVisible);
	g_pMetaHookAPI->VFTHook(g_pSurface, 0, 87, (void *)pVFTable[87], (void *&)g_pfnSurfaceSetCursorPos);
#endif
}

void ISurface::DrawSetAlphaMultiplier(float alpha)
{
	((CSurface *)this)->DrawSetAlphaMultiplier(alpha);
}

float ISurface::DrawGetAlphaMultiplier(void)
{
	return ((CSurface *)this)->DrawGetAlphaMultiplier();
}

void ISurface::DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal)
{
	return ((CSurface *)this)->DrawFilledRectFade(x0, y0, x1, y1, alpha0, alpha1, bHorizontal);
}

bool ISurface::DrawGetTextureFile(int id, char *filename, int maxlen)
{
	Texture *texture = GetTextureById(id);

	if (!texture)
		return false;

	Q_strncpy(filename, texture->_filename, maxlen);
	return true;
}

int ISurface::DrawGetTextureId(char const *filename)
{
	int i = m_VGuiSurfaceTextures.FirstInorder();

	while (i != m_VGuiSurfaceTextures.InvalidIndex())
	{
		Texture *texture = &m_VGuiSurfaceTextures[i];

		if (texture->_filename)
		{
			if (!Q_stricmp(filename, texture->_filename))
				return texture->_id;
		}

		i = m_VGuiSurfaceTextures.NextInorder(i);
	}

	return -1;
}

void ISurface::DrawTexturedPolygon(int n, Vertex_t *pVertices)
{
	g_Surface.DrawTexturedPolygon(n, (Vertex_t *)pVertices);
}

void ISurface::DrawTexturedSubRect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1)
{
	Assert(0);
}

bool GetIconSize(ICONINFO &iconInfo, int &w, int &h)
{
	w = h = 0;

	HBITMAP bitmap = iconInfo.hbmColor;
	BITMAP bm;

	if (0 == GetObject((HGDIOBJ)bitmap, sizeof(BITMAP), (LPVOID)&bm))
		return false;

	w = bm.bmWidth;
	h = bm.bmHeight;
	return true;
}

class CIconImage : public IImage
{
public:
	CIconImage(HICON hIcon) : m_hIcon(CopyIcon(hIcon))
	{
		ICONINFO iconInfo;

		m_Pos.x = m_Pos.y = 0;

		if (0 != GetIconInfo(m_hIcon, &iconInfo))
		{
			int w, h;
			GetIconSize(iconInfo, w, h);
			m_Size.cx = w;
			m_Size.cy = h;
		}
		else
		{
			m_Size.cx = 0;
			m_Size.cy = 0;
		}
	}

	virtual ~CIconImage(void)
	{
		DestroyIcon(m_hIcon);
	}

	virtual void Paint(void)
	{
		Assert(0);
	}

	virtual void SetPos(int x, int y)
	{
		m_Pos.x = x;
		m_Pos.y = y;
	}

	virtual void GetContentSize(int &wide, int &tall)
	{
		wide = m_Size.cx;
		tall = m_Size.cy;
	}

	virtual void GetSize(int &wide, int &tall)
	{
		GetContentSize(wide, tall);
	}

	virtual void SetSize(int wide, int tall)
	{
	}

	virtual void SetColor(Color col)
	{
	}

private:
	HICON m_hIcon;
	POINT m_Pos;
	SIZE m_Size;
};

static bool ShouldMakeUnique(char const *extension)
{
	if (!Q_stricmp(extension, "cur"))
		return true;

	if (!Q_stricmp(extension, "ani"))
		return true;

	return false;
}

IImage *ISurface::GetIconImageForFullPath(char const *pFullPath)
{
	IImage *newIcon = NULL;

	SHFILEINFO info = { 0 };
	DWORD_PTR dwResult = SHGetFileInfo(pFullPath, 0, &info, sizeof(info), SHGFI_TYPENAME | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SHELLICONSIZE);

	if (dwResult)
	{
		if (info.szTypeName[0] != 0)
		{
			char ext[32];
			Q_ExtractFileExtension(pFullPath, ext, sizeof(ext));

			char lookup[512];
			Q_snprintf(lookup, sizeof(lookup), "%s", ShouldMakeUnique(ext) ? pFullPath : info.szTypeName);

			unsigned short idx = m_FileTypeImages.Find(lookup);

			if (idx == m_FileTypeImages.InvalidIndex())
			{
				newIcon = new CIconImage(info.hIcon);
				idx = m_FileTypeImages.Insert(lookup, newIcon);
			}

			newIcon = m_FileTypeImages[idx];
		}

		DestroyIcon(info.hIcon);
	}

	return newIcon;
}

void ISurface::DumpFontTextures(void)
{
	g_Surface.DumpFontTextures();
}

void ISurface::DrawSetTextureRGB(int id, const unsigned char *rgb, int wide, int tall, int hardwareFilter, bool forceReload)
{
	g_Surface.DrawSetTextureRGB(id, rgb, wide, tall, hardwareFilter, forceReload);
}

void ISurface::DrawSetTextureBGR(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	g_Surface.DrawSetTextureBGR(id, rgba, wide, tall, hardwareFilter, forceReload);
}

void ISurface::DrawSetTextureBGRA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	g_Surface.DrawSetTextureBGRA(id, rgba, wide, tall, hardwareFilter, forceReload);
}

void ISurface::DrawUpdateRegionTextureRGB(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
	g_Surface.DrawUpdateRegionTextureRGB(nTextureID, x, y, pchData, wide, tall);
}

void ISurface::DrawUpdateRegionTextureRGBA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
	g_Surface.DrawUpdateRegionTextureRGBA(nTextureID, x, y, pchData, wide, tall);
}

void ISurface::DrawUpdateRegionTextureBGR(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
	g_Surface.DrawUpdateRegionTextureBGR(nTextureID, x, y, pchData, wide, tall);
}

void ISurface::DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
	g_Surface.DrawUpdateRegionTextureBGRA(nTextureID, x, y, pchData, wide, tall);
}

int ISurface::GetCharWidth(HFont font, wchar_t ch)
{
	if (!iswcntrl(ch))
	{
		int a, b, c;
		GetCharABCwide(font, ch, a, b, c);
		return (a + b + c);
	}

	return 0;
}

void ISurface::DeleteTextureByID(int id)
{
	g_Surface.DeleteTextureByID(id);
}

const char *ISurface::GetResolutionKey(void) const
{
	return NULL;
}