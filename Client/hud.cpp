
#include "hud.h"
#include "bte_const.h"
#include "exportfuncs.h"
#include "parsemsg.h"
#include "util.h"
#include "common.h"
#include <BaseUI.h>
#include <EButtonManager.h>
#include "CounterStrikeViewport.h"
#include "Viewport.h"

#include "HUD/ammo.h"
#include "HUD/MVPBoard.h"
#include "HUD/deathmsg.h"
#include "HUD/zb3ui.h"

#include "HUD/drawimage.h"
#include "HUD/followicon.h"
#include "HUD/fonttext.h"
#include "HUD/drawtga.h"

//CHud gHUD;
CHud &Hud()
{
	static CHud x;
	return x;
}

CHud &gHUD = Hud();

CHudBase::CHudBase()
{
	Hud().AddHudElem(this);
}

cvar_t *hud_draw;
cvar_t *cl_shadows = NULL;

void Cmd_Buy_Advanced(void);

int MsgFunc_PlayerSpawn(const char *pszName, int iSize, void *pbuf)
{
	if (IS_ZOMBIE_MODE)
	{
		Cmd_Buy_Advanced();
	}
	if (g_iMod == MOD_ZB3)
	{
		HudZombieMod3().ParseData(12, 0);
		HudZombieMod3().ParseData(0, 0);
	}

	HudDeathNotice().OnPlayerSpawn();

	return 1;
}

int MsgFunc_RoundStart(const char *pszName, int iSize, void *pbuf)
{
	HudDeathNotice().OnRoundStart();
	HudAmmo().SetNvgOn(false);
	HudMVPBoard().OnPlayerSpawn();

	return 1;
}

int MsgFunc_EButton(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iEntity = READ_SHORT();
	int bOn = READ_BYTE();
	int iTeamLimit = READ_BYTE();

	TheEButtons.AddAttachment(iEntity, iTeamLimit, bOn);

	return 1;
}

CHud::CHud() : 
	m_iSpriteCount(0),
	m_FollowIcon(HudFollowIconElements()), 
	m_FontText(HudFontTextElements()), 
	m_SPR(HudSPRElements()),
	m_TGA(HudTGAElements())
{

}

void CHud::Init()
{
	CounterStrikeViewport_InstallHook();
	ShutDown();
	
	/*m_NVG.Init();	// Actually it won't place any other element
	m_Retina.Init();
	m_BinkPlayer.Init();
	m_TGA.Init();
	m_FontText.Init();
	m_DeathRunUI.Init();
	m_SPR.Init();
	m_Ammo.Init();
	m_Health.Init();
	m_ScoreBoard.Init();
	m_GunDeathBoard.Init();
	m_Quest.Init();
	m_RespawnBar.Init();
	m_FollowIcon.Init();
	m_ZEUI.Init();
	m_ZBSBoard.Init();
	m_ZB3.Init();
	m_DeathNotice.Init();
	m_Overview.Init();
	m_ZB4.Init();
	m_ZSE.Init();
	m_ZBSelect.Init();
	m_ZBCounter.Init();
	m_SayText.Init();
	m_Menu.Init();
	m_RoundTimer.Init();
	m_Statistics.Init();
	m_Alarm.Init();
	m_MVPBoard.Init();
	m_TABBoard.Init();
	m_DestroyerSniperScope.Init();
	m_BuffAWPSniperScope.Init();
	HudBuffClassDamage().Init();
	HudDeathInfo().Init();*/
	for (CHudBase * const p : m_List)
	{
		p->Init();
	}

	gEngfuncs.pfnHookUserMsg("PlayerSpawn", MsgFunc_PlayerSpawn);
	gEngfuncs.pfnHookUserMsg("RoundStart", MsgFunc_RoundStart);
	gEngfuncs.pfnHookUserMsg("EButton", MsgFunc_EButton);

	//gExportfuncs.HUD_Init();
	g_pViewPort->Init();
}

