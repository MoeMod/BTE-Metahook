#include "base.h"
#include "exportfuncs.h"
#include "hud.h"
#include "BaseUI.h"
#include "ZombieCountDown.h"
#include "msghook.h"

static CHudZBCounter g_HudZBCounter;
CHudZBCounter &HudZBCounter()
{
	return g_HudZBCounter;
}

void CHudZBCounter::Init()
{
	// BUG FIX : DO NOT Add Hud Element for this class
	gEngfuncs.pfnAddCommand("zb_roundstart", [](){HudZBCounter().SetStartTime(Hud().m_flTime); });
}

void CHudZBCounter::VidInit()
{
	m_iStartTime = -1;
	m_iCurTime = -1;
}

void CHudZBCounter::Draw(float flTime)
{
	if (!CanDraw())
		return;

	int time = flTime - m_iStartTime;

	if (time != m_iCurTime)
	{
		m_iCurTime = time;
				
		if(time < 20)
		{
			char count[4];
			sprintf(count, "%d", 20 - time);

			MSG_BeginWrite("TextMsg");
			MSG_WriteByte(HUD_PRINTCENTER);
			MSG_WriteString("#CSBTE_ZombiSelectCount");
			MSG_WriteString(count);
			MSG_EndWrite();

			if(time >= 10)
			{
				switch (20 - time - 1)
				{
				case 0: gEngfuncs.pfnClientCmd("spk sound/vox/one.wav");break;
				case 1: gEngfuncs.pfnClientCmd("spk sound/vox/two.wav");break;
				case 2: gEngfuncs.pfnClientCmd("spk sound/vox/three.wav");break;
				case 3: gEngfuncs.pfnClientCmd("spk sound/vox/four.wav");break;
				case 4: gEngfuncs.pfnClientCmd("spk sound/vox/five.wav");break;
				case 5: gEngfuncs.pfnClientCmd("spk sound/vox/six.wav");break;
				case 6: gEngfuncs.pfnClientCmd("spk sound/vox/seven.wav");break;
				case 7: gEngfuncs.pfnClientCmd("spk sound/vox/eight.wav");break;
				case 8: gEngfuncs.pfnClientCmd("spk sound/vox/nine.wav");break;
				case 9: gEngfuncs.pfnClientCmd("spk sound/vox/ten.wav");break;
				}
			}
		}

		if (time == 0)
		{
			gEngfuncs.pfnClientCmd("mp3 stop");
			gEngfuncs.pfnClientCmd("spk sound/vox/zombi_start.wav");
		}

		if (time == 21)
		{
			gEngfuncs.pfnClientCmd("mp3 stop");
			gEngfuncs.pfnClientCmd("mp3 play sound/music/zb_start.mp3");

			if (gEngfuncs.pfnRandomLong(0, 1))
				gEngfuncs.pfnClientCmd("spk sound/vox/zombi_coming_1.wav");
			else
				gEngfuncs.pfnClientCmd("spk sound/vox/zombi_coming_2.wav");
		}
	}
}

void CHudZBCounter::SetStartTime(float flTime)
{
	m_iStartTime = flTime;
}

bool CHudZBCounter::CanDraw()
{
	if (g_iMod != MOD_ZSE)
		return false;

	if (m_iStartTime == -1)
		return false;

	return true;
}