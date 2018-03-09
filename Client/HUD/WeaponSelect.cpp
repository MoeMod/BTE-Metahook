#include "base.h"
#include "hud.h"
#include "BaseUI.h"
#include "common.h"

#include "Client/HUD/ZombieSelect.h"
#include "Client/HUD/WeaponSelect.h"

static CHudWeaponSelect g_HudWeaponSelect;
CHudWeaponSelect &HudWeaponSelect()
{
	return g_HudWeaponSelect;
}

void CHudWeaponSelect::Init(void)
{
	m_iFlags |= HUD_ACTIVE;

	for (int i = 0; i < 5; i++)
		m_iIcon[i] = vgui::surface()->CreateNewTextureID();

	memset(m_rcIcon, NULL, sizeof(wrect_t));
	memset(m_szTimeRemaining, 0, sizeof(m_szTimeRemaining));

	m_Bink.Init("cstrike//resource//zombie//zbselectbg.bik");
}

void CHudWeaponSelect::VidInit(void)
{
	for (int i = 0; i < 4; i++)
		m_bDraw[i] = false;

	m_Bink.VidInit();

	SetIcon(4, "cancel");
	m_bDraw[4] = true;

	m_flTimeEnd = 0.0f;
	m_bCanDraw = false;
}

void CHudWeaponSelect::SetIcon(int slot, char *name)
{
	char szFilename[128];
	sprintf(szFilename, "resource\\hud\\zombie\\zmrewalk_%s", name);

	vgui::surface()->DrawSetTextureFile(m_iIcon[slot], szFilename, true, false);
	vgui::surface()->DrawGetTextureSize(m_iIcon[slot], m_rcIcon[slot].right, m_rcIcon[slot].bottom);

	m_bDraw[slot] = true;
}

void CHudWeaponSelect::Draw(float flTime)
{
	if (!m_bCanDraw)
		return;

	if (flTime > m_flTimeEnd)
	{
		m_bCanDraw = false;

		return;
	}
}

void CHudWeaponSelect::SetDrawTime(float flTime)
{
	if (HudZBSelect().m_bCanDraw)
		HudZBSelect().m_bCanDraw = false;
	
	m_flTimeEnd = flTime + 5.0;
	m_Bink.Play(true, flTime + 5.0);
	m_iTimeRemaining = 5;

	m_bCanDraw = true;
}

void CHudWeaponSelect::Select(int item)
{
	m_bCanDraw = false;
	m_Bink.Stop();

	if (!item)
		return;

	gEngfuncs.pfnClientCmd(va("bte_zb3_select_weapon %d\n", item - 1));
}