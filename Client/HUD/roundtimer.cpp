#include "metahook.h"
#include "bte_const.h"
#include "exportfuncs.h"
#include "hud.h"
#include "display.h"
#include "configs.h"
#include "util.h"

#include "roundtimer.h"

static CHudRoundTimer g_HudRoundTimer;
CHudRoundTimer &HudRoundTimer()
{
	return g_HudRoundTimer;
}

void CHudRoundTimer::Think(float flTime)
{
	int timer = m_flTimeEnd - flTime + 1;

	if (timer < 0)
		timer = 0;

	if (timer <= 0)
	{
		m_bPanicColor = true;
	}
	else if (timer <= 2)
	{
		if (flTime > m_flNextToggle)
		{
			m_flNextToggle = flTime + 0.05;
			m_bPanicColor = !m_bPanicColor;
		}
	}
	else if (timer <= 5)
	{
		if (flTime > m_flNextToggle)
		{
			m_flNextToggle = flTime + 0.1;
			m_bPanicColor = !m_bPanicColor;
		}
	}
	else if (timer <= 10)
	{
		if (flTime > m_flNextToggle)
		{
			m_flNextToggle = flTime + 0.2;
			m_bPanicColor = !m_bPanicColor;
		}
	}
	else if (timer <= 20)
	{
		if (flTime > m_flNextToggle)
		{
			m_flNextToggle = flTime + 0.4;
			m_bPanicColor = !m_bPanicColor;
		}
	}
	else if (timer <= 30)
	{
		if (flTime > m_flNextToggle)
		{
			m_flNextToggle = flTime + 0.8;
			m_bPanicColor = !m_bPanicColor;
		}
	}

	if (!m_bPanicColor)
	{
		UnpackRGB(m_R, m_G, m_B, RGB_YELLOWISH);
		ScaleColors(m_R, m_G, m_B, 100);
	}
	else
	{
		UnpackRGB(m_R, m_G, m_B, RGB_REDISH);
	}

	m_timer = timer;
}

void CHudRoundTimer::Draw(float flTime)
{
	Think(flTime);

	if (gConfigs.bEnableNewHud)
		return;

	if (!g_bAlive)
		return;

	if (g_iMod != MOD_ZB4)
		return;

	if (Hud().m_iHideHUDDisplay & (1 << 4))
		return;

	int iX = ScreenWidth / 2;
	int iY = /*!g_bAlive ? 200 : */ScreenHeight - 37;



	int timer_m, timer_s;
	int timer = m_timer;

	timer_m = timer / 60;
	timer_s = timer % 60;

	Hud().DrawHudNumber(iX - 44, iY, DHN_2DIGITS, timer_m, m_R, m_G, m_B);
	Hud().DrawHudNumber(iX + 6, iY, DHN_2DIGITS, timer_s, m_R, m_G, m_B);

	gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iIcon), m_R, m_G, m_B);
	gEngfuncs.pfnSPR_DrawAdditive(0, iX - 64, iY, &m_rcIcon);

	gEngfuncs.pfnDrawCharacter(iX, iY, ':', m_R, m_G, m_B);

	if (timer_m == 0)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(Hud().m_HUD_number_0), m_R, m_G, m_B);
		gEngfuncs.pfnSPR_DrawAdditive(0, iX - 24, iY, &Hud().GetSpriteRect(Hud().m_HUD_number_0));
	}

	if (timer_s < 10)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(Hud().m_HUD_number_0), m_R, m_G, m_B);
		gEngfuncs.pfnSPR_DrawAdditive(0, iX + 6, iY, &Hud().GetSpriteRect(Hud().m_HUD_number_0));
	}
	if (timer_s == 0)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(Hud().m_HUD_number_0), m_R, m_G, m_B);
		gEngfuncs.pfnSPR_DrawAdditive(0, iX + 26, iY, &Hud().GetSpriteRect(Hud().m_HUD_number_0));
	}

}

void CHudRoundTimer::VidInit(void)
{
	m_iIcon = Hud().GetSpriteIndex("stopwatch");
	m_rcIcon = Hud().GetSpriteRect(m_iIcon);

	m_iAlpha = 100;
	UnpackRGB(m_R, m_G, m_B, RGB_YELLOWISH);
	ScaleColors(m_R, m_G, m_B, m_iAlpha);

	m_bPanicColor = false;
	m_flTimeEnd = 0.0f;
}

void CHudRoundTimer::SetTimeEnd(float flTime)
{
	m_flTimeEnd = flTime;

	m_bPanicColor = false;
}

void CHudRoundTimer::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}