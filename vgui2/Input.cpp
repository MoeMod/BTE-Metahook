#include <metahook.h>
#include <VGUI/VGUI.h>
#include <VGUI/ISurface.h>
#include <VGUI/IInput.h>
#include <VGUI/IInputInternal.h>
#include <VGUI/ISystem.h>
#include <VGUI/IVGui.h>
#include <VGUI/IPanel.h>

#include <UtlVector.h>
#include <UtlRBTree.h>
#include <UtlSymbol.h>

#include "Color.h"
#include "KeyValues.h"
#include "Encode.h"
#include "plugins.h"
#include "BaseUI.h"

#include <imm.h>
#pragma comment(lib, "Imm32.lib")

using namespace vgui;
class VPanel;

#define VGUI_GCS_COMPREADSTR 0x0001
#define VGUI_GCS_COMPREADATTR 0x0002
#define VGUI_GCS_COMPREADCLAUSE 0x0004
#define VGUI_GCS_COMPSTR 0x0008
#define VGUI_GCS_COMPATTR 0x0010
#define VGUI_GCS_COMPCLAUSE 0x0020
#define VGUI_GCS_CURSORPOS 0x0080
#define VGUI_GCS_DELTASTART 0x0100
#define VGUI_GCS_RESULTREADSTR 0x0200
#define VGUI_GCS_RESULTREADCLAUSE 0x0400
#define VGUI_GCS_RESULTSTR 0x0800
#define VGUI_GCS_RESULTCLAUSE 0x1000
#define VGUI_CS_INSERTCHAR 0x2000
#define VGUI_CS_NOMOVECARET 0x4000

bool (__fastcall *g_pfnInternalKeyCodePressed)(void *pthis, int, KeyCode code);
void (__fastcall *g_pfnInternalKeyCodeTyped)(void *pthis, int, KeyCode code);
void (__fastcall *g_pfnInternalKeyTyped)(void *pthis, int, wchar_t unichar);
void (__fastcall *g_pfnReleaseAppModalSurface)(void *pthis, int);
void (__fastcall *g_pfnSetCursorPos)(void *pthis, int, int x, int y);

class CInput : public IInputInternal
{
public:
	void RunFrame(void);
	void UpdateMouseFocus(int x, int y);
	void PanelDeleted(VPANEL panel);
	bool InternalCursorMoved(int x, int y);
	bool InternalMousePressed(MouseCode code);
	bool InternalMouseDoublePressed(MouseCode code);
	bool InternalMouseReleased(MouseCode code);
	bool InternalMouseWheeled(int delta);
	bool InternalKeyCodePressed(KeyCode code);
	void InternalKeyCodeTyped(KeyCode code);
	void InternalKeyTyped(wchar_t unichar);
	bool InternalKeyCodeReleased(KeyCode code);
	HInputContext CreateInputContext(void);
	void DestroyInputContext(HInputContext context);
	void AssociatePanelWithInputContext(HInputContext context, VPANEL pRoot);
	void ActivateInputContext(HInputContext context);
	VPANEL GetMouseCapture(void);
	bool IsChildOfModalPanel(VPANEL panel);
	void ResetInputContext(HInputContext context);
	void SetMouseFocus(VPANEL newMouseFocus);
	void SetMouseCapture(VPANEL panel);
	void GetKeyCodeText(KeyCode code, char *buf, int buflen);
	void SetFocus(VPANEL panel);
	VPANEL GetFocus(void);
	VPANEL GetMouseOver(void);
	void SetCursorPos(int x, int y);
	void GetCursorPos(int &x, int &y);
	bool WasMousePressed(MouseCode code);
	bool WasMouseDoublePressed(MouseCode code);
	bool IsMouseDown(MouseCode code);
	void SetCursorOveride(HCursor cursor);
	HCursor GetCursorOveride(void);
	bool WasMouseReleased(MouseCode code);
	bool WasKeyPressed(KeyCode code);
	bool IsKeyDown(KeyCode code);
	bool WasKeyTyped(KeyCode code);
	bool WasKeyReleased(KeyCode code);
	VPANEL GetAppModalSurface(void);
	void SetAppModalSurface(VPANEL panel);
	void ReleaseAppModalSurface(void);
	void GetCursorPosition(int &x, int &y);

public:
	void OnChangeIMEByHandle(int handleValue);
	void OnChangeIMEConversionModeByHandle(int handleValue);
	void OnChangeIMESentenceModeByHandle(int handleValue);
	void OnInputLanguageChanged(void);
	void OnIMEStartComposition(void);
	void OnIMEComposition(int flags);
	void OnIMEEndComposition(void);
	void OnIMEShowCandidates(void);
	void OnIMEChangeCandidates(void);
	void OnIMECloseCandidates(void);
	void OnIMERecomputeModes(void);

public:
	void SetIMEWindow(void *hwnd);
	void SetIMEEnabled(bool state);
	void *GetIMEWindow(void);
	void OnChangeIME(bool forward);
	int GetCurrentIMEHandle(void);
	int GetCurrentIMELanguage(void);
	int GetEnglishIMEHandle(void);
	void GetIMELanguageName(wchar_t *buf, int unicodeBufferSizeInBytes);
	void GetIMELanguageShortCode(wchar_t *buf, int unicodeBufferSizeInBytes);
	int GetIMELanguageList(LanguageItem *dest, int destcount);
	int GetIMEConversionModes(ConversionModeItem *dest, int destcount);
	int GetIMESentenceModes(SentenceModeItem *dest, int destcount);
	void ClearCompositionString(void);
	int GetCandidateListCount(void);
	void GetCandidate(int num, wchar_t *dest, int destSizeBytes);
	int GetCandidateListSelectedItem(void);
	int GetCandidateListPageSize(void);
	int GetCandidateListPageStart(void);
	void SetCandidateWindowPos(int x, int y);
	bool GetShouldInvertCompositionString(void);
	bool CandidateListStartsAtOne(void);
	void SetCandidateListPageStart(int start);
	void SetCompositionString(wchar_t *text);

public:
	void InternalSetCompositionString(const wchar_t *compstr);
	bool PostKeyMessage(KeyValues *message);
	void CreateNewCandidateList(void);
	void DestroyCandidateList(void);
	void InternalShowCandidateWindow(void);
	void InternalHideCandidateWindow(void);
	void InternalUpdateCandidateWindow(void);
	struct InputContext_s *GetInputContext(HInputContext context);
};

CInput g_Input;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CInput, IInput, VGUI_INPUT_INTERFACE_VERSION, g_Input); // export IInput to everyone else, not IInputInternal!
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CInput, IInputInternal, VGUI_INPUTINTERNAL_INTERFACE_VERSION, g_Input); // for use in external surfaces only! (like the engine surface)

IInputInternal *g_pInput;

typedef struct InputContext_s
{
	VPANEL _rootPanel;
	bool _mousePressed[MOUSE_LAST];
	bool _mouseDoublePressed[MOUSE_LAST];
	bool _mouseDown[MOUSE_LAST];
	bool _mouseReleased[MOUSE_LAST];
	bool _keyPressed[KEY_LAST];
	bool _keyTyped[KEY_LAST];
	bool _keyDown[KEY_LAST];
	bool _keyReleased[KEY_LAST];
	VPanel *_keyFocus;
	VPanel *_oldMouseFocus;
	VPanel *_mouseFocus;
	VPanel *_mouseOver;
	VPanel *_mouseCapture;
	VPanel *_appModalPanel;
	int m_nCursorX;
	int m_nCursorY;

	MouseCode m_MouseCaptureStartCode;
	int m_nLastPostedCursorX;
	int m_nLastPostedCursorY;
	int m_nExternallySetCursorX;
	int m_nExternallySetCursorY;
	bool m_bSetCursorExplicitly;
	CUtlVector<VPanel *> m_KeyCodeUnhandledListeners;
	VPanel *m_pModalSubTree;
	VPanel *m_pUnhandledMouseClickListener;
	bool m_bRestrictMessagesToModalSubTree;
}
InputContext_t;

