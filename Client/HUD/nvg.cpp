#include "base.h"
#include "exportfuncs.h"
#include "parsemsg.h"
#include "hud.h"
#include "util.h"
#include <eventscripts.h>

#include "ammo.h"
#include "nvg.h"
#include "zb3ui.h"

#include <functional>

static CHudNVG g_HudNVG;
CHudNVG &HudNVG()
{
	return g_HudNVG;
}

BOOL CHudNVG::GetStatus()
{
	return m_bOn;
}

void CreateLight(vec3_t origin, float radius, int r, int g, int b, float die)
{
	dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
	dl->origin = origin;
	dl->radius = radius;
	dl->color.r = r;
	dl->color.g = g;
	dl->color.b = b;
	dl->die = cl.time + die;
}

void CHudNVG::Init(void)
{
	Reset();
	
	m_iFlags |= HUD_ACTIVE;
	
	gEngfuncs.pfnAddCommand("+nvgadjust", [](){HudNVG().UserCmd_NVGAdjustDown(); });
	gEngfuncs.pfnAddCommand("-nvgadjust", [](){HudNVG().UserCmd_NVGAdjustUp(); });
}

void CHudNVG::VidInit(void)
{
	Reset();
	m_bOn = FALSE;
	HudAmmo().SetNvgOn(FALSE);
}

void CHudNVG::Reset(void)
{
	//m_bOn = FALSE;
	m_flAlpha = 110;
}

int CHudNVG::MsgFunc_NVGToggle(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_bOn = READ_BYTE();
	HudAmmo().SetNvgOn(m_bOn);
	if (IS_ZOMBIE_MODE)
		HudZombieMod3().m_iNVG = m_bOn;

	return 1;
}

void CHudNVG::UserCmd_NVGAdjustDown()
{
	if (IS_ZOMBIE_MODE)
		return;
	m_flAlpha += 10;
	if (m_flAlpha > 220)
		m_flAlpha = 220;
}

void CHudNVG::UserCmd_NVGAdjustUp()
{
	if (IS_ZOMBIE_MODE)
		return;
	m_flAlpha -= 10;
	if (m_flAlpha < 30)
		m_flAlpha = 30;
}

void CHudNVG::Draw(float time)
{
	if (!m_bOn || gEngfuncs.IsSpectateOnly())
		return;
	int idx;
	if (IS_FIRSTPERSON_SPEC)
		idx = g_iUser2;
	else
		idx = gEngfuncs.GetLocalPlayer()->index;
	if (idx < 1 || idx > gEngfuncs.GetMaxClients())
		return;
	if (vPlayer[idx].team == 2 && IS_ZOMBIE_MODE)
		return;

	if (IS_ZOMBIE_MODE)
	{
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight, 50, 255, 60, 20);
		gEngfuncs.pfnFillRGBA(0, 0, ScreenWidth, ScreenHeight, 0, 90, 0, rand() % 10 + 50);
	}
	else
	{
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight, 50, 225, 50, m_flAlpha);
		CreateLight(Hud().m_vecOrigin, 750 + gEngfuncs.pfnRandomLong(0, 50), 1, 20, 1, 0.1);
	}
}

void CHudNVG::DrawBackground()
{
	if (!m_bOn || gEngfuncs.IsSpectateOnly() || !IS_ZOMBIE_MODE)
		return;
	gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight, 90, 0, 0, 120);
}