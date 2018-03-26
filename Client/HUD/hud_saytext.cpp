#include "cdll_dll.h"
#include "hud.h"
#include "gl/gl.h"
#include "configs.h"
#include "parsemsg.h"
#include "BaseUI.h"
#include "hud_saytext.h"
#include "cmd.h"
#include "Fonts.h"
#include "util.h"
#include "Encode.h"

#include "DrawTabPanel.h"

static CHudSayText g_HudSayText;
CHudSayText &HudSayText()
{
	return g_HudSayText;
}

CHudSayText::~CHudSayText()
{
	line *pLine = head;
	while (pLine)
	{
		head = pLine;
		pLine = pLine->next;
		free(head);
		head = NULL;
	}
}


inline char *safe_strcpy(char *dst, const char *src, int len_dst)
{
	if (len_dst <= 0)
		return NULL;

	strncpy(dst, src, len_dst);
	dst[len_dst - 1] = '\0';

	return dst;
}

#define MSG_BUF_SIZE 128

char *LookupString(const char *msg, int *msg_dest = 0)
{
	if (!msg)
		return "";

	if (msg[0] == '#')
	{
		client_textmessage_t *clmsg = gEngfuncs.pfnTextMessageGet(msg + 1);

		if (!clmsg || !(clmsg->pMessage))
			return (char *)msg;

		if (msg_dest)
		{
			if (clmsg->effect < 0)
				*msg_dest = -clmsg->effect;
		}

		return (char *)clmsg->pMessage;
	}
	else
	{
		return (char *)msg;
	}
}

void StripEndNewlineFromString(char *str)
{
	int s = strlen(str) - 1;

	if (s >= 0)
	{
		if (str[s] == '\n' || str[s] == '\r')
			str[s] = 0;
	}
}

void StripEndNewlineFromString(wchar_t *str)
{
	int s = wcslen(str) - 1;

	if (s >= 0)
	{
		if (str[s] == L'\n' || str[s] == L'\r')
			str[s] = 0;
	}
}

// 3 = team 1 = yellow 4 = green

int team_drawing = 0;

int CheckColor(wchar_t ch)
{
	if (ch == 1 || ch == 2 || ch == 3 || ch == 4)
		return 0;

	return 1;
}

int CheckColor2(wchar_t ch)
{
	if (ch == 3)
	{
		switch (team_drawing)
		{
		case TEAM_CT: g_FontBold.SetColor(216, 81, 80, 255); return 0;
		case TEAM_TERRORIST: g_FontBold.SetColor(173, 201, 235, 255); return 0;
		case TEAM_SPECTATOR: g_FontBold.SetColor(200, 200, 200, 255); return 0;
		default: g_FontBold.SetColor(255, 255, 255, 255); return 0;
		}
	}
	
	switch (ch)
	{
	case 1: g_FontBold.SetColor(255, 255, 255, 255); return 0;
	case 2: g_FontBold.SetColor(173, 201, 235, 255); return 0;
	//case 3: g_Font.SetColor(173, 201, 235, 255); return 0;
	case 4: g_FontBold.SetColor(34, 177, 76, 255); return 0;
	}
	
	return 1;
}

void messagemode()
{
	//Hud().m_SayText.m_bInputing = true;
}

void messagemode2()
{
	//Hud().m_SayText.m_bInputing = true;
}

void CHudSayText::Init()
{
	//m_bInputing = false;
	//g_piEngineKeyDest = *(int **)((DWORD)g_pMetaHookAPI->SearchPattern((void *)((DWORD)gEngfuncs.VGui_ViewportPaintBackground + 0x10), 0x200, "\x85\xC0\x75\x2A\x83\x3D\x2A\x2A\x2A\x2A\x01", 10) + 0xE);
	m_iFlags |= HUD_ACTIVE;

	m_bInputing = *(int **)((DWORD)g_pMetaHookAPI->SearchPattern((void *)((DWORD)gEngfuncs.VGui_ViewportPaintBackground + 0x10), 0x200, "\x85\xC0\x75\x2A\x83\x3D\x2A\x2A\x2A\x2A\x01", 10) + 0xE);
}