HIMC m_hImc;
#define m_hContext (HInputContext)*((DWORD *)g_pInput + 231)

void CInput::SetMouseFocus(VPANEL newMouseFocus)
{
}

void CInput::SetMouseCapture(VPANEL panel)
{
}

void CInput::GetKeyCodeText(KeyCode code, char *buf, int buflen)
{
}

void CInput::SetFocus(VPANEL panel)
{
#if 1
	InputContext_t *pContext = GetInputContext(m_hContext);

	if (pContext)
		pContext->_keyFocus = (VPanel *)panel;
#else
	Assert(0);
#endif
}

VPANEL CInput::GetFocus(void)
{
	return (VPANEL)(GetInputContext(m_hContext)->_keyFocus);
}

VPANEL CInput::GetMouseOver(void)
{
	return NULL;
}

void CInput::SetCursorPos(int x, int y)
{
	g_pfnSetCursorPos(this, 0, x, y);
}

void CInput::GetCursorPos(int &x, int &y)
{
}

bool CInput::WasMousePressed(MouseCode code)
{
	return false;
}

bool CInput::WasMouseDoublePressed(MouseCode code)
{
	return false;
}

bool CInput::IsMouseDown(MouseCode code)
{
	return false;
}

void CInput::SetCursorOveride(HCursor cursor)
{
}

HCursor CInput::GetCursorOveride(void)
{
	return NULL;
}

bool CInput::WasMouseReleased(MouseCode code)
{
	return false;
}

bool CInput::WasKeyPressed(KeyCode code)
{
	return false;
}

bool CInput::IsKeyDown(KeyCode code)
{
	return false;
}

bool CInput::WasKeyTyped(KeyCode code)
{
	return false;
}

bool CInput::WasKeyReleased(KeyCode code)
{
	return false;
}

VPANEL CInput::GetAppModalSurface(void)
{
	return NULL;
}

void CInput::SetAppModalSurface(VPANEL panel)
{
}

void CInput::ReleaseAppModalSurface(void)
{
	g_pfnReleaseAppModalSurface(this, 0);
}

void CInput::GetCursorPosition(int &x, int &y)
{
}

void CInput::RunFrame(void)
{
}

void CInput::UpdateMouseFocus(int x, int y)
{
}

void CInput::PanelDeleted(VPANEL panel)
{
}

bool CInput::InternalCursorMoved(int x, int y)
{
	return false;
}

bool CInput::InternalMousePressed(MouseCode code)
{
	return false;
}

bool CInput::InternalMouseDoublePressed(MouseCode code)
{
	return false;
}

bool CInput::InternalMouseReleased(MouseCode code)
{
	return false;
}

bool CInput::InternalMouseWheeled(int delta)
{
	return false;
}

bool CInput::InternalKeyCodePressed(KeyCode code)
{
	return g_pfnInternalKeyCodePressed(this, 0, code);
}

void CInput::InternalKeyCodeTyped(KeyCode code)
{
	g_pfnInternalKeyCodeTyped(this, 0, code);
}

void CInput::InternalKeyTyped(wchar_t unichar)
{
	if (g_dwEngineBuildnum >= 5953)
		return g_pfnInternalKeyTyped(this, 0, unichar);

	InputContext_t *pContext = GetInputContext(m_hContext);
	// set key state
	if (unichar <= KEY_LAST)
	{
		pContext->_keyTyped[unichar] = 1;
	}
	
	/*// tell the current focused panel that a key was typed
	PostKeyMessage(new KeyValues("KeyTyped", "unichar", unichar));*/

	char cChar = (char)unichar;
	static bool bDoubleChar = false;
	static char sInpub[3] = { 0 };
	wchar_t *pComplete;

	if (bDoubleChar)
	{
		vgui::VPANEL focus = g_pInput->GetFocus();

		if (focus)
		{
			const char *module = g_pPanel->GetModuleName(focus);
			const char *name = g_pPanel->GetName(focus);

			//if (!strcmp(name, "ConsoleEntry") || !strcmp(name, "NameEntry"))
			{
				void *panel = g_pPanel->GetPanel(focus, module);

				if (panel)
				{
					void **pVtable = *(void ***)panel;

					sInpub[1] = cChar;
					bDoubleChar = false;
					pComplete = ANSIToUnicode(sInpub);

					//*(bool *)((char *)panel + 303) = true;
					//reinterpret_cast<void(__fastcall *)(void *, int, wchar_t)>(pVtable[128])(panel, 0, pComplete[0]);
					PostKeyMessage(new KeyValues("KeyTyped", "unichar", pComplete[0]));
					return;
				}
			}
		}

		bDoubleChar = false;
	}
	else if (IsDBCSLeadByte(cChar))
	{
		bDoubleChar = true;
		sInpub[0] = cChar;
		return;
	}

	g_pfnInternalKeyTyped(this, 0, unichar);
}

bool CInput::InternalKeyCodeReleased(KeyCode code)
{
	return false;
}

HInputContext CInput::CreateInputContext(void)
{
	return DEFAULT_INPUT_CONTEXT;
}

void CInput::DestroyInputContext(HInputContext context)
{
}

void CInput::AssociatePanelWithInputContext(HInputContext context, VPANEL pRoot)
{
}

void CInput::ActivateInputContext(HInputContext context)
{
}

VPANEL CInput::GetMouseCapture(void)
{
	return NULL;
}

bool CInput::IsChildOfModalPanel(VPANEL panel)
{
	return false;
}

void CInput::ResetInputContext(HInputContext context)
{
}

void *_imeWnd;
CANDIDATELIST *_imeCandidates;

enum LANGFLAG
{
	ENGLISH,
	TRADITIONAL_CHINESE,
	JAPANESE,
	KOREAN,
	SIMPLIFIED_CHINESE,
	UNKNOWN,

	NUM_IMES_SUPPORTED
}
LangFlag;

struct LanguageIds
{
	unsigned short id;
	int languageflag;
	wchar_t const *shortcode;
	wchar_t const *displayname;
	bool invertcomposition;
};