int CHud::VidInit()
{
	cl_shadows = gEngfuncs.pfnGetCvarPointer("cl_shadows");
	
	//m_scrinfo.iSize = sizeof(SCREENINFO_s);
	//gEngfuncs.pfnGetScreenInfo(&m_scrinfo);
	if (ScreenInfo().iWidth < 640)
		m_iRes = 320;
	else
		m_iRes = 640;

	hud_draw = gEngfuncs.pfnGetCvarPointer("hud_draw");

	if (!m_pSpriteList)
	{
		m_pSpriteList = gEngfuncs.pfnSPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);
		if (m_pSpriteList)
		{
			m_iSpriteCount = 0;
			client_sprite_t *p = m_pSpriteList;
			for (int j = 0; j < m_iSpriteCountAllRes; j++)
			{
				if (p->iRes == m_iRes)
					m_iSpriteCount++;
				p++;
			}
			m_rghSprites = new HSPRITE[m_iSpriteCount];
			m_rgrcRects = new wrect_t[m_iSpriteCount];
			m_rgszSpriteNames = new char[m_iSpriteCount * MAX_SPRITE_NAME_LENGTH];

			p = m_pSpriteList;
			int index = 0;
			for (int j = 0; j < m_iSpriteCountAllRes; j++)
			{
				if (p->iRes == m_iRes)
				{
					char sz[256];
					sprintf(sz, "sprites/%s.spr", p->szSprite);
					m_rghSprites[index] = SPR_Load(sz);
					m_rgrcRects[index] = p->rc;
					strncpy(&m_rgszSpriteNames[index * MAX_SPRITE_NAME_LENGTH], p->szName, MAX_SPRITE_NAME_LENGTH);

					index++;
				}
				p++;
			}
		}
	}
	else
	{
		client_sprite_t *p = m_pSpriteList;
		int index = 0;
		for (int j = 0; j < m_iSpriteCountAllRes; j++)
		{
			if (p->iRes == m_iRes)
			{
				char sz[256];
				sprintf(sz, "sprites/%s.spr", p->szSprite);
				m_rghSprites[index] = SPR_Load(sz);
				index++;
			}
			p++;
		}
	}

	m_HUD_number_0 = GetSpriteIndex("number_0");
	m_iFontHeight = m_rgrcRects[m_HUD_number_0].bottom - m_rgrcRects[m_HUD_number_0].top;
	m_iFontWidth = m_rgrcRects[m_HUD_number_0].right - m_rgrcRects[m_HUD_number_0].left;

	m_flTime = m_flOldTime = m_iIntermission = 0;

	/*m_Overview.VidInit();
	m_Ammo.VidInit();
	m_Health.VidInit();
	m_ZB3.VidInit();
	m_FontText.VidInit();
	m_ScoreBoard.VidInit();
	m_GunDeathBoard.VidInit();
	m_SPR.VidInit();
	m_Quest.VidInit();
	m_FollowIcon.VidInit();
	m_TGA.VidInit();
	m_DeathRunUI.VidInit();
	m_DeathNotice.VidInit();
	m_Retina.VidInit();
	m_ZEUI.VidInit();
	m_Menu.VidInit();
	m_RoundTimer.VidInit();
	m_ZBSelect.VidInit();
	m_TABBoard.VidInit();
	m_Statistics.VidInit();
	m_ZBSBoard.VidInit();
	m_ZB4.VidInit();
	m_ZSE.VidInit();
	m_SayText.VidInit();
	m_NVG.VidInit();
	m_RespawnBar.VidInit();
	m_MVPBoard.VidInit();
	m_SfsniperScope.VidInit();
	m_DestroyerSniperScope.VidInit();
	m_BuffAWPSniperScope.VidInit();
	m_BuffClassDamage.VidInit();*/
	for (CHudBase *p : m_List)
	{
		p->VidInit();
	}

	TheEButtons.Init();
	g_pViewPort->VidInit();
	//gExportfuncs.HUD_VidInit();
	return 1;
}

int CHud::Redraw(float time, int intermission)
{
	//int iResult = gExportfuncs.HUD_Redraw(time, intermission);

	m_scrinfo.iSize = sizeof(SCREENINFO_s);
	gEngfuncs.pfnGetScreenInfo(&m_scrinfo);
	if (m_scrinfo.iWidth < 640)
		m_iRes = 320;
	else
		m_iRes = 640;

	m_flOldTime = m_flTime;
	m_flTime = time;
	m_flTimeDelta = m_flTime - m_flOldTime;

	if (m_flTimeDelta < 0)
		m_flTimeDelta = 0;

	if (m_iIntermission && !intermission)
	{
		m_iIntermission = intermission;

		g_pViewPort->HideAllVGUIMenu();
		//g_pViewPort->UpdateSpectatorPanel();
	}
	else if (!m_iIntermission && intermission)
	{
		m_iIntermission = intermission;

		g_pViewPort->HideAllVGUIMenu();
		//g_pViewPort->ShowScoreBoard();
		//g_pViewPort->UpdateSpectatorPanel();
	}

	m_iIntermission = intermission;

	if (hud_draw->value)
	{
		for (CHudBase *p : m_List)
		{
			if (p->m_iFlags & HUD_ACTIVE)
				p->Draw(time);
		}
	}
	g_pViewPort->SetPaintEnabled(hud_draw->value);
	return 1;//iResult;
}

