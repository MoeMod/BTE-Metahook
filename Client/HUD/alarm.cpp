#include "base.h"
#include "exportfuncs.h"
#include "hud.h"
#include "parsemsg.h"
#include "DrawTargaImage.h"
#include "Fonts.h"
#include "util.h"

#include "alarm.h"
#include "deathmsg.h"

static CHudAlarm g_HudAlarm;
CHudAlarm &HudAlarm()
{
	return g_HudAlarm;
}

//DECLARE_MESSAGE(m_Alarm, StatusAlarm);
//DECLARE_MESSAGE(m_Alarm, Assist);

const char *CHudAlarm::m_szAlarmNames[MAX_ALARM_TYPES] =
{
	"headshot",
	"grenade",
	"knife",
	"kill",
	"2kill",
	"3kill",
	"4kill",
	"excellent",
	"incredible",
	"crazy",
	"cantbelieve",
	"outofworld",
	"firstblood",
	"welcome",
	"comeback",
	"kingmurder",
	"backmarker",
	"payback",
	"savior",
	"thelast",
	"assist",
	"supporter",
	"kingmaker",
	"invisiblehand",
	"c4plant",
	"c4plantblock",
	"c4defuseblock",
	"c4mankill",
	"lastsoldier",
	"infector",
	"liberator",
	"allround",
	"zombietanker",
	"zombiebomb"
};

int CHudAlarm::m_iOrder[MAX_ALARM_TYPES] =
{
	ALARM_EXCELLENT,
	ALARM_INCREDIBLE,
	ALARM_CRAZY,
	ALARM_CANTBELIEVE,
	ALARM_OUTOFWORLD,
	ALARM_FIRSTBLOOD,
	ALARM_2KILL,
	ALARM_3KILL,
	ALARM_4KILL,
	ALARM_HEADSHOT,
	ALARM_GRENADE,
	ALARM_KNIFE,
	ALARM_WELCOME,
	ALARM_COMEBACK,
	ALARM_KINGMURDER,
	ALARM_BACKMARKER,
	ALARM_PAYBACK,
	ALARM_SAVIOR,
	ALARM_ASSIST,
	ALARM_SURPPORTER,
	ALARM_KINGMAKER,
	ALARM_INVISIBLEHAND,
	ALARM_C4PLANT,
	ALARM_C4PLANTBLOCK,
	ALARM_C4DEFUSEBLOCK,
	ALARM_C4MANKILL,
	ALARM_LASTSOLDIER,
	ALARM_INFECTOR,
	ALARM_LIBERATOR,
	ALARM_ALLROUND,
	ALARM_THELAST,
	ALARM_ZOMBIETANKER,
	ALARM_ZOMBIEBOMB,
	ALARM_1KILL
};

const char *CHudAlarm::m_szSound[MAX_ALARM_TYPES] = {
	"vox/headshot.wav",
	"vox/gotit.wav",
	"vox/humililation.wav",
	"metalarena/supply.wav",
	"vox/doublekill.wav",
	"vox/triplekill.wav",
	"vox/multikill.wav",
	"vox/excellent.wav",
	"vox/incredible.wav",
	"vox/crazy.wav",
	"vox/cantbelive.wav",
	"vox/outofworld.wav",
	"vox/firstkill.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"basic/lastsoldier.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav",
	"metalarena/supply.wav"
};