LanguageIds g_LanguageIds[] =
{
	{ 0x0000, UNKNOWN, L"", L"Neutral" },
	{ 0x007f, UNKNOWN, L"", L"Invariant" },
	{ 0x0400, UNKNOWN, L"", L"User Default Language" },
	{ 0x0800, UNKNOWN, L"", L"System Default Language" },
	{ 0x0436, UNKNOWN, L"AF", L"Afrikaans" },
	{ 0x041c, UNKNOWN, L"SQ", L"Albanian" },
	{ 0x0401, UNKNOWN, L"AR", L"Arabic (Saudi Arabia)" },
	{ 0x0801, UNKNOWN, L"AR", L"Arabic (Iraq)" },
	{ 0x0c01, UNKNOWN, L"AR", L"Arabic (Egypt)" },
	{ 0x1001, UNKNOWN, L"AR", L"Arabic (Libya)" },
	{ 0x1401, UNKNOWN, L"AR", L"Arabic (Algeria)" },
	{ 0x1801, UNKNOWN, L"AR", L"Arabic (Morocco)" },
	{ 0x1c01, UNKNOWN, L"AR", L"Arabic (Tunisia)" },
	{ 0x2001, UNKNOWN, L"AR", L"Arabic (Oman)" },
	{ 0x2401, UNKNOWN, L"AR", L"Arabic (Yemen)" },
	{ 0x2801, UNKNOWN, L"AR", L"Arabic (Syria)" },
	{ 0x2c01, UNKNOWN, L"AR", L"Arabic (Jordan)" },
	{ 0x3001, UNKNOWN, L"AR", L"Arabic (Lebanon)" },
	{ 0x3401, UNKNOWN, L"AR", L"Arabic (Kuwait)" },
	{ 0x3801, UNKNOWN, L"AR", L"Arabic (U.A.E.)" },
	{ 0x3c01, UNKNOWN, L"AR", L"Arabic (Bahrain)" },
	{ 0x4001, UNKNOWN, L"AR", L"Arabic (Qatar)" },
	{ 0x042b, UNKNOWN, L"HY", L"Armenian" },
	{ 0x042c, UNKNOWN, L"AZ", L"Azeri (Latin)" },
	{ 0x082c, UNKNOWN, L"AZ", L"Azeri (Cyrillic)" },
	{ 0x042d, UNKNOWN, L"ES", L"Basque" },
	{ 0x0423, UNKNOWN, L"BE", L"Belarusian" },
	{ 0x0445, UNKNOWN, L"", L"Bengali (India)" },
	{ 0x141a, UNKNOWN, L"", L"Bosnian (Bosnia and Herzegovina)" },
	{ 0x0402, UNKNOWN, L"BG", L"Bulgarian" },
	{ 0x0455, UNKNOWN, L"", L"Burmese" },
	{ 0x0403, UNKNOWN, L"CA", L"Catalan" },
	{ 0x0404, TRADITIONAL_CHINESE, L"CHT", L"#IME_0404", true },
	{ 0x0804, SIMPLIFIED_CHINESE, L"CHS", L"#IME_0804", true },
	{ 0x0c04, UNKNOWN, L"CH", L"Chinese (Hong Kong SAR, PRC)" },
	{ 0x1004, UNKNOWN, L"CH", L"Chinese (Singapore)" },
	{ 0x1404, UNKNOWN, L"CH", L"Chinese (Macao SAR)" },
	{ 0x041a, UNKNOWN, L"HR", L"Croatian" },
	{ 0x101a, UNKNOWN, L"HR", L"Croatian (Bosnia and Herzegovina)" },
	{ 0x0405, UNKNOWN, L"CZ", L"Czech" },
	{ 0x0406, UNKNOWN, L"DK", L"Danish" },
	{ 0x0465, UNKNOWN, L"MV", L"Divehi" },
	{ 0x0413, UNKNOWN, L"NL", L"Dutch (Netherlands)" },
	{ 0x0813, UNKNOWN, L"BE", L"Dutch (Belgium)" },
	{ 0x0409, ENGLISH, L"EN", L"#IME_0409" },
	{ 0x0809, ENGLISH, L"EN", L"English (United Kingdom)" },
	{ 0x0c09, ENGLISH, L"EN", L"English (Australian)" },
	{ 0x1009, ENGLISH, L"EN", L"English (Canadian)" },
	{ 0x1409, ENGLISH, L"EN", L"English (New Zealand)" },
	{ 0x1809, ENGLISH, L"EN", L"English (Ireland)" },
	{ 0x1c09, ENGLISH, L"EN", L"English (South Africa)" },
	{ 0x2009, ENGLISH, L"EN", L"English (Jamaica)" },
	{ 0x2409, ENGLISH, L"EN", L"English (Caribbean)" },
	{ 0x2809, ENGLISH, L"EN", L"English (Belize)" },
	{ 0x2c09, ENGLISH, L"EN", L"English (Trinidad)" },
	{ 0x3009, ENGLISH, L"EN", L"English (Zimbabwe)" },
	{ 0x3409, ENGLISH, L"EN", L"English (Philippines)" },
	{ 0x0425, UNKNOWN, L"ET", L"Estonian" },
	{ 0x0438, UNKNOWN, L"FO", L"Faeroese" },
	{ 0x0429, UNKNOWN, L"FA", L"Farsi" },
	{ 0x040b, UNKNOWN, L"FI", L"Finnish" },
	{ 0x040c, UNKNOWN, L"FR", L"#IME_040c" },
	{ 0x080c, UNKNOWN, L"FR", L"French (Belgian)" },
	{ 0x0c0c, UNKNOWN, L"FR", L"French (Canadian)" },
	{ 0x100c, UNKNOWN, L"FR", L"French (Switzerland)" },
	{ 0x140c, UNKNOWN, L"FR", L"French (Luxembourg)" },
	{ 0x180c, UNKNOWN, L"FR", L"French (Monaco)" },
	{ 0x0456, UNKNOWN, L"GL", L"Galician" },
	{ 0x0437, UNKNOWN, L"KA", L"Georgian" },
	{ 0x0407, UNKNOWN, L"DE", L"#IME_0407" },
	{ 0x0807, UNKNOWN, L"DE", L"German (Switzerland)" },
	{ 0x0c07, UNKNOWN, L"DE", L"German (Austria)" },
	{ 0x1007, UNKNOWN, L"DE", L"German (Luxembourg)" },
	{ 0x1407, UNKNOWN, L"DE", L"German (Liechtenstein)" },
	{ 0x0408, UNKNOWN, L"GR", L"Greek" },
	{ 0x0447, UNKNOWN, L"IN", L"Gujarati" },
	{ 0x040d, UNKNOWN, L"HE", L"Hebrew" },
	{ 0x0439, UNKNOWN, L"HI", L"Hindi" },
	{ 0x040e, UNKNOWN, L"HU", L"Hungarian" },
	{ 0x040f, UNKNOWN, L"IS", L"Icelandic" },
	{ 0x0421, UNKNOWN, L"ID", L"Indonesian" },
	{ 0x0434, UNKNOWN, L"", L"isiXhosa/Xhosa (South Africa)" },
	{ 0x0435, UNKNOWN, L"", L"isiZulu/Zulu (South Africa)" },
	{ 0x0410, UNKNOWN, L"IT", L"#IME_0410" },
	{ 0x0810, UNKNOWN, L"IT", L"Italian (Switzerland)" },
	{ 0x0411, JAPANESE, L"JP", L"#IME_0411" },
	{ 0x044b, UNKNOWN, L"IN", L"Kannada" },
	{ 0x0457, UNKNOWN, L"IN", L"Konkani" },
	{ 0x0412, KOREAN, L"KR", L"#IME_0412" },
	{ 0x0812, UNKNOWN, L"KR", L"Korean (Johab)" },
	{ 0x0440, UNKNOWN, L"KZ", L"Kyrgyz." },
	{ 0x0426, UNKNOWN, L"LV", L"Latvian" },
	{ 0x0427, UNKNOWN, L"LT", L"Lithuanian" },
	{ 0x0827, UNKNOWN, L"LT", L"Lithuanian (Classic)" },
	{ 0x042f, UNKNOWN, L"MK", L"FYRO Macedonian" },
	{ 0x043e, UNKNOWN, L"MY", L"Malay (Malaysian)" },
	{ 0x083e, UNKNOWN, L"MY", L"Malay (Brunei Darussalam)" },
	{ 0x044c, UNKNOWN, L"IN", L"Malayalam (India)" },
	{ 0x0481, UNKNOWN, L"", L"Maori (New Zealand)" },
	{ 0x043a, UNKNOWN, L"", L"Maltese (Malta)" },
	{ 0x044e, UNKNOWN, L"IN", L"Marathi" },
	{ 0x0450, UNKNOWN, L"MN", L"Mongolian" },
	{ 0x0414, UNKNOWN, L"NO", L"Norwegian (Bokmal)" },
	{ 0x0814, UNKNOWN, L"NO", L"Norwegian (Nynorsk)" },
	{ 0x0415, UNKNOWN, L"PL", L"Polish" },
	{ 0x0416, UNKNOWN, L"PT", L"Portuguese (Brazil)" },
	{ 0x0816, UNKNOWN, L"PT", L"Portuguese (Portugal)" },
	{ 0x0446, UNKNOWN, L"IN", L"Punjabi" },
	{ 0x046b, UNKNOWN, L"", L"Quechua (Bolivia)" },
	{ 0x086b, UNKNOWN, L"", L"Quechua (Ecuador)" },
	{ 0x0c6b, UNKNOWN, L"", L"Quechua (Peru)" },
	{ 0x0418, UNKNOWN, L"RO", L"Romanian" },
	{ 0x0419, UNKNOWN, L"RU", L"#IME_0419" },
	{ 0x044f, UNKNOWN, L"IN", L"Sanskrit" },
	{ 0x043b, UNKNOWN, L"", L"Sami, Northern (Norway)" },
	{ 0x083b, UNKNOWN, L"", L"Sami, Northern (Sweden)" },
	{ 0x0c3b, UNKNOWN, L"", L"Sami, Northern (Finland)" },
	{ 0x103b, UNKNOWN, L"", L"Sami, Lule (Norway)" },
	{ 0x143b, UNKNOWN, L"", L"Sami, Lule (Sweden)" },
	{ 0x183b, UNKNOWN, L"", L"Sami, Southern (Norway)" },
	{ 0x1c3b, UNKNOWN, L"", L"Sami, Southern (Sweden)" },
	{ 0x203b, UNKNOWN, L"", L"Sami, Skolt (Finland)" },
	{ 0x243b, UNKNOWN, L"", L"Sami, Inari (Finland)" },
	{ 0x0c1a, UNKNOWN, L"SR", L"Serbian (Cyrillic)" },
	{ 0x1c1a, UNKNOWN, L"SR", L"Serbian (Cyrillic, Bosnia, and Herzegovina)" },
	{ 0x081a, UNKNOWN, L"SR", L"Serbian (Latin)" },
	{ 0x181a, UNKNOWN, L"SR", L"Serbian (Latin, Bosnia, and Herzegovina)" },
	{ 0x046c, UNKNOWN, L"", L"Sesotho sa Leboa/Northern Sotho (South Africa)" },
	{ 0x0432, UNKNOWN, L"", L"Setswana/Tswana (South Africa)" },
	{ 0x041b, UNKNOWN, L"SK", L"Slovak" },
	{ 0x0424, UNKNOWN, L"SI", L"Slovenian" },
	{ 0x040a, UNKNOWN, L"ES", L"#IME_040a" },
	{ 0x080a, UNKNOWN, L"ES", L"Spanish (Mexican)" },
	{ 0x0c0a, UNKNOWN, L"ES", L"Spanish (Spain, Modern Sort)" },
	{ 0x100a, UNKNOWN, L"ES", L"Spanish (Guatemala)" },
	{ 0x140a, UNKNOWN, L"ES", L"Spanish (Costa Rica)" },
	{ 0x180a, UNKNOWN, L"ES", L"Spanish (Panama)" },
	{ 0x1c0a, UNKNOWN, L"ES", L"Spanish (Dominican Republic)" },
	{ 0x200a, UNKNOWN, L"ES", L"Spanish (Venezuela)" },
	{ 0x240a, UNKNOWN, L"ES", L"Spanish (Colombia)" },
	{ 0x280a, UNKNOWN, L"ES", L"Spanish (Peru)" },
	{ 0x2c0a, UNKNOWN, L"ES", L"Spanish (Argentina)" },
	{ 0x300a, UNKNOWN, L"ES", L"Spanish (Ecuador)" },
	{ 0x340a, UNKNOWN, L"ES", L"Spanish (Chile)" },
	{ 0x380a, UNKNOWN, L"ES", L"Spanish (Uruguay)" },
	{ 0x3c0a, UNKNOWN, L"ES", L"Spanish (Paraguay)" },
	{ 0x400a, UNKNOWN, L"ES", L"Spanish (Bolivia)" },
	{ 0x440a, UNKNOWN, L"ES", L"Spanish (El Salvador)" },
	{ 0x480a, UNKNOWN, L"ES", L"Spanish (Honduras)" },
	{ 0x4c0a, UNKNOWN, L"ES", L"Spanish (Nicaragua)" },
	{ 0x500a, UNKNOWN, L"ES", L"Spanish (Puerto Rico)" },
	{ 0x0430, UNKNOWN, L"", L"Sutu" },
	{ 0x0441, UNKNOWN, L"KE", L"Swahili (Kenya)" },
	{ 0x041d, UNKNOWN, L"SV", L"Swedish" },
	{ 0x081d, UNKNOWN, L"SV", L"Swedish (Finland)" },
	{ 0x045a, UNKNOWN, L"SY", L"Syriac" },
	{ 0x0449, UNKNOWN, L"IN", L"Tamil" },
	{ 0x0444, UNKNOWN, L"RU", L"Tatar (Tatarstan)" },
	{ 0x044a, UNKNOWN, L"IN", L"Telugu" },
	{ 0x041e, UNKNOWN, L"TH", L"#IME_041e" },
	{ 0x041f, UNKNOWN, L"TR", L"Turkish" },
	{ 0x0422, UNKNOWN, L"UA", L"Ukrainian" },
	{ 0x0420, UNKNOWN, L"PK", L"Urdu (Pakistan)" },
	{ 0x0820, UNKNOWN, L"IN", L"Urdu (India)" },
	{ 0x0443, UNKNOWN, L"UZ", L"Uzbek (Latin)" },
	{ 0x0843, UNKNOWN, L"UZ", L"Uzbek (Cyrillic)" },
	{ 0x042a, UNKNOWN, L"VN", L"Vietnamese" },
	{ 0x0452, UNKNOWN, L"", L"Welsh (United Kingdom)" },
};

