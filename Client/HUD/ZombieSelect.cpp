#include "base.h"
#include "exportfuncs.h"
#include "hud.h"
#include "BaseUI.h"
#include "Fonts.h"
#include "util.h"

#include "Client/HUD/ZombieSelect.h"
#include "Client/HUD/WeaponSelect.h"

static CHudZBSelect g_HudZBSelect;
CHudZBSelect &HudZBSelect()
{
	return g_HudZBSelect;
}

/*
static const char *g_szZombieSlot[19] = 
{
	"tank_zombi",
	"speed_zombi",
	"heavy_zombi",
	"heal_zombi",
	"pc_zombi",
	"deimos_zombi",
	"deimos2_zombi",
	"witch_zombi",
	"undertaker",
	"china_zombi",
	"boomer_zombi",
	"resident_zombi",
	"z4normal",
	"z4light",
	"z4heavy",
	"z4hide",
	"z4humpback",
	"random",
	"cancel"
};
*/
void CHudZBSelect::Init()
{
	m_iFlags |= HUD_ACTIVE;
	
	for(int i=0;i<20;i++)
		m_iIcon[i] = vgui::surface()->CreateNewTextureID();
		
	memset(m_rcIcon, 0, sizeof(m_rcIcon));
	memset(m_szTimeRemaining, 0, sizeof(m_szTimeRemaining));

	m_Bink.Init("cstrike//resource//hud//zombie//zbselectbg.bik");
}

void CHudZBSelect::VidInit()
{
	m_Bink.VidInit();
	
	m_iSlotCount = 0;

	m_flTimeEnd = 0.0f;
	m_bCanDraw = false;
}

void CHudZBSelect::SetCount(int count)
{
	count += 1;

	int width = m_Bink.GetOriginalWidth();
	int height = m_Bink.GetOriginalHeight() + 59 * ((count) % 2 - 2);

	m_iSlotCount = 0;

	m_Bink.SetSize(width, height);

	SetIcon(19, "cancel");
}

void CHudZBSelect::SetIcon(int slot, const char *name)
{
	char szFilename[128];
	sprintf(szFilename,"resource\\hud\\zombie\\zombietype_%s", name);

	vgui::surface()->DrawSetTextureFile(m_iIcon[slot], szFilename, true, false);
	vgui::surface()->DrawGetTextureSize(m_iIcon[slot], m_rcIcon[slot].right, m_rcIcon[slot].bottom);
}

void CHudZBSelect::AddZombie(const char *name)
{
	if (m_iSlotCount >= 19)
		return;

	SetIcon(m_iSlotCount, name);

	m_iSlotCount++;

}

void CHudZBSelect::Draw(float flTime)
{
	if (m_bCanDraw == false)
		return;
	
	if (flTime > m_flTimeEnd)
	{
		m_bCanDraw = false;

		return;	
	}

	int iX = 0, iY = 210;//ScreenHeight - 40 - 417;

	m_Bink.SetPos(0, iY);
	m_Bink.Paint(flTime);

	iX += 60;

	g_Font.SetWidth(16);
	g_Font.SetColor(255, 255, 255, 200);
	g_Font.DrawString(m_szTimeRemaining, iX, iY + 45, 100);

	iY += 80 + 59;

	for (int i = 0; i<=m_iSlotCount; i++)
	{
		iX = 24 + 69 * (i % 2);
		iY += 59 * ((i - 1) % 2);

		if (i >= m_iSlotCount)
			i = 19;

		vgui::surface()->DrawSetColor(255, 255, 255, 255);
		vgui::surface()->DrawSetTexture(m_iIcon[i]);
		vgui::surface()->DrawTexturedRect(iX, iY, iX + m_rcIcon[i].right, iY + m_rcIcon[i].bottom);

		char p[2];
		if (i < 9)
			p[0] = '0' + i + 1;
		else
		{
			switch (i)
			{
			case 9:
				p[0] = '!';
				break;
			case 10:
				p[0] = '@';
				break;
			case 11:
				p[0] = '#';
				break;
			case 12:
				p[0] = '$';
				break;
			case 13:
				p[0] = '%';
				break;
			case 14:
				p[0] = '^';
				break;
			case 15:
				p[0] = '&';
				break;
			case 16:
				p[0] = '*';
				break;
			case 17:
				p[0] = '(';
				break;
			case 18:
				p[0] = ')';
				break;
			case 19:
				p[0] = '0';
				break;
			}
		}
		p[1] = '\0';

		g_Font.SetWidth(15);
		g_Font.SetColor(255, 255, 255, 200);
		g_Font.DrawString(UTF8ToUnicode(p), iX + 4,iY + 15, 100);
	}

	if(m_iTimeRemaining != int(m_flTimeEnd - flTime))
	{
		m_iTimeRemaining = int(m_flTimeEnd - flTime);

		wchar_t szBuf[2][256];		

		wchar_t *pBuf;
		pBuf = vgui::localize()->Find("CSBTE_ZombieSelect");

		memset(szBuf, 0, sizeof(szBuf));
		memset(m_szTimeRemaining, 0, sizeof(m_szTimeRemaining));

		wcsncpy(szBuf[0], pBuf, sizeof(szBuf[0]));
		
		szBuf[1][0] = L'0' + m_iTimeRemaining + 1;
		szBuf[1][1] = 0;

		vgui::localize()->ConstructString(m_szTimeRemaining, 64, szBuf[0], 1, szBuf[1]);			
	}
	
}

void CHudZBSelect::SetDrawTime(float flTime)
{
	if (HudWeaponSelect().m_bCanDraw)
		HudWeaponSelect().m_bCanDraw = false;
	
	m_flTimeEnd = flTime + 5.0;
	m_Bink.Play(true, flTime + 5.0);
	m_iTimeRemaining = 5;

	m_bCanDraw = true;
}

void CHudZBSelect::Select(int item)
{
	if (!item)
		m_bCanDraw = false;

	if (item)
	{
		char ch[128];

		if(g_iMod == MOD_ZB4)
			sprintf(ch, "bte_zb4_select_zombie %d\n", item - 1);
		else
			sprintf(ch, "bte_zb3_select_zombie %d\n", item - 1);

		gEngfuncs.pfnClientCmd(ch);

		m_bCanDraw = false;	
		m_Bink.Stop();
	}
}