void CHudAlarm::Init(void)
{
	m_iFlags |= HUD_ACTIVE;

	//HOOK_MESSAGE(StatusAlarm);
	//HOOK_MESSAGE(Assist);
	gEngfuncs.pfnHookUserMsg("StatusAlarm", [](const char *pszName, int iSize, void *pbuf){ return HudAlarm().MsgFunc_StatusAlarm(pszName, iSize, pbuf); });
	gEngfuncs.pfnHookUserMsg("Assist", [](const char *pszName, int iSize, void *pbuf){ return HudAlarm().MsgFunc_Assist(pszName, iSize, pbuf); });

	//m_bitsAlarms = m_bitsPrePareAlarms = 0;
	memset(m_bAlarms, 0, sizeof(m_bAlarms));
	memset(m_bPrePareAlarms, 0, sizeof(m_bPrePareAlarms));
	m_iSlotFree = 1;
	m_bDrawBackground = true;

	char szAlarm[256], szRibbon[256];
	char szTextAlarm[256], szTextRibbon[256];

	m_iCurAlarm = -1;

	m_iAlpha = 0;
	m_flEndDraw = 0.0;

	m_iFontBG = Hud().m_TGA.FindTexture("resource\\announceribbon\\announceicon\\alarm_logobg");
	m_iAlarmBG = Hud().m_TGA.FindTexture("resource\\announceribbon\\announceicon\\alarm_bg");

	for (int i = 0; i < MAX_ALARM_TYPES; i++)
	{
		Q_snprintf(szAlarm, 255, "resource\\announceribbon\\announceicon\\alarm_%s", m_szAlarmNames[i]);
		Q_snprintf(szRibbon, 255, "resource\\announceribbon\\ribbonicon\\ribbon_%s", m_szAlarmNames[i]);
		Q_snprintf(szTextAlarm, 255, "#CSO_Alarm_%s", m_szAlarmNames[i]);
		Q_snprintf(szTextRibbon, 255, "#CSO_Ribbon_%s", m_szAlarmNames[i]);

		m_iAlarms[i] = Hud().m_TGA.FindTexture(szAlarm);
		m_iRibbons[i] = Hud().m_TGA.FindTexture(szRibbon);
		m_iHappenedTimes[i] = 0;
		wcscpy(m_lpAlarms[i], GetLangUni(szTextAlarm));
		wcscpy(m_lpRibbons[i], GetLangUni(szTextRibbon));

		if (!wcscmp(m_lpAlarms[i], L"#CSO_Alarm_supporter"))
		{
			wcscpy(m_lpAlarms[i], GetLangUni("#CSO_Alarm_Surpporter"));
			wcscpy(m_lpRibbons[i], GetLangUni("#CSO_Ribbon_Surpporter"));
		}
	}
}

void CHudAlarm::Draw(float time)
{
	if (m_flEndDraw && cl.time >= m_flEndDraw)
	{
		if (GetNext())
		{
			m_flEndDraw = cl.time + 1.5;
			m_iAlpha = 255;
		}
		else
			m_flEndDraw = 0.0;
		return;	// This time we won't draw
	}
	if (m_iCurAlarm < 0 || m_iCurAlarm > MAX_ALARM_TYPES-1) return;

	static int iX, iY, iW, iH, iBGX, iBGY;

	g_Font.SetColor(230, 230, 230, m_iAlpha - 30);
	g_Font.SetWidth(19);

	iW = g_Font.GetLen(m_lpAlarms[m_iCurAlarm]);
	iH = -10;

	iX = ScreenWidth / 2 - g_Texture[m_iAlarms[m_iCurAlarm]].iWidth / 2;
	iY = int(ScreenHeight * (123.0 / 768.0));//g_Texture[m_iAlarms[m_iCurAlarm]].iHeight + 15 + 6;

	iBGX = ScreenWidth / 2 - g_Texture[m_iAlarmBG].iWidth / 2;
	iBGY = iY - g_Texture[m_iAlarms[m_iCurAlarm]].iHeight + 5;
	if (m_iAlpha > 120 && m_bDrawBackground)
		GL_DrawTGA(g_Texture[m_iAlarmBG].iTexture, 255, 255, 255, m_iAlpha, iBGX, iBGY, 1.0);
	GL_DrawTGA(g_Texture[m_iAlarms[m_iCurAlarm]].iTexture, 255, 255, 255, m_iAlpha, iX, iY, 1.0);

	iX = ScreenWidth / 2 - g_Texture[m_iFontBG].iWidth / 2 + 5;
	iY += 87;

	if (m_lpAlarms[m_iCurAlarm][0] != L'#')
	{
		GL_DrawTGA(g_Texture[m_iFontBG].iTexture, 255, 255, 255, m_iAlpha - 30, iX, iY, 1.0);

		iX = ScreenWidth / 2 - iW / 2;
		iY += (g_Texture[m_iFontBG].iHeight - iH) / 2;
		//iY = int(ScreenHeight * (234.0 / 768.0));

		g_Font.DrawString(m_lpAlarms[m_iCurAlarm], iX, iY, 1000, 1000);
	}

	if (m_flEndDraw - cl.time <= 1.0)
	{
		m_iAlpha -= 10;
	}

	if (m_iAlpha <= 30)
	{
		m_iCurAlarm = -1;
		m_iAlpha = 255;
	}
}

