#include "base.h"
#include "exportfuncs.h"
#include "parsemsg.h"
#include "hud.h"

#include "zse.h"

static CHudZSEUI g_HudZSEUI;
CHudZSEUI &HudZSEUI()
{
	return g_HudZSEUI;
}

void CHudZSEUI::Init()
{
	m_iFlags |= HUD_ACTIVE;
}

void CHudZSEUI::VidInit()
{
	m_Retina[0].VidInit("cstrike//resource//hud//zombie//zombicrazy.tga", false, true);
	m_Retina[1].VidInit("cstrike//resource//hud//zombie//zombitrap.tga", true, true);
}

void CHudZSEUI::Draw(float flTime)
{
	if (g_iMod != MOD_ZSE)
		return;

	if(g_bAlive)
	{
		m_Retina[0].Redraw(flTime);
		m_Retina[1].Redraw(flTime);
	}
	else
	{
		m_Retina[0].SetCanDraw(false);
		m_Retina[1].SetCanDraw(false);
	}
}

void CHudZSEUI::SendData()
{
	int type = READ_BYTE();
	switch (type)
	{
	case 0: // zb skill1
		{
			m_Retina[0].SetCanDraw(true);
			m_Retina[0].SetEndTime(Hud().m_flTime + READ_BYTE());
			break;
		}
	case 1: // zb skill2
		{
			m_Retina[1].SetCanDraw(true);
			m_Retina[1].SetEndTime(Hud().m_flTime + READ_BYTE());
			break;
		}
	case 2: // last solider
		{
			
			break;
		}
	}
}