static LanguageIds *GetLanguageInfo(unsigned short id)
{
	for (int j = 0; j < sizeof(g_LanguageIds) / sizeof(g_LanguageIds[0]); ++j)
	{
		if (g_LanguageIds[j].id == id)
			return &g_LanguageIds[j];
	}

	return NULL;
}

static bool IsIDInList(unsigned short id, int count, HKL *list)
{
	for (int i = 0; i < count; ++i)
	{
		if (LOWORD(list[i]) == id)
			return true;
	}

	return false;
}

static const wchar_t *GetLanguageName(unsigned short id)
{
	wchar_t const *name = L"???";

	for (int j = 0; j < sizeof(g_LanguageIds) / sizeof(g_LanguageIds[0]); ++j)
	{
		if (g_LanguageIds[j].id == id)
		{
			name = g_LanguageIds[j].displayname;
			break;
		}
	}

	return name;
}

WNDPROC g_ImeWndProc;

LRESULT ImeWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INPUTLANGCHANGE:
		{
			((CInput *)g_pInput)->OnInputLanguageChanged();
			return 0;
		}

		case WM_IME_CONTROL:
		{
			switch (wParam)
			{
				case IMC_SETCANDIDATEPOS:
				{
				}
			}

			return 0;
		}

		case WM_IME_SETCONTEXT:
		{
			lParam = 0;
			return 0;
		}

		case WM_IME_CHAR:
		{
			break;
		}

		case WM_IME_STARTCOMPOSITION:
		{
			((CInput *)g_pInput)->OnIMEStartComposition();
			return 0;
		}

		case WM_IME_COMPOSITION:
		{
			((CInput *)g_pInput)->OnIMEComposition(lParam);
			return 0;
		}

		case WM_IME_ENDCOMPOSITION:
		{
			((CInput *)g_pInput)->OnIMEEndComposition();
			return 0;
		}

		case WM_IME_NOTIFY:
		{
			switch (wParam)
			{
				case IMN_OPENCANDIDATE:
				{
					((CInput *)g_pInput)->OnIMEShowCandidates();
					return 0;
				}

				case IMN_CHANGECANDIDATE:
				{
					((CInput *)g_pInput)->OnIMEChangeCandidates();
					return 0;
				}

				case IMN_CLOSECANDIDATE:
				{
					((CInput *)g_pInput)->OnIMECloseCandidates();
					return 0;
				}

				case IMN_SETCONVERSIONMODE:
				{
					g_pInput->OnChangeIMEConversionModeByHandle(lParam);
					break;
				}

				case IMN_SETSENTENCEMODE:
				{
					g_pInput->OnChangeIMESentenceModeByHandle(lParam);
					break;
				}

				case IMN_OPENSTATUSWINDOW: return 0;
				case IMN_CLOSESTATUSWINDOW: return 0;
			}
		}
	}

	return CallWindowProc(g_ImeWndProc, hWnd, uMsg, wParam, lParam);
}

void CInput::SetIMEWindow(void *hwnd)
{
	_imeWnd = hwnd;

	HWND hWnd = (HWND)hwnd;
	HIMC hImc = ImmGetContext(hWnd);
	bool support = false;

	if (!hImc)
	{
		hImc = ImmCreateContext();

		if (hImc)
			goto support;
	}
	else
	{
support:
		ImmAssociateContext(hWnd, hImc);
		CreateWindow("Ime", "", WS_POPUP | WS_DISABLED, 0, 0, 0, 0, hWnd, NULL, GetModuleHandle(NULL), NULL);
		ImmReleaseContext(hWnd, hImc);
		support = true;
	}

	if (support)
	{
		g_ImeWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG)ImeWndProc);
	}

	m_hImc = hImc;
}

void CInput::SetIMEEnabled(bool state)
{
	ImmAssociateContext((HWND)_imeWnd, state ? m_hImc : NULL);
}

void *CInput::GetIMEWindow(void)
{
	return _imeWnd;
}

static void SpewIMEInfo(int langid)
{
	LanguageIds *info = GetLanguageInfo(langid);

	if (info)
	{
		wchar_t const *name = info->shortcode ? info->shortcode : L"???";
		wchar_t outstr[512];
		_snwprintf(outstr, sizeof(outstr) / sizeof(wchar_t), L"IME language changed to:  %s", name);
		OutputDebugStringW(outstr);
		OutputDebugStringW(L"\n");
	}
}

void CInput::OnChangeIME(bool forward)
{
	HKL currentKb = GetKeyboardLayout(0);
	UINT numKBs = GetKeyboardLayoutList(0, NULL);

	if (numKBs > 0)
	{
		HKL *list = new HKL[numKBs];
		GetKeyboardLayoutList(numKBs, list);

		int oldKb = 0;
		CUtlVector<HKL> selections;

		for (unsigned int i = 0; i < numKBs; ++i)
		{
			BOOL first = !IsIDInList(LOWORD(list[i]), i, list);

			if (!first)
				continue;

			selections.AddToTail(list[i]);

			if (list[i] == currentKb)
				oldKb = selections.Count() - 1;
		}

		oldKb += forward ? 1 : -1;

		if (oldKb < 0)
			oldKb = max(0, selections.Count() - 1);
		else if (oldKb >= selections.Count())
			oldKb = 0;

		ActivateKeyboardLayout(selections[oldKb], 0);

		int langid = LOWORD(selections[oldKb]);
		SpewIMEInfo(langid);

		delete [] list;
	}
}

int CInput::GetCurrentIMEHandle(void)
{
	HKL hkl = (HKL)GetKeyboardLayout(0);
	return (int)hkl;
}

int CInput::GetCurrentIMELanguage(void)
{
	return LOWORD(GetKeyboardLayout(0));
}

int CInput::GetEnglishIMEHandle(void)
{
	HKL hkl = (HKL)0x04090409;
	return (int)hkl;
}

void CInput::OnChangeIMEByHandle(int handleValue)
{
	HKL hkl = (HKL)handleValue;

	ActivateKeyboardLayout((HKL)handleValue, 0);
	SpewIMEInfo(LOWORD(hkl));
}

void CInput::GetIMELanguageName(wchar_t *buf, int unicodeBufferSizeInBytes)
{
	wchar_t const *name = GetLanguageName(GetCurrentIMELanguage());
	wcsncpy(buf, name, unicodeBufferSizeInBytes / sizeof(wchar_t) - 1);
	buf[unicodeBufferSizeInBytes / sizeof(wchar_t) - 1] = L'\0';
}

void CInput::GetIMELanguageShortCode(wchar_t *buf, int unicodeBufferSizeInBytes)
{
	LanguageIds *info = GetLanguageInfo(GetCurrentIMELanguage());

	if (!info)
	{
		buf[0] = L'\0';
	}
	else
	{
		wcsncpy(buf, info->shortcode, unicodeBufferSizeInBytes / sizeof(wchar_t) - 1);
		buf[unicodeBufferSizeInBytes / sizeof(wchar_t) - 1] = L'\0';
	}
}

int CInput::GetIMELanguageList(LanguageItem *dest, int destcount)
{
	int iret = 0;
	UINT numKBs = GetKeyboardLayoutList(0, NULL);

	if (numKBs > 0)
	{
		HKL *list = new HKL[numKBs];
		GetKeyboardLayoutList(numKBs, list);

		CUtlVector<HKL> selections;

		for (unsigned int i = 0; i < numKBs; ++i)
		{
			BOOL first = !IsIDInList(LOWORD(list[i]), i, list);

			if (!first)
				continue;

			selections.AddToTail(list[i]);
		}

		iret = selections.Count();

		if (dest)
		{
			for (int i = 0; i < min(iret,destcount); ++i)
			{
				HKL hkl = selections[i];
				CInput::LanguageItem *p = &dest[i];
				LanguageIds *info = GetLanguageInfo(LOWORD(hkl));

				memset(p, 0, sizeof(LanguageItem));

				wcsncpy(p->shortname, info->shortcode, sizeof(p->shortname) / sizeof(wchar_t));
				p->shortname[sizeof(p->shortname) / sizeof(wchar_t) - 1] = L'\0';

				wcsncpy(p->menuname, info->displayname, sizeof(p->menuname) / sizeof(wchar_t));
				p->menuname[sizeof(p->menuname) / sizeof(wchar_t) - 1 ] = L'\0';

				p->handleValue = (int)hkl;
				p->active = (hkl == GetKeyboardLayout(0)) ? true : false;
			}
		}

		delete [] list;
	}

	return iret;
}

struct IMESettingsTransform
{
	IMESettingsTransform(unsigned int cmr, unsigned int cma, unsigned int smr, unsigned int sma) : cmode_remove(cmr), cmode_add(cma), smode_remove(smr), smode_add(sma)
	{
	}

	void Apply(HWND hwnd)
	{
		HIMC hImc = ImmGetContext(hwnd);

		if (hImc)
		{
			DWORD dwConvMode, dwSentMode;
			ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);

			dwConvMode &= ~cmode_remove;
			dwSentMode &= ~smode_remove;

			ImmSetConversionStatus(hImc, dwConvMode, dwSentMode);

			dwConvMode |= cmode_add;
			dwSentMode |= smode_add;

			ImmSetConversionStatus(hImc, dwConvMode, dwSentMode);
			ImmReleaseContext(hwnd, hImc);
		}
	}

	bool ConvMatches(DWORD convFlags)
	{
		if (convFlags & cmode_remove)
			return false;

		if ((convFlags & cmode_add) == cmode_add)
			return true;

		return false;
	}

	bool SentMatches(DWORD sentFlags)
	{
		if (sentFlags & smode_remove)
			return false;

		if ((sentFlags & smode_add) == smode_add)
			return true;

		return false;
	}

	unsigned int cmode_remove;
	unsigned int cmode_add;
	unsigned int smode_remove;
	unsigned int smode_add;
};

static IMESettingsTransform g_ConversionMode_CHT_ToChinese(IME_CMODE_ALPHANUMERIC, IME_CMODE_NATIVE | IME_CMODE_LANGUAGE, 0, 0);
static IMESettingsTransform g_ConversionMode_CHT_ToEnglish(IME_CMODE_NATIVE | IME_CMODE_LANGUAGE, IME_CMODE_ALPHANUMERIC, 0, 0);
static IMESettingsTransform g_ConversionMode_CHS_ToChinese(IME_CMODE_ALPHANUMERIC, IME_CMODE_NATIVE | IME_CMODE_LANGUAGE, 0, 0);
static IMESettingsTransform g_ConversionMode_CHS_ToEnglish(IME_CMODE_NATIVE | IME_CMODE_LANGUAGE, IME_CMODE_ALPHANUMERIC, 0, 0);
static IMESettingsTransform g_ConversionMode_KO_ToKorean(IME_CMODE_ALPHANUMERIC, IME_CMODE_NATIVE | IME_CMODE_LANGUAGE, 0, 0);
static IMESettingsTransform g_ConversionMode_KO_ToEnglish(IME_CMODE_NATIVE | IME_CMODE_LANGUAGE, IME_CMODE_ALPHANUMERIC, 0, 0);
static IMESettingsTransform g_ConversionMode_JP_Hiragana(IME_CMODE_ALPHANUMERIC | IME_CMODE_KATAKANA, IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE, 0, 0);
static IMESettingsTransform g_ConversionMode_JP_DirectInput(IME_CMODE_NATIVE | (IME_CMODE_KATAKANA | IME_CMODE_LANGUAGE) | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN, IME_CMODE_ALPHANUMERIC, 0, 0);
static IMESettingsTransform g_ConversionMode_JP_FullwidthKatakana(IME_CMODE_ALPHANUMERIC, IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN | IME_CMODE_KATAKANA | IME_CMODE_LANGUAGE, 0, 0);
static IMESettingsTransform g_ConversionMode_JP_HalfwidthKatakana(IME_CMODE_ALPHANUMERIC | IME_CMODE_FULLSHAPE, IME_CMODE_NATIVE | IME_CMODE_ROMAN | (IME_CMODE_KATAKANA | IME_CMODE_LANGUAGE), 0, 0);
static IMESettingsTransform g_ConversionMode_JP_FullwidthAlphanumeric(IME_CMODE_NATIVE | (IME_CMODE_KATAKANA | IME_CMODE_LANGUAGE), IME_CMODE_ALPHANUMERIC | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN, 0, 0);
static IMESettingsTransform g_ConversionMode_JP_HalfwidthAlphanumeric(IME_CMODE_NATIVE | (IME_CMODE_KATAKANA | IME_CMODE_LANGUAGE) | IME_CMODE_FULLSHAPE, IME_CMODE_ALPHANUMERIC | IME_CMODE_ROMAN, 0, 0);

int CInput::GetIMEConversionModes(ConversionModeItem *dest, int destcount)
{
	if (dest)
		memset(dest, 0, destcount * sizeof(ConversionModeItem));

	DWORD dwConvMode = 0, dwSentMode = 0;
	HIMC hImc = ImmGetContext((HWND)GetIMEWindow());

	if (hImc)
	{
		ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
		ImmReleaseContext((HWND)GetIMEWindow(), hImc);
	}

	LanguageIds *info = GetLanguageInfo(LOWORD(GetKeyboardLayout(0)));

	switch (info->languageflag)
	{
		default: return 0;

		case TRADITIONAL_CHINESE:
		{
			if (dest)
			{
				ConversionModeItem *item;
				int i = 0;
				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_Chinese", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_CHT_ToChinese;
				item->active = g_ConversionMode_CHT_ToChinese.ConvMatches(dwConvMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_English", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_CHT_ToEnglish;
				item->active = g_ConversionMode_CHT_ToEnglish.ConvMatches(dwConvMode);
			}

			return 2;
		}

		case JAPANESE:
		{
			if (dest)
			{
				ConversionModeItem *item;

				int i = 0;
				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_Hiragana", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_JP_Hiragana;
				item->active = g_ConversionMode_JP_Hiragana.ConvMatches(dwConvMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_FullWidthKatakana", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_JP_FullwidthKatakana;
				item->active = g_ConversionMode_JP_FullwidthKatakana.ConvMatches(dwConvMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_FullWidthAlphanumeric", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_JP_FullwidthAlphanumeric;
				item->active = g_ConversionMode_JP_FullwidthAlphanumeric.ConvMatches(dwConvMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_HalfWidthKatakana", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_JP_HalfwidthKatakana;
				item->active = g_ConversionMode_JP_HalfwidthKatakana.ConvMatches(dwConvMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_HalfWidthAlphanumeric", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_JP_HalfwidthAlphanumeric;
				item->active = g_ConversionMode_JP_HalfwidthAlphanumeric.ConvMatches(dwConvMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_English", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_JP_DirectInput;
				item->active = g_ConversionMode_JP_DirectInput.ConvMatches(dwConvMode);
			}

			return 6;
		}

		case KOREAN:
		{
			if (dest)
			{
				ConversionModeItem *item;
				int i = 0;
				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_Korean", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_KO_ToKorean;
				item->active = g_ConversionMode_KO_ToKorean.ConvMatches(dwConvMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_English", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_KO_ToEnglish;
				item->active = g_ConversionMode_KO_ToEnglish.ConvMatches(dwConvMode);
			}

			return 2;
		}

		case SIMPLIFIED_CHINESE:
		{
			if (dest)
			{
				ConversionModeItem *item;
				int i = 0;
				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_Chinese", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_CHS_ToChinese;
				item->active = g_ConversionMode_CHS_ToChinese.ConvMatches(dwConvMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_English", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_ConversionMode_CHS_ToChinese;
				item->active = g_ConversionMode_CHS_ToChinese.ConvMatches(dwConvMode);
			}

			return 2;
		}
	}

	return 0;
}

static IMESettingsTransform g_SentenceMode_JP_None(0, 0, IME_SMODE_PLAURALCLAUSE | IME_SMODE_SINGLECONVERT | IME_SMODE_AUTOMATIC | IME_SMODE_PHRASEPREDICT | IME_SMODE_CONVERSATION, IME_SMODE_NONE);
static IMESettingsTransform g_SentenceMode_JP_General(0, 0, IME_SMODE_NONE | IME_SMODE_PLAURALCLAUSE | IME_SMODE_SINGLECONVERT | IME_SMODE_AUTOMATIC | IME_SMODE_CONVERSATION, IME_SMODE_PHRASEPREDICT);
static IMESettingsTransform g_SentenceMode_JP_BiasNames(0, 0, IME_SMODE_NONE | IME_SMODE_PHRASEPREDICT | IME_SMODE_SINGLECONVERT | IME_SMODE_AUTOMATIC | IME_SMODE_CONVERSATION, IME_SMODE_PLAURALCLAUSE);
static IMESettingsTransform g_SentenceMode_JP_BiasSpeech(0, 0, IME_SMODE_NONE | IME_SMODE_PHRASEPREDICT | IME_SMODE_SINGLECONVERT | IME_SMODE_AUTOMATIC | IME_SMODE_PLAURALCLAUSE, IME_SMODE_CONVERSATION);

int CInput::GetIMESentenceModes(SentenceModeItem *dest, int destcount)
{
	if (dest)
		memset(dest, 0, destcount * sizeof(SentenceModeItem));

	DWORD dwConvMode = 0, dwSentMode = 0;
	HIMC hImc = ImmGetContext((HWND)GetIMEWindow());

	if (hImc)
	{
		ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
		ImmReleaseContext((HWND)GetIMEWindow(), hImc);
	}

	LanguageIds *info = GetLanguageInfo(LOWORD(GetKeyboardLayout(0)));

	switch (info->languageflag)
	{
		default: return 0;

		case JAPANESE:
		{
			if (dest)
			{
				SentenceModeItem *item;

				int i = 0;
				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_General", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_SentenceMode_JP_General;
				item->active = g_SentenceMode_JP_General.SentMatches(dwSentMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_BiasNames", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_SentenceMode_JP_BiasNames;
				item->active = g_SentenceMode_JP_BiasNames.SentMatches(dwSentMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_BiasSpeech", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_SentenceMode_JP_BiasSpeech;
				item->active = g_SentenceMode_JP_BiasSpeech.SentMatches(dwSentMode);

				item = &dest[i++];
				wcsncpy(item->menuname, L"#IME_NoConversion", sizeof(item->menuname) / sizeof(wchar_t));
				item->handleValue = (int)&g_SentenceMode_JP_None;
				item->active = g_SentenceMode_JP_None.SentMatches(dwSentMode);
			}

			return 4;
		}
	}

	return 0;
}

void CInput::OnChangeIMEConversionModeByHandle(int handleValue)
{
	if (handleValue == 0)
		return;

	IMESettingsTransform *txform = (IMESettingsTransform *)handleValue;
	txform->Apply((HWND)GetIMEWindow());
}

void CInput::OnChangeIMESentenceModeByHandle(int handleValue)
{
}

void CInput::OnInputLanguageChanged(void)
{
}

void CInput::OnIMEStartComposition(void)
{
}

void DescribeIMEFlag(char const *string, bool value)
{
	if (value)
		Msg("   %s\n", string);
}

#define IMEDesc(x) DescribeIMEFlag(#x, flags & x);

void CInput::OnIMEComposition(int flags)
{
	HIMC hIMC = ImmGetContext((HWND)GetIMEWindow());

	if (hIMC)
	{
		if (flags & VGUI_GCS_RESULTSTR)
		{
			wchar_t tempstr[64];
			int len = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, (LPVOID)tempstr, sizeof(tempstr));

			if (len > 0)
			{
				if ((len % 2) != 0)
					len++;

				int numchars = len / sizeof(wchar_t);
				tempstr[numchars] = 0;

				PostKeyMessage(new KeyValues("InsertText", "wtext", tempstr));
				InternalSetCompositionString(L"");
			}
		}

		if (flags & VGUI_GCS_COMPSTR)
		{
			wchar_t tempstr[256];
			int len = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, (LPVOID)tempstr, sizeof(tempstr));

			if (len > 0)
			{
				if ((len % 2) != 0)
					len++;

				int numchars = len / sizeof(wchar_t);
				tempstr[numchars] = L'\0';
				InternalSetCompositionString(tempstr);
			}
		}

		ImmReleaseContext((HWND)GetIMEWindow(), hIMC);
	}
}

void CInput::OnIMEEndComposition(void)
{
	InputContext_t *pContext = GetInputContext(m_hContext);

	if (pContext)
		PostKeyMessage(new KeyValues("DoCompositionString", "string", L""));
}

void CInput::DestroyCandidateList(void)
{
	if (_imeCandidates)
	{
		delete [] (char *)_imeCandidates;
		_imeCandidates = null;
	}
}

void CInput::OnIMEShowCandidates(void)
{
	DestroyCandidateList();
	CreateNewCandidateList();

	InternalShowCandidateWindow();
}

void CInput::OnIMECloseCandidates(void)
{
	InternalHideCandidateWindow();
	DestroyCandidateList();
}

void CInput::OnIMEChangeCandidates(void)
{
	DestroyCandidateList();
	CreateNewCandidateList();

	InternalUpdateCandidateWindow();
}

void CInput::CreateNewCandidateList(void)
{
	Assert(!_imeCandidates);

	HIMC hImc = ImmGetContext((HWND)GetIMEWindow());

	if (hImc)
	{
		DWORD buflen = ImmGetCandidateListW(hImc, 0, NULL, 0);

		if (buflen > 0)
		{
			char *buf = new char[buflen];
			Q_memset(buf, 0, buflen);

			CANDIDATELIST *list = (CANDIDATELIST *)buf;
			DWORD copyBytes = ImmGetCandidateListW(hImc, 0, list, buflen);

			if (copyBytes > 0)
				_imeCandidates = list;
			else
				delete [] buf;
		}

		ImmReleaseContext((HWND)GetIMEWindow(), hImc);
	}
}

void CInput::ClearCompositionString(void)
{
	if (ImmGetOpenStatus(m_hImc))
	{
		ImmSetOpenStatus(m_hImc, false);
		ImmSetOpenStatus(m_hImc, true);
	}
}

int CInput::GetCandidateListCount(void)
{
	if (!_imeCandidates)
		return 0;

	return (int)_imeCandidates->dwCount;
}

void CInput::GetCandidate(int num, wchar_t *dest, int destSizeBytes)
{
	dest[0] = L'\0';

	if (num < 0 || num >= (int)_imeCandidates->dwCount)
		return;

	DWORD offset = *(DWORD *)((char *)(_imeCandidates->dwOffset + num));
	wchar_t *s = (wchar_t *)((char *)_imeCandidates + offset);

	wcsncpy(dest, s, destSizeBytes / sizeof(wchar_t) - 1);
	dest[destSizeBytes / sizeof(wchar_t) - 1] = L'\0';
}

int CInput::GetCandidateListSelectedItem(void)
{
	if (!_imeCandidates)
		return 0;

	return (int)_imeCandidates->dwSelection;
}

int CInput::GetCandidateListPageSize(void)
{
	if (!_imeCandidates)
		return 0;

	return (int)_imeCandidates->dwPageSize;
}

int CInput::GetCandidateListPageStart(void)
{
	if (!_imeCandidates)
		return 0;

	return (int)_imeCandidates->dwPageStart;
}

void CInput::SetCandidateListPageStart(int start)
{
	HIMC hImc = ImmGetContext((HWND)GetIMEWindow());

	if (hImc)
	{
		ImmNotifyIME(hImc, NI_SETCANDIDATE_PAGESTART, 0, start);
		ImmReleaseContext((HWND)GetIMEWindow(), hImc);
	}
}

void CInput::SetCompositionString(wchar_t *text)
{
}

void CInput::OnIMERecomputeModes(void)
{
}

bool CInput::CandidateListStartsAtOne(void)
{
	DWORD prop = ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);

	if (prop &	IME_PROP_CANDLIST_START_FROM_1)
		return true;

	return false;
}

void CInput::SetCandidateWindowPos(int x, int y)
{
	POINT point;
	CANDIDATEFORM Candidate;

	point.x = x;
	point.y = y;

	HIMC hIMC = ImmGetContext((HWND)GetIMEWindow());

	if (hIMC)
	{
		Candidate.dwIndex = 0;
		Candidate.dwStyle = CFS_FORCE_POSITION;
		Candidate.ptCurrentPos.x = point.x;
		Candidate.ptCurrentPos.y = point.y;
		ImmSetCandidateWindow(hIMC, &Candidate);

		ImmReleaseContext((HWND)GetIMEWindow(), hIMC);
	}
}

void CInput::InternalSetCompositionString(const wchar_t *compstr)
{
	InputContext_t *pContext = GetInputContext(m_hContext);

	if (pContext)
		PostKeyMessage(new KeyValues("DoCompositionString", "string", compstr));
}

void CInput::InternalShowCandidateWindow(void)
{
	InputContext_t *pContext = GetInputContext(m_hContext);

	if (pContext)
		PostKeyMessage(new KeyValues("DoShowIMECandidates"));
}

void CInput::InternalHideCandidateWindow(void)
{
	InputContext_t *pContext = GetInputContext(m_hContext);

	if (pContext)
		PostKeyMessage(new KeyValues("DoHideIMECandidates"));
}

void CInput::InternalUpdateCandidateWindow(void)
{
	InputContext_t *pContext = GetInputContext(m_hContext);

	if (pContext)
		PostKeyMessage(new KeyValues("DoUpdateIMECandidates"));
}

bool CInput::GetShouldInvertCompositionString(void)
{
	LanguageIds *info = GetLanguageInfo(LOWORD(GetKeyboardLayout(0)));

	if (!info)
		return false;

	return info->invertcomposition;
}

bool CInput::PostKeyMessage(KeyValues *message)
{
	InputContext_t *pContext = GetInputContext(m_hContext);

	if ((pContext->_keyFocus != NULL) && IsChildOfModalPanel((VPANEL)pContext->_keyFocus))
	{
		g_pVGui->PostMessage((VPANEL)pContext->_keyFocus, message, NULL);
		return true;
	}

	message->deleteThis();
	return false;
}

InputContext_t *CInput::GetInputContext(HInputContext context)
{
	if (context == -1)
		return (InputContext_t *)((DWORD)g_pInput + 440);
	else
		return (InputContext_t *)(*(DWORD *)((DWORD)g_pInput + 928) + 492 * context);
}

void Input_InstallHook(vgui::IInputInternal *pInput)
{
	DWORD *pVFTable = *(DWORD **)&g_Input;

	g_pInput = pInput;
	g_pMetaHookAPI->VFTHook(pInput, 0, 32, (void *)pVFTable[32], (void *&)g_pfnInternalKeyTyped);
}

void IInput::OnKeyCodeUnhandled(int keyCode)
{
	return;
}

VPANEL IInput::GetModalSubTree(void)
{
	return NULL;
}

bool IInput::ShouldModalSubTreeReceiveMessages(void) const
{
	return false;
}

void IInput::OnChangeIME(bool forward)
{
	((CInput *)this)->OnChangeIME(forward);
}

int IInput::GetCurrentIMEHandle(void)
{
	return ((CInput *)this)->GetCurrentIMEHandle();
}

int IInput::GetEnglishIMEHandle(void)
{
	return ((CInput *)this)->GetEnglishIMEHandle();
}

void IInput::GetIMELanguageShortCode(wchar_t *buf, int unicodeBufferSizeInBytes)
{
	((CInput *)this)->GetIMELanguageShortCode(buf, unicodeBufferSizeInBytes);
}

int IInput::GetIMELanguageList(LanguageItem *dest, int destcount)
{
	return ((CInput *)this)->GetIMELanguageList(dest, destcount);
}

int IInput::GetIMEConversionModes(ConversionModeItem *dest, int destcount)
{
	return ((CInput *)this)->GetIMEConversionModes(dest, destcount);
}

int IInput::GetIMESentenceModes(SentenceModeItem *dest, int destcount)
{
	return ((CInput *)this)->GetIMESentenceModes(dest, destcount);
}

void IInput::OnChangeIMEByHandle(int handleValue)
{
	((CInput *)this)->OnChangeIMEByHandle(handleValue);
}

void IInput::OnChangeIMEConversionModeByHandle(int handleValue)
{
	((CInput *)this)->OnChangeIMEConversionModeByHandle(handleValue);
}

void IInput::OnChangeIMESentenceModeByHandle(int handleValue)
{
	((CInput *)this)->OnChangeIMESentenceModeByHandle(handleValue);
}

void IInput::ClearCompositionString(void)
{
	return ((CInput *)this)->ClearCompositionString();
}

int IInput::GetCandidateListCount(void)
{
	return ((CInput *)this)->GetCandidateListCount();
}

void IInput::GetCandidate(int num, wchar_t *dest, int destSizeBytes)
{
	((CInput *)this)->GetCandidate(num, dest, destSizeBytes);
}

int IInput::GetCandidateListSelectedItem(void)
{
	return ((CInput *)this)->GetCandidateListSelectedItem();
}

int IInput::GetCandidateListPageSize(void)
{
	return ((CInput *)this)->GetCandidateListPageSize();
}

int IInput::GetCandidateListPageStart(void)
{
	return ((CInput *)this)->GetCandidateListPageStart();
}

void IInput::SetCandidateWindowPos(int x, int y)
{
	((CInput *)this)->SetCandidateWindowPos(x, y);
}

bool IInput::GetShouldInvertCompositionString(void)
{
	return ((CInput *)this)->GetShouldInvertCompositionString();
}

bool IInput::CandidateListStartsAtOne(void)
{
	return ((CInput *)this)->CandidateListStartsAtOne();
}

void IInput::SetCandidateListPageStart(int start)
{
	((CInput *)this)->SetCandidateListPageStart(start);
}

VPANEL IInput::GetMouseCapture(void)
{
	return NULL;
}