void CHud::Frame(double flHostFrameTime)
{
	return gExportfuncs.HUD_Frame(flHostFrameTime);
}

int CHud::UpdateClientData(client_data_t *cdata, float time)
{
	memcpy(m_vecOrigin, cdata->origin, sizeof(vec3_t));
	memcpy(m_vecAngles, cdata->viewangles, sizeof(vec3_t));

	m_iWeaponBits = cdata->iWeaponBits;
	m_FOV = cdata->fov;

	Think();

	if (m_FOV > 55 && cl_gunsmoke->value > 0)
		m_bCreateSmoke = TRUE;
	else
		m_bCreateSmoke = FALSE;

	return gExportfuncs.HUD_UpdateClientData(cdata, time);
}

void CHud::Think(void)
{
	for (CHudBase *p : m_List)
	{
		if (p->m_iFlags & HUD_ACTIVE)
			p->Think();
	}
}

void CHud::ShutDown(void)
{
	/*for (CHudBase *p : m_List)
	{
		delete p;
	}*/

	
}

void CHud::AddHudElem(CHudBase *phudelem)
{
	m_List.push_back(phudelem);
}

int CHud::DrawHudString(int xpos, int ypos, int iMaxX, char *szIt, int r, int g, int b)
{
	return xpos += gEngfuncs.pfnDrawString(xpos, ypos, szIt, r, g, b);
}

int CHud::DrawHudNumberString(int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b)
{
	return DrawHudStringReverse(xpos, ypos, iMinX, va("%d", iNumber), r, g, b);
}

int CHud::DrawHudStringReverse(int xpos, int ypos, int iMinX, char *szString, int r, int g, int b)
{
	return xpos -= gEngfuncs.pfnDrawStringReverse(xpos, ypos, szString, r, g, b);
}

int CHud::DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b)
{
	int k;
	int iWidth = m_iFontWidth;

	if (iNumber > 0)
	{
		if (iNumber >= 10000)
		{
			k = iNumber / 10000;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS))
			x += iWidth;

		if (iNumber >= 1000)
		{
			k = (iNumber % 10000) / 1000;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
			x += iWidth;

		if (iNumber >= 100)
		{
			k = (iNumber % 1000) / 100;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
			x += iWidth;

		if (iNumber >= 10)
		{
			k = (iNumber % 100) / 10;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
			x += iWidth;

		k = iNumber % 10;
		gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
		x += iWidth;
	}
	else if (iFlags & DHN_DRAWZERO)
	{
		gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0), r, g, b);

		if (iFlags & (DHN_5DIGITS))
			x += iWidth;

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
			x += iWidth;

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
			x += iWidth;

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
			x += iWidth;

		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));
		x += iWidth;
	}
	return x;
}

int CHud::DrawHudNumber(int x, int y, int iNumber, int r, int g, int b)
{
	static char szBuffer[16];
	const char *pszPosint;
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	int k;

	sprintf(szBuffer, "%5i", iNumber);
	pszPosint = szBuffer;

	if (iNumber < 0)
		pszPosint++;

	while (*pszPosint)
	{
		k = *pszPosint - '0';

		if (k >= 0 && k <= 9)
		{
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			SPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
		}

		x += iWidth;
		pszPosint++;
	}

	return x;
}

int CHud::DrawHudNumber2(int x, int y, bool DrawZero, int iDigits, int iNumber, int r, int g, int b)
{
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	x += (iDigits - 1) * iWidth;

	int ResX = x + iWidth;
	do
	{
		int k = iNumber % 10;
		iNumber /= 10;
		SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
		SPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
		x -= iWidth;
		iDigits--;
	} while (iNumber > 0 || (iDigits > 0 && DrawZero));

	return ResX;
}

int CHud::DrawHudNumber2(int x, int y, int iNumber, int r, int g, int b)
{
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;

	int iDigits = 0;
	int temp = iNumber;
	do
	{
		iDigits++;
		temp /= 10;
	} while (temp > 0);

	x += (iDigits - 1) * iWidth;

	int ResX = x + iWidth;
	do
	{
		int k = iNumber % 10;
		iNumber /= 10;
		SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
		SPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
		x -= iWidth;
	} while (iNumber > 0);

	return ResX;
}

int CHud::GetNumBits(int iNumber)
{
	int k = iNumber;
	int bits = 0;

	while (k)
	{
		k /= 10;
		bits++;
	}

	return bits;
}