
#include "hud.h"
#include "parsemsg.h"
#include "DrawTargaImage.h"
#include "Fonts.h"
#include "util.h"

#include "hudmenu.h"

static CHudMenu g_HudMenu;
CHudMenu &HudMenu()
{
	return g_HudMenu;
}

cvar_t *cl_menu_font;

#define MAX_MENU_STRING 512

char g_szMenuString[MAX_MENU_STRING];
char g_szPrelocalisedMenuString[MAX_MENU_STRING];
wchar_t g_wszMenuString[MAX_MENU_STRING];

#define FADEIN_TIME 0.3
#define FADEOUT_TIME 0.2

enum menu_anim
{
	ANIM_NONE,
	ANIM_FADEIN,
	ANIM_FADEOUT
};

void CHudMenu::Init()
{
	m_iFlags |= HUD_ACTIVE;
	
	enable = gEngfuncs.pfnRegisterVariable("mh_drawmenu", "1", FCVAR_ARCHIVE);
}

void CHudMenu::VidInit()
{
	m_bCanDraw = false;
	m_bLargeFont = (ScreenWidth > 1024 && ScreenHeight > 768);
}

void CHudMenu::Draw(float flTime)
{
	OnThink(flTime);
	
	if (!m_bCanDraw)
		return;

	if (m_iW != -1)
		GL_DrawFullPanel(g_UI_Panel, m_iX - 10, m_iY, m_iW + 5, m_iH, m_iAlpha);

	int i = 0;
	int line = 0;

	int iX = m_iX;
	int iY = m_iY;

	m_bChangeColor = false;
	m_bFontShadow = false;

	if (1/*cl_menu_font->value*/)
	{
		while (g_wszMenuString[i])
		{
			m_bFontShadow = (line == 0);

			iX += DrawChar(g_wszMenuString[i], iX + (line ? 4 : 0), iY + 10);

			if (iX > m_iW)
				m_iW = iX + 4 + (line ? 4 : 0);

			if (g_wszMenuString[i] == '\n')
			{
				line ++;
				iX = m_iX;
				iY += m_bLargeFont ? 18 : 16;
			}

			i ++;
		}
	}
}

int CHudMenu::DrawChar(wchar_t ch, int iX, int iY)
{
	if (ch == 13)
		return 0;

	if (ch == '\\')
	{
		m_bChangeColor = true;
		return 0;
	}

	if (m_bChangeColor)
	{
		float a = float(m_iAlpha / 255.0);
		
		switch (ch)
		{
			case L'r': SetColor(1.0, 0.3, 0.3, a); break;
			case L'd': SetColor(0.4, 0.4, 0.4, a); break;
			case L'y': SetColor(1.0, 0.93, 0.3, a); break;
			case L'w': SetColor(0.95, 0.95, 0.95, a); break;
		}

		m_bChangeColor = false;
		return 0;
	}

	

	if (m_bFontShadow)
	{
		g_FontBold.SetWidth(m_bLargeFont ? 16 : 17);
		iY += 2;
		g_FontBold.SetColor(int(m_color.r * 0.5), m_color.g * 0.5, m_color.b * 0.5, m_color.a * 0.5);
		g_FontBold.DrawString(&ch, iX + 1, iY + (m_bLargeFont ? 15 : 14) + 1, 100);
		
		g_FontBold.SetColor(int(m_color.r), m_color.g, m_color.b, m_color.a);
		g_FontBold.DrawString(&ch, iX, iY + (m_bLargeFont ? 15 : 14), 100);
	}
	else
	{
		g_Font.SetWidth(m_bLargeFont ? 15 : 14);
		g_Font.SetColor(int(m_color.r), m_color.g, m_color.b, m_color.a);
		g_Font.DrawString(&ch, iX, iY + (m_bLargeFont ? 15 : 14), 100);
	}
	
	return g_Font.GetLen(&ch);
}

void CHudMenu::SetColor(float r, float g, float b, float a)
{
	m_color.r = r * 255;
	m_color.g = g * 255;
	m_color.b = b * 255;
	m_color.a = a * 255;
}

void CHudMenu::OnThink(float flTime)
{
	if (!m_bCanDraw)
		return;

	if (Hud().m_flTime > m_flShutoffTime - ANIM_FADEOUT && m_flShutoffTime != -1)
		Close();

	if (m_iAnimStatus == ANIM_FADEIN)
	{
		if (m_flAnimTime > flTime)
		{
			m_iAlpha = 255 - 255.0f * (m_flAnimTime - flTime) / FADEIN_TIME;
		}
		else
		{
			m_iAlpha = 255;
			m_iAnimStatus = ANIM_NONE;
		}
	}

	if (m_iAnimStatus == ANIM_FADEOUT)
	{
		if (m_flAnimTime > flTime)
		{
			m_iAlpha = 255.0f * (m_flAnimTime - flTime) / FADEOUT_TIME;
		}
		else
		{
			m_bCanDraw = false;
			m_iAnimStatus = ANIM_NONE;
		}
	}
}

void CHudMenu::Select(int item)
{
	if (!item)
		Close();
	
	if (m_bitsValidSlots & (1 << (item - 1)))
	{
		char szbuf[32];
		
		sprintf(szbuf, "menuselect %d\n", item);
		gEngfuncs.pfnClientCmd(szbuf);

		Close();
	}
}

void CHudMenu::Open()
{
	m_bCanDraw = true;
	m_flAnimTime = Hud().m_flTime + FADEIN_TIME;
	m_iAlpha = 0;
	m_iAnimStatus = ANIM_FADEIN;

	gEngfuncs.pfnPlaySoundByName("sound/ui/menu_show.wav", 1.0);

	CalcSize();
}

void CHudMenu::Close()
{
	m_flAnimTime = Hud().m_flTime + FADEOUT_TIME;
	m_iAnimStatus = ANIM_FADEOUT;

	gEngfuncs.pfnPlaySoundByName("sound/ui/menu_select.wav", 1.0);
}

void CHudMenu::CalcSize()
{
	m_iLine = 0;
	
	int i = 0;	
	while (g_szMenuString[i])
	{
		if (g_szMenuString[i] == '\n')
			m_iLine ++;

		i ++;
	}

	if (g_szPrelocalisedMenuString[0] == '#')
		m_iLine += 1; // dont know why

	m_iX = 20;
	m_iY = (ScreenHeight - m_iLine * (m_bLargeFont ? 18 : 16)) / 2;
	
	m_iH = m_iLine * (m_bLargeFont ? 18 : 16) + 32;
	m_iW = -1;
}

int CHudMenu::MsgFunc_ShowMenu(const char *pszName, int iSize, void *pbuf)
{
	if (enable->value == 0)
		return 0;
	
	BEGIN_READ(pbuf, iSize);

	m_bitsValidSlots = READ_SHORT();

	int DisplayTime = READ_CHAR();
	int NeedMore = READ_BYTE();

	if (DisplayTime > 0)
		m_flShutoffTime = DisplayTime + Hud().m_flTime;
	else
		m_flShutoffTime = -1;

	if (m_bitsValidSlots)
	{
		if (!m_bWaitingForMore)
			strncpy(g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING);
		else
			strncat(g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING - strlen(g_szPrelocalisedMenuString));

		g_szPrelocalisedMenuString[MAX_MENU_STRING - 1] = 0;

		if (!NeedMore)
		{
			strcpy(g_szMenuString, BufferedLocaliseTextString(g_szPrelocalisedMenuString));

			memset(g_wszMenuString, 0, sizeof(g_wszMenuString));
			wcscpy(g_wszMenuString, UTF8ToUnicode(g_szMenuString));
			
			Open();
		}
	}

	m_bWaitingForMore = (NeedMore == 1);

	return 1;
}

char *BufferedLocaliseTextString(const char *msg)
{
	static char dst_buffer[1024];
	
	LocaliseTextString(msg, dst_buffer, 1024);
	return dst_buffer;
}

char *LocaliseTextString(const char *msg, char *dst_buffer, int buffer_size)
{
	char *dst = dst_buffer;

	for (char *src = (char*)msg; *src != 0 && buffer_size > 0; buffer_size--)
	{
		if (*src == '#')
		{
			static char word_buf[255];
			char *wdst = word_buf, *word_start = src;

			for (++src ; (*src >= 'A' && *src <= 'z') || (*src >= '0' && *src <= '9'); wdst++, src++)
				*wdst = *src;

			*wdst = 0;

			client_textmessage_t *clmsg = TextMessageGet(word_buf);

			if (!clmsg || !(clmsg->pMessage))
			{
				src = word_start;
				*dst = *src;
				dst++, src++;
				continue;
			}

			for (char *wsrc = (char*)clmsg->pMessage; *wsrc != 0; wsrc++, dst++)
				*dst = *wsrc;

			*dst = 0;
		}
		else
		{
			*dst = *src;
			dst++, src++;
			*dst = 0;
		}
	}

	dst_buffer[buffer_size - 1] = 0;
	return dst_buffer;
}