void CHudAlarm::AddAlarm(AlarmType iAlarm, bool bNeedCheck)
{
	if (m_bAlarms[iAlarm])
		return;
	ClearSmallerIcon(iAlarm);
	m_iHappenedTimes[iAlarm] ++;
	if (bNeedCheck && !CheckFree())
	{
		m_bAlarms[iAlarm] = true;
		m_iSlotFree = 0;
		m_bDrawBackground = true;
		return;
	}
	m_iSlotFree = 0;
	m_iCurAlarm = iAlarm;
	PlayAlarmSound();
	m_iAlpha = 255;
	m_flEndDraw = cl.time + 1.5;
	m_bDrawBackground = true;
	return;
}

void CHudAlarm::PlayAlarmSound(void)
{
	if (0 > m_iCurAlarm || m_iCurAlarm > MAX_ALARM_TYPES-1)
		return;
	static char pszCommand[256];
	sprintf_s(pszCommand, "speak %s", m_szSound[m_iCurAlarm]);
	gEngfuncs.pfnClientCmd(pszCommand);
	return;
}

int CHudAlarm::MsgFunc_StatusAlarm(const char * pszName, int iSize, void * pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iAlarm = READ_SHORT();
	if (iAlarm == ALARM_LASTSOLDIER || iAlarm == ALARM_C4PLANT || iAlarm == ALARM_ZOMBIETANKER)
		AddAlarm(GetPrivate(iAlarm));
	else
		m_bPrePareAlarms[iAlarm]=true;

	return 1;
}

int CHudAlarm::MsgFunc_Assist(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	
	int iKiller = READ_SHORT();
	int iVictim = READ_SHORT();

	if (iKiller != iVictim)
	{
		bool bCheckAdded = false;
		if (IS_ZOMBIE_MODE && vPlayer[gEngfuncs.GetLocalPlayer()->index].team == 2)
		{
			bCheckAdded = true;
			AddAlarm(ALARM_ZOMBIEBOMB, false);
		}
		int iFirst[2] = { RankFirst(vPlayer[iKiller].team), RankLast(vPlayer[iVictim].team) };
		if (vPlayer[iKiller].team == vPlayer[iVictim].team && iFirst[0] == iKiller && g_PlayerExtraInfo[iFirst[0]].frags > 0)
		{

			bCheckAdded = true;
			AddAlarm(ALARM_KINGMAKER, false);
		}
		if (iFirst[1] == iVictim && g_PlayerExtraInfo[iFirst[1]].frags > 0)
		{
			bCheckAdded = true;
			AddAlarm(ALARM_INVISIBLEHAND, false);
		}
		if (GetAssistTimes() == 5)
		{
			bCheckAdded = true;
			if (!GetHappenedTimes(ALARM_SURPPORTER))
				AddAlarm(ALARM_SURPPORTER, false);
		}
		if (!bCheckAdded)
		{
			AddAlarm(ALARM_ASSIST, false);
		}
	}

	return 1;
}