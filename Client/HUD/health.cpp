#include "metahook.h"
#include "bte_const.h"
#include "exportfuncs.h"
#include "hud.h"
#include "display.h"
#include "parsemsg.h"
#include "configs.h"
#include "util.h"

#include "weapons.h"

#include "health.h"

static CHudHealth g_HudHealth;
CHudHealth &HudHealth()
{
	return g_HudHealth;
}

//DECLARE_MESSAGE(m_Health, HealthExtra);

void CHudHealth::Init(void)
{
	m_iFlags |= HUD_ACTIVE;

	//HOOK_MESSAGE(HealthExtra);
	gEngfuncs.pfnHookUserMsg("HealthExtra", [](const char *pszName, int iSize, void *pbuf){ return HudHealth().MsgFunc_HealthExtra(pszName, iSize, pbuf); });
}

void CHudHealth::VidInit(void)
{
	m_iHealth = 100;
	m_flHealthFade = 0;
	m_flArmorFade = 0;
	m_iArmorFlags = DHN_3DIGITS | DHN_DRAWZERO;

	m_iHealthIcon = Hud().GetSpriteIndex("cross");
	m_iHealthExtraIcon = Hud().GetSpriteIndex("crosstime");
	m_iArmorIcon[0] = Hud().GetSpriteIndex("suit_empty");
	m_iArmorFullIcon[0] = Hud().GetSpriteIndex("suit_full");
	m_iArmorIcon[1] = Hud().GetSpriteIndex("suithelmet_empty");
	m_iArmorFullIcon[1] = Hud().GetSpriteIndex("suithelmet_full");
}

void CHudHealth::Draw(float time)
{
	if (g_iMod == MOD_ZE || g_iMod == MOD_DR || !g_bAlive)
		return;
	DrawHealth(time);
	DrawHealthExtra(time);
	DrawArmor(time);
}

void CHudHealth::Think(void)
{
	if (gConfigs.bEnableNewHud)
	{
		if (m_iFlags & HUD_ACTIVE)
			m_iFlags &= ~HUD_ACTIVE;
	}
	else
	{
		if (!(m_iFlags & HUD_ACTIVE))
			m_iFlags |= HUD_ACTIVE;
	}
}

void CHudHealth::DrawHealth(float time)
{
	int r, g, b;
	int a = 0, x, y;

	if ((Hud().m_iHideHUDDisplay & HIDEWEAPON_HEALTH) || gEngfuncs.IsSpectateOnly())
		return;

	if (m_flHealthFade)
	{
		m_flHealthFade -= (Hud().m_flTimeDelta * 20);

		if (m_flHealthFade <= 0)
		{
			a = MIN_ALPHA;
			m_flHealthFade = 0;
		}
		a = MIN_ALPHA + (m_flHealthFade / FADE_TIME) * 128;
	}
	else
		a = MIN_ALPHA;

	if (m_iHealth <= 15)
		a = 255;

	if (m_iHealth <= 25)
	{
		r = 250;
		g = 0;
		b = 0;
	}
	else
		UnpackRGB(r, g, b, RGB_YELLOWISH);

	ScaleColors(r, g, b, a);

	if (Hud().m_iWeaponBits & (1 << (WEAPON_VEST)))
	{
		int iCrossWidth = Hud().GetSpriteRect(m_iHealthIcon).right - Hud().GetSpriteRect(m_iHealthIcon).left;

		x = iCrossWidth / 2;
		y = ScreenHeight - Hud().m_iFontHeight - Hud().m_iFontHeight / 2;

		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iHealthIcon), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iHealthIcon));

		x = iCrossWidth + Hud().m_iFontWidth / 2;
		Hud().DrawHudNumber(x, y, m_iHealthFlags, m_iHealth, r, g, b);
	}
}

void CHudHealth::DrawHealthExtra(float time)
{
	int r, g, b;
	int a = 0, x, y;

	if ((Hud().m_iHideHUDDisplay & HIDEWEAPON_HEALTH) || gEngfuncs.IsSpectateOnly())
		return;
	if (m_iHealthExtra <= 0)
		return;

	a = 255;

	UnpackRGB(r, g, b, RGB_LIGHTBLUE);

	ScaleColors(r, g, b, a);

	if (Hud().m_iWeaponBits & (1 << (WEAPON_VEST)))
	{
		int iCrossWidth = Hud().GetSpriteRect(m_iHealthExtraIcon).right - Hud().GetSpriteRect(m_iHealthExtraIcon).left;

		x = iCrossWidth / 2;
		y = ScreenHeight - Hud().m_iFontHeight * 3 + 7;

		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iHealthExtraIcon), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iHealthExtraIcon));

		x = iCrossWidth + Hud().m_iFontWidth / 2;
		Hud().DrawHudNumber(x, y, m_iHealthFlags, m_iHealthExtra, r, g, b);
	}
}

void CHudHealth::DrawArmor(float time)
{
	if ((Hud().m_iHideHUDDisplay & (HIDEWEAPON_HEALTH | HIDEWEAPON_ALL)) || gEngfuncs.IsSpectateOnly())
		return;

	if (!(Hud().m_iWeaponBits & (1 << (WEAPON_VEST))))
		return;

	if (m_iArmorType >= sizeof m_iArmorIcon)
		return;

	int x, y;
	int r, g, b, a;

	if (m_flArmorFade)
	{
		m_flArmorFade -= (Hud().m_flTimeDelta * 20);

		if (m_flArmorFade <= 0)
		{
			a = MIN_ALPHA;
			m_flArmorFade = 0;
		}
		a = MIN_ALPHA + (m_flArmorFade / FADE_TIME) * 128;
	}
	else
		a = MIN_ALPHA;
	UnpackRGB(r, g, b, RGB_YELLOWISH);
	ScaleColors(r, g, b, a);
	int iCrossWidth = Hud().GetSpriteRect(m_iHealthIcon).right - Hud().GetSpriteRect(m_iHealthIcon).left;
	x = ScreenWidth / 5;
	y = ScreenHeight - Hud().m_iFontHeight - Hud().m_iFontHeight / 2;
	gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iArmorIcon[m_iArmorType]), r, g, b);
	gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iArmorIcon[m_iArmorType]));
	wrect_t rcFullArmo = *&Hud().GetSpriteRect(m_iArmorFullIcon[m_iArmorType]);
	rcFullArmo.top += (rcFullArmo.bottom - rcFullArmo.top) * ((float)(100 - (min(100, m_iArmor))) * 0.01);
	if (rcFullArmo.bottom > rcFullArmo.top)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iArmorFullIcon[m_iArmorType]), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y + rcFullArmo.top - Hud().GetSpriteRect(m_iArmorFullIcon[m_iArmorType]).top, &rcFullArmo);
	}
	x += iCrossWidth;
	Hud().DrawHudNumber(x, y, m_iArmorFlags, m_iArmor, r, g, b);
}

int CHudHealth::MsgFunc_HealthExtra(const char * pszName, int iSize, void * pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_iHealthExtra = READ_SHORT();

	return 1;
}