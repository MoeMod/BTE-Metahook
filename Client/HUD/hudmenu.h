#pragma once


extern cvar_t *cl_menu_font;

char *BufferedLocaliseTextString(const char *msg);
char *LocaliseTextString(const char *msg, char *dst_buffer, int buffer_size);
inline client_textmessage_t *TextMessageGet(const char *pName) { return gEngfuncs.pfnTextMessageGet(pName); }

class CHudMenu : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float flTime);
	void OnThink(float flTime);

	void Open();
	void Close();
	void CalcSize();

	void Select(int i);

	int DrawChar(char *ch);
	int DrawChar(wchar_t ch, int iX, int iY);

	void SetColor(float r, float g, float b, float a);

public:
	int MsgFunc_ShowMenu(const char *pszName, int iSize, void *pbuf);

public:
	bool m_bCanDraw;

private:
	cvar_t *enable;

	int m_iX, m_iY, m_iW, m_iH, m_iLine;
	int m_iAlpha;
	float m_flAnimTime;
	int m_iAnimStatus;
	bool m_bLargeFont;
	bool m_bChangeColor;
	bool m_bFontShadow;
	colorVec m_color;

	bool m_bWaitingForMore;
	int m_bitsValidSlots;
	float m_flShutoffTime;
};

CHudMenu &HudMenu();