void CHudSayText::VidInit()
{
	/*if (!gConfigs.bEnableNewHud)
		return;*/

	//m_bInputing = false;
	//hIMC = ImmAssociateContext(g_hWnd, NULL);
	m_bInputCheck = 0;

	m_iListCount = 0;
	m_iSelection = 0;
	m_iInsertPos = 0;

	memset(m_wszList, 0, sizeof(m_wszList));
	memset(m_wszComp, 0, sizeof(m_wszComp));

	//ImmAssociateContextEx(g_hWnd, NULL, IACE_CHILDREN);

	while (this->head != NULL)
	{
		line *temp = head->next;
		delete head;
		head = temp;
	}

	head = NULL;

	m_flScale = ((float)ScreenWidth / 1600);

	m_iFontSize = ScreenWidth > 1024 ? 17 : 15;
	m_iMaxLine = ScreenWidth > 1024 ? 7 : 5;
	m_iLineSpace = ScreenWidth > 1024 ? 5 : 3;
	m_iMaxWidth = 356 * m_flScale;
}

void CHudSayText::Think()
{
	//gEngfuncs.Con_Printf("%d\n", *m_bInputing); 
	
	if (m_bInputCheck != *m_bInputing)
	{
		if (*m_bInputing == 1)
		{
			m_iListCount = 0;
			m_iSelection = 0;
			m_iInsertPos = 0;
			m_iIMEMaxWidth = 0;

			memset(m_wszList, 0, sizeof(m_wszList));
			memset(m_wszComp, 0, sizeof(m_wszComp));
			
			//ImmAssociateContextEx(g_hWnd, NULL, IACE_DEFAULT);
			/*hIMC = ImmAssociateContext(g_hWnd, hIMC);
			//ImmAssociateContext(g_hWnd, hIMC);
			//hIMC = ImmGetContext(g_hWnd);
			//ImmSetOpenStatus(hIMC, TRUE);
			ImmReleaseContext(g_hWnd, hIMC);*/
			//gEngfuncs.Con_Printf("IME Enabled.\n");
		}
		else
		{
			//ImmAssociateContextEx(g_hWnd, NULL, IACE_CHILDREN);
			/*hIMC = ImmAssociateContext(g_hWnd, NULL);
			ImmReleaseContext(g_hWnd, hIMC);
			//hIMC = ImmGetContext(g_hWnd);
			//ImmSetOpenStatus(hIMC, FALSE);*/
			//gEngfuncs.Con_Printf("IME Disabled.\n");
		}
		
		m_bInputCheck = *m_bInputing;
	}
}

void CHudSayText::Draw(float time)
{
	Think();

	g_Font.SetWidth(m_iFontSize);

	if (*m_bInputing == 1)
	{
		wchar_t Comp[2][256];
		memset(Comp, 0, sizeof(Comp));
		wcscpy(Comp[0], m_wszComp);
		wcscpy(Comp[1], m_wszComp + m_iInsertPos);
		Comp[0][m_iInsertPos] = 0;

		int x = 20;
		int y = 80;
		int line = m_iFontSize + 5;

		int start = 0, end = m_iListCount;

		if (m_iListCount > 10)
		{
			start = 10 * (m_iSelection / 10);
			end = start + 10;

			if (end > m_iListCount)
				end = m_iListCount;
		}

		if (m_iListCount)
			gEngfuncs.pfnFillRGBABlend(x - 5, y - 23 - m_iFontSize - 3, m_iIMEMaxWidth - 3, line * (end - start) + 23 + 10, 0, 0, 0, 100);
		else
			m_iIMEMaxWidth = 0;

		g_Font.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		x = g_Font.DrawString(Comp[0], x, y - 23, 10000);

		if (wcslen(m_wszComp))
			gEngfuncs.pfnFillRGBABlend(x + 1, y - 23 - m_iFontSize, 3, m_iFontSize + 3, 153, 217, 234, 255);

		g_Font.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		int x2 = g_Font.DrawString(Comp[1], x + 5, y - 23, 10000);
		if (x2 > m_iIMEMaxWidth)
			m_iIMEMaxWidth = x2;

		x = 20;

		for (int i = start; i < end; i++)
		{
			if (i == m_iSelection)
			{
				int x2 = g_Font.GetLen(m_wszList[i]) + x;

				if (i == m_iSelection)
					gEngfuncs.pfnFillRGBABlend(x - 2, y + (i - start) * line + 5 - m_iFontSize, x2 - x + 4, m_iFontSize + 4, 200, 200, 200, 180);
			}
		}

		for (int i = start; i < end; i++)
		{
			/*if (i == m_iSelection)
			g_Font.SetColor(0.9f, 0.9f, 0.9f, 1.0f);
			else
			g_Font.SetColor(1.0f, 1.0f, 1.0f, 1.0f);*/

			if (i == m_iSelection)
				g_Font.SetColor(0.125f, 0.125f, 0.125f, 1.0f);
			else
				g_Font.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

			int x2 = g_Font.DrawString(m_wszList[i], x, y + (i - start) * line + 5, 10000);
			if (x2 > m_iIMEMaxWidth)
				m_iIMEMaxWidth = x2;
		}
	}

	if (!gConfigs.bEnableNewHud)
		return;
	
	line *m = head;
	line *last;

	while (m)
	{
		if (time > m->die)
		{
			line *temp = m;

			if (m == head)
			{
				head = m->next;
				m = head;
			}
			else
			{
				m = m->next;
				last->next = m;
			}
			delete temp;
		}
		else
		{
			last = m;
			m = m->next;
		}
	}

	m = head;
		
	g_FontBold.SetWidth(m_iFontSize);

	/*int max_heigh = (m_iFontSize + m_iLineSpace + 3) * m_iMaxLine;
	int buttom = 288 * m_flScale;
	int x = 44 * m_flScale;
	int y = ScreenHeight - buttom;
	int min_y = ScreenHeight - max_heigh - buttom;*/

	int max_heigh = (m_iFontSize + m_iLineSpace + 3) * m_iMaxLine;
	int buttom = 30 * m_flScale;
	int x = 318 * m_flScale;
	int y = ScreenHeight - buttom;
	int min_y = ScreenHeight - max_heigh - buttom;

	while (m)
	{
		y -= (m_iLineSpace + 3);
		y -= m->height;
		m = m->next;		
	}
	
	m = head;

	while (m)
	{
		int h = m->height;
		y += h;
		y += (m_iLineSpace + 3);
		m = m->next;

		if (y > min_y - (h + m_iLineSpace + 3) * 2)
			break;
	}

	if (y > min_y)
	{
		m = head;
		y = min_y;
	}

	glEnable(GL_SCISSOR_TEST);
	glScissor(x, buttom + m_iFontSize, m_iMaxWidth + m_iFontSize, max_heigh);

	/*TEXD_SetRenderMode(kRenderTransColor);
	TEXD_SetColor(255, 255, 255, 120);
	TEXD_BindTexture(9999);
	TEXD_DrawTexture(0, 0, 9999, 9999, NULL);
	//TEXD_DrawTexture(x, ScreenHeight - max_heigh - buttom - m_iFontSize, m_iMaxWidth + m_iFontSize, max_heigh, NULL);
	*/

	while (m)
	{
		team_drawing = m->team;
		
		int rx, ry;
		g_FontBold.SetColor(0.2f, 0.2f, 0.2f, 1.0f);
		g_FontBold.DrawString(m->text, x + 2, y + 2, m_iMaxWidth, rx, ry, CheckColor, m_iFontSize + m_iLineSpace);
		g_FontBold.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		g_FontBold.DrawString(m->text, x, y, m_iMaxWidth, rx, ry, CheckColor2, m_iFontSize + m_iLineSpace);

		y += m->height + m_iLineSpace + 3;
		
		m = m->next;
	}

	glDisable(GL_SCISSOR_TEST);
}

void CHudSayText::Add(wchar_t *text, int team)
{
	g_Font.SetWidth(m_iFontSize);

	line *add = new line;
	wcscpy(add->text, text);
	add->die = gEngfuncs.GetAbsoluteTime() + 6.0f;
	add->next = NULL;
	add->team = team;
	add->height = g_Font.GetHeight(text, m_iMaxWidth, m_iLineSpace);

	if (head == NULL)
	{
		head = add;
	}
	else
	{
		line *temp = head;

		while (temp->next != NULL)
			temp = temp->next;

		temp->next = add;
	}	
}

void CHudSayText::MsgFunc_SayText(const char *pszName, int iSize, void *pbuf)
{
	if (!gConfigs.bEnableNewHud)
		return;

	BEGIN_READ(pbuf, iSize);

	int client_index = READ_BYTE();

	if (!client_index)
		return;

	char formatStr[256];
	safe_strcpy(formatStr, READ_STRING(), sizeof(formatStr));

	char sstr[4][256];
	for (int i = 0; i < 4; i++)
	{
		safe_strcpy(sstr[i], READ_STRING(), sizeof(sstr[i]));
		StripEndNewlineFromString(sstr[i]);
	}
	
	if (!sstr[0][0] && client_index > 0)
	{
		gEngfuncs.pfnGetPlayerInfo(client_index, &g_PlayerInfoList[client_index]);
		strcpy(sstr[0], g_PlayerInfoList[client_index].name);
	}
	wchar_t format[256];
	memset(format, 0, sizeof(format));
	wcscpy(format, GetLangUni2(formatStr));

	wchar_t wstr[4][256];

	for (int i = 0; i < 4; i++)
	{
		wchar_t *buf = vgui::localize()->Find(sstr[i]);
		if (buf)
			wcscpy(wstr[i], buf);
		else
			wcscpy(wstr[i], UTF8ToUnicode(sstr[i]));
	}

	wchar_t text[256];
	vgui::localize()->ConstructString(text, sizeof(text), format, 4, wstr[0], wstr[1], wstr[2], wstr[3]);

	Add(text, vPlayer[client_index].team);

	gEngfuncs.Con_Printf("%s", text);
}

void CHudSayText::MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int msg_dest = READ_BYTE();
	int client_index = -1;

	if (msg_dest == HUD_PRINTRADIO)
	{
		char *tmp = READ_STRING();

		if (tmp)
			client_index = atoi(tmp);
	}

	char formatStr[256];
	safe_strcpy(formatStr, LookupString(READ_STRING(), &msg_dest), sizeof(formatStr));
	StripEndNewlineFromString(formatStr);

	wchar_t format[256];
	memset(format, 0, sizeof(format));
	wcscpy(format, GetLangUni2(formatStr));

	char sstr[4][256];
	for (int i = 0; i < 4; i++)
	{
		safe_strcpy(sstr[i], LookupString(READ_STRING()), MSG_BUF_SIZE);
		StripEndNewlineFromString(sstr[i]);
	}

	wchar_t wstr[4][256];

	for (int i = 0; i < 4; i++)
	{
		wchar_t *buf = vgui::localize()->Find(sstr[i]);
		if (buf)
			wcscpy(wstr[i], buf);
		else
			wcscpy(wstr[i], UTF8ToUnicode(sstr[i]));
	}

	wchar_t text[256];

	if (msg_dest == HUD_PRINTRADIO)
	{
		vgui::localize()->ConstructString(text, sizeof(text), format, 4, wstr[0], wstr[1], wstr[2], wstr[3]);
	}
	else
	{
		swprintf(text, format, wstr[0], wstr[1], wstr[2], wstr[3]);
	}

	switch (msg_dest)
	{
	case HUD_PRINTRADIO:
	{
		Add(text, vPlayer[client_index].team);
		break;
	}

	case HUD_PRINTCENTER:
	{
		//StripEndNewlineFromString(msg_text);
		//Hud().m_VGUI2Print.VGUI2HudPrintArgs(msg_text, sstr1, sstr2, sstr3, sstr4, -1, ScreenHeight * 0.35, 1.0, 0.705, 0.118);
		break;
	}

	case HUD_PRINTNOTIFY:
	{
		//psz[0] = 1;
		//safe_sprintf(psz + 1, MSG_BUF_SIZE, msg_text, sstr1, sstr2, sstr3, sstr4);
		//ConsolePrint(ConvertCRtoNL(psz));
		break;
	}

	case HUD_PRINTTALK:
	{
		Add(text, vPlayer[client_index].team);
		gEngfuncs.pfnConsolePrint(UnicodeToUTF8(text));
		gEngfuncs.pfnConsolePrint("\n");
		//gEngfuncs.Con_Printf("%s", text);
		//Hud().m_SayText.SayTextPrint(msg_text, 128, -1, sstr1, sstr2, sstr3, sstr4);
		break;
	}

	case HUD_PRINTCONSOLE:
	{
		//safe_sprintf(psz, MSG_BUF_SIZE, msg_text, sstr1, sstr2, sstr3, sstr4);
		//ConsolePrint(ConvertCRtoNL(psz));
		break;
	}
	}

	//return 1;
}