#include "base.h"
#include "exportfuncs.h"
#include "message.h"
#include "hud.h"
#include "configs.h"
#include "BaseUI.h"
#include "texdraw.h"
#include "TriAPI.h"
#include "HUD3D.h"
#include "weapons.h"
#include "TextureManager.h"

#include "Client/WeaponManager.h"
#include "Client/HUD/overview.h"
#include "Client/HUD/DrawTabPanel.h"
#include "Client/HUD/scoreboard.h"
#include "Client/HUD/roundtimer.h"
#include "Client/HUD/health.h"
#include "Client/HUD/ammo.h"


CHud3D gHud3D;

cvar_t *hud3d_color;

void CHud3D::LoadTexture(char *name, int &id, wrect_t &rc)
{
	auto &Texture = TextureManager()[name];
	id = Texture;

	rc.right = Texture.w();
	rc.bottom = Texture.h();
}

void CHud3D::Init()
{
	//if (!gConfigs.bEnableNewHud)
		//return;

	hud3d_color = gEngfuncs.pfnRegisterVariable("hud3d_color", "0", FCVAR_ARCHIVE);
	
}

void CHud3D::VidInit()
{
	m_fReloadTimer = 0;
	m_fMoneyTimer = 0;
	m_iHPFade = 0;
	m_flHPTimer = 0;
	m_iHPAP[0] = m_iHPAP[1] = 100;
	m_iMaxHPAP[0] = m_iMaxHPAP[1] = 100;
	//CalcPos(1, m_rcBg[1].right, m_rcBg[1].bottom);

	LoadTexture("resource\\hud3d_v2\\Left-UI.tga", m_iBg[0], m_rcBg[0]);
	LoadTexture("resource\\hud3d_v2\\Right-UI-3.tga", m_iBg[1], m_rcBg[1]);
	LoadTexture("resource\\hud3d_v2\\Right-UI-5.tga", m_iBg[2], m_rcBg[1]);

	for (int i = '0'; i <= '9'; i++)
	{
		char name[64];
		char a[2]; a[0] = i; a[1] = 0;
		sprintf(name, "%s%s%s", "resource\\hud3d\\", a, ".tga");
		LoadTexture(name, m_iNumber[i - '0'], m_rcNumber[i - '0']);
	}

	LoadTexture("resource\\hud3d\\line.tga", m_iLine, m_rcLine);
	LoadTexture("resource\\hud3d\\add.tga", m_iDollar[1], m_rcDollar);
	LoadTexture("resource\\hud3d\\substract.tga", m_iDollar[2], m_rcDollar);
	LoadTexture("resource\\hud3d\\dollar.tga", m_iDollar[0], m_rcDollar);
	LoadTexture("resource\\hud3d\\reloading.tga", m_iReloading, m_rcReloading);

	LoadTexture("resource\\hud3d_v2\\HPAP.tga", m_iHPAPBar, m_rcHPAPBar);
	LoadTexture("resource\\hud3d_v2\\flashlight.tga", m_iFlashlightBar, m_rcFlashlightBar);

	LoadTexture("resource\\hud3d_v2\\player.tga", m_iMapIcon[0], m_rcMapIcon[0]);
	LoadTexture("resource\\hud3d\\other.tga", m_iMapIcon[1], m_rcMapIcon[1]);

	LoadTexture("resource\\hud3d_v2\\helmet.tga", m_iHelmet, m_rcHelemet);
}

void CHud3D::SetMapBg(char *pszPath)
{
	byte *buffer = new byte[1024 * 768 * 4];
	int width, height;
	LoadBMP(pszPath, buffer, 0, width, height);

	//vgui::surface()->DrawSetTextureRGBA(m_iMapBg, buffer, width, height, true, false);

	Tri_Enable(GL_TEXTURE_2D);
	Tri_BindTexture(GL_TEXTURE_2D, m_iMapBg);
	Tri_SetTextureParam();

	Tri_TexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	if (buffer)
		free(buffer);

	byte *buffer2 = new byte[1024 * 768 * 4];
	LoadBMP2(pszPath, buffer2, 0, width, height);

	//vgui::surface()->DrawSetTextureRGBA(m_iMapBg, buffer, width, height, true, false);

	Tri_Enable(GL_TEXTURE_2D);
	Tri_BindTexture(GL_TEXTURE_2D, m_iMapBg2);
	Tri_SetTextureParam();

	Tri_TexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer2);

	if (buffer2)
		free(buffer2);
}

void CHud3D::CalcPos(int i, int width, int height)
{
	//VectorAdd(v_angles, punchangle, v_angles);
	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(v_angles, forward, right, up);
	

#define SCALE_LEFT	0.15 * 0.24
#define SCALE_RIGHT	0.15 * 0.3

	float sinf, cosf;
	float xo, yo, zo;

	Vector s = gEngfuncs.GetViewModel()->origin;
	m_vecOrigin[i][0] = m_vecOrigin[i][1] = m_vecOrigin[i][2] = m_vecOrigin[i][3] = (s + forward * (90 / Hud().m_FOV - 1) * 135);

	if (viewmodel_widescreen)
	{
		if (viewmodel_widescreen->value)
		{
			for (int j = 0; j < 4; j++)
			{
				m_vecOrigin[i][j] = m_vecOrigin[i][j] + forward * -1.4 + up * -0.6;
			}
		}
	}

	switch (i)
	{
	case 0:
	{
		float w = width * SCALE_LEFT;
		float h = height * SCALE_LEFT;
		
		if ((float)ScreenWidth / ScreenHeight <= 1.34) // 4:3
		{
			xo = 80; yo = -80; zo = 32;
			sinf = sin(M_PI / 10); cosf = cos(M_PI / 10);
		}
		else // 16:9
		{
			xo = 80; yo = -80.2; zo = 16;
			sinf = sin(M_PI / 10); cosf = cos(M_PI / 10);
		}
		
		/*if (Hud().m_FOV > 90)
		{
			float scale = 1.0 + (90 - Hud().m_FOV) * 0.015;
			xo = 80 * scale;
		}*/

		m_vecOrigin[i][0] = m_vecOrigin[i][0] + forward * xo + right * yo + up * -(h + zo);
		m_vecOrigin[i][1] = m_vecOrigin[i][1] + forward * xo + right * yo + up * -zo;
		m_vecOrigin[i][2] = m_vecOrigin[i][2] + forward * (sinf * w + xo) + right * (cosf * w + yo) + up * -zo;
		m_vecOrigin[i][3] = m_vecOrigin[i][3] + forward * (sinf * w + xo) + right * (cosf * w + yo) + up * -(h + zo);

		break;
	}
	case 1:
	{
		float w = width * SCALE_RIGHT;
		float h = height * SCALE_RIGHT;
		
		if ((float)ScreenWidth / ScreenHeight <= 1.34) // 4:3
		{
			xo = 80; yo = 43; zo = 34;
			sinf = sin(M_PI / 13); cosf = cos(M_PI / 13);
		}
		else // 16:9
		{
			xo = 80; yo = 43; zo = 18;
			sinf = sin(M_PI / 13); cosf = cos(M_PI / 13);
		}

		/*if (Hud().m_FOV > 90)
		{
			float scale = 1.0 + (90 - Hud().m_FOV) * 0.015;
			xo = 80 * scale;
		}*/

		m_vecOrigin[i][0] = m_vecOrigin[i][0] + forward * (sinf * w + xo) + right * yo + up * -(h + zo);
		m_vecOrigin[i][1] = m_vecOrigin[i][1] + forward * (sinf * w + xo) + right * yo + up * -zo;
		m_vecOrigin[i][2] = m_vecOrigin[i][2] + forward * xo + right * (cosf * w + yo) + up * -zo;
		m_vecOrigin[i][3] = m_vecOrigin[i][3] + forward * xo + right * (cosf * w + yo) + up * -(h + zo);

		break;
	}
	}
}

void CHud3D::Think(float flTime)
{
	if (!g_bAlive)
		m_iMaxHPAP[0] = m_iMaxHPAP[1] = 100;

	/*if (m_iHPAP[0] < 0)
		m_iHPAP[0] = 0;

	if (m_iHPAP[1] < 0)
		m_iHPAP[1] = 0;*/

	if (flTime > m_flNextReset && m_flNextReset)
	{
		m_iMaxHPAP[0] = m_iMaxHPAP[1] = 100;
		m_iHPAP[0] = 100;
		m_iHPAP[1] = 0;
		m_flNextReset = 0;
	}
	
	if (g_bInReload && m_fReloadTimer == 0)
		m_fReloadTimer = flTime;
	
	if (g_bInReload)
	{
		m_colorReload.r = m_colorReload.g = m_colorReload.b = 255;
		m_colorReload.a = abs(0.5 - (flTime - m_fReloadTimer - int(flTime - m_fReloadTimer))) / 0.5 * 255 * 0.9 + 255 * 0.1;
	}

	if (g_iMoney != m_iOldMoney)
	{
		m_fMoneyTimer = flTime + 1.2;
		m_iMoney = g_iMoney - m_iOldMoney;
		m_iOldMoney = g_iMoney;		
	}
	else
	{
		if (flTime > m_fMoneyTimer)
		{
			m_fMoneyTimer = 0;
			m_iMoney = g_iMoney;
			m_colorMoney.r = m_colorMoney.g = m_colorMoney.b = 255;
			m_colorMoney.a = 235;
		}
		else
		{
			if (m_iMoney < 0)
			{
				m_colorMoney.r = 255;
				m_colorMoney.g = 64;
				m_colorMoney.b = 64;
				m_colorMoney.a = 235 - 235 * 0.8 * (1 - (m_fMoneyTimer - flTime) / 1.2);
			}
			else
			{
				m_colorMoney.r = 0;
				m_colorMoney.g = 255;
				m_colorMoney.b = 32;
				m_colorMoney.a = 200 - 200 * 0.8 * (1 - (m_fMoneyTimer - flTime) / 1.2);
			}			
		}
	}

#define HP_FADE1	0.15
#define HP_FADE2	0.2
#define HP_FADE3	0.5

	if (HudHealth().m_iHealth < m_iHPAP[0])
	{
		// 0.2s -> 255 64 64
		m_flHPTimer = flTime + HP_FADE1;
		m_iHPFade = 1;
	}

	m_colorHP.r = m_colorHP.a = 255;

	if (m_iHPFade == 1)
	{
		if (flTime > m_flHPTimer)
		{		
			m_iHPFade = 2;
			m_flHPTimer = flTime + HP_FADE2;
		}
		else
			m_colorHP.g = m_colorHP.b = 64 + (255 - 64) * (m_flHPTimer - flTime) / HP_FADE1;
	}
	else if (m_iHPFade == 2)
	{
		if ((float)m_iHPAP[0] / m_iMaxHPAP[0] <= 0.15)
		{
			m_iHPFade = 0;
		}
		else if (flTime > m_flHPTimer)
		{
			m_iHPFade = 3;
			m_flHPTimer = flTime + HP_FADE3;
		}
	}
	else if (m_iHPFade == 3)
	{
		if (flTime > m_flHPTimer)
			m_iHPFade = 0;
		else
			m_colorHP.g = m_colorHP.b = 64 + (255 - 64) * (1 - (m_flHPTimer - flTime) / HP_FADE3);
	}
	else
	{
		m_colorHP.r = m_colorHP.a = 255;
		if ((float)m_iHPAP[0] / m_iMaxHPAP[0] <= 0.15)
			m_colorHP.g = m_colorHP.b = 64;
		else
			m_colorHP.g = m_colorHP.b = 255;
	}
		

	if (HudHealth().m_iHealth != m_iHPAP[0])
		m_iHPAP[0] = HudHealth().m_iHealth;
	
	if (m_iHPAP[0] > m_iMaxHPAP[0])
		m_iMaxHPAP[0] = m_iHPAP[0];

	if (HudHealth().m_iArmor != m_iHPAP[1])
		m_iHPAP[1] = HudHealth().m_iArmor;

	if (m_iHPAP[1] > m_iMaxHPAP[1])
		m_iMaxHPAP[1] = m_iHPAP[1];
}

void CHud3D::Redraw(float flTime)
{

	if (!gConfigs.bEnableNewHud || Hud().m_FOV < 90)
		return;
	
	Think(flTime);
	
	if (!g_bAlive)
		return;
		
	CalcPos(0, m_rcBg[0].right, m_rcBg[0].bottom);
	CalcPos(1, m_rcBg[1].right, m_rcBg[1].bottom);

	glDisable(GL_DEPTH_TEST);
	//glDepthMask(false);
	TEXD_SetRenderMode(kRenderTransColor);

	/*TEXD_SetRenderMode(kRenderTransColor);
	glColor4f(1, 1, 1, 1.0);
	DrawBg();*/

	TEXD_SetRenderMode(kRenderTransColor);
	//glColor4f(1, 1, 1, 1.0);

	/*glColor4f(0.6, 0.6, 0.6, 0.8);

	DrawBg();*/
	
	if (hud3d_color->value)
	{
		if (g_iTeam == TEAM_TERRORIST)
			TEXD_SetColor(98, 184, 255, 255);
		else
			TEXD_SetColor(255, 80, 80, 255);
	}
	else
		glColor4f(1, 1, 1, 1.0);
		//glColor4f(0.5, 0.5, 0.5, 1.0);

	DrawBg();

	glColor4f(1, 1, 1, 1.0);
	DrawNumbers();
	DrawHPAP();

	DrawMap();
	DrawFlashlight();

	//glColor4f(0, 0, 0, 1);
	glColor4f(1, 1, 1, 0.92);
	//DrawABC(g_szCurWeapon, 250, 0.137, 0.685);

	glEnable(GL_DEPTH_TEST);

	TEXD_SetRenderMode(kRenderNormal);
}

void CHud3D::DrawBg()
{
	Draw(m_iBg[0], 0, m_rcBg[0].right, m_rcBg[0].bottom, 0, 0);

	if (IS_ZOMBIE_MODE)
		Draw(m_iBg[2], 1, m_rcBg[1].right, m_rcBg[1].bottom, 0, 0);
	else
		Draw(m_iBg[1], 1, m_rcBg[1].right, m_rcBg[1].bottom, 0, 0);
}

void CHud3D::DrawHPAP()
{
	glColor4f(1, 1, 1, 1);

	// (203, 454) 350 * 30
	// (203, 501)

	float width = m_rcHPAPBar.right;
	float heigh = m_rcHPAPBar.bottom;

	for (int i = 0; i <= 1; i++)
	{
		float precent = (float)m_iHPAP[i] / m_iMaxHPAP[i];

		// 203 -> 553 width = 350
		float pos[4][2];
		memset(pos, 0, sizeof(pos));
		pos[0][0] = pos[3][0] = 1 - precent;
		pos[1][0] = pos[2][0] = pos[2][1] = pos[3][1] = 1;

		float x = (float)(203 + (553 - 203) * (1 - precent)) / m_rcBg[1].right;
		float y = ((float)454 + i * (float)(501 - 454)) / m_rcBg[1].bottom;

		/*if ((float)m_iHPAP[0] / m_iMaxHPAP[0] <= 0.15 && i == 0)
			glColor4f(1, 0.25, 0.25, 1);
		else
			*/
		if (i == 0)
			TEXD_SetColor(m_colorHP);
		else
			glColor4f(1, 1, 1, 1);

		if (IS_ZOMBIE_MODE)
		{
			Draw(m_iHPAPBar, 1, width * precent, heigh, pos, x - 0.06, y);
			DrawNumber(1, m_iHPAP[i] <= 0 ? 0 : m_iHPAP[i], 0.22, 0.030, 0.67 - 0.06, 0.75 + i * 0.082, false, 3);
		}
		else
		{
			Draw(m_iHPAPBar, 1, width * precent, heigh, pos, x, y);
			DrawNumber(1, m_iHPAP[i] <= 0 ? 0 : m_iHPAP[i], 0.22, 0.030, 0.67, 0.75 + i * 0.082, false, 3);
		}

	}

	if (HudHealth().m_iArmorType == 1)
	{
		glColor4f(1, 1, 1, 1);
		if (IS_ZOMBIE_MODE)
			Draw(m_iHelmet, 1, m_rcHelemet.right, m_rcHelemet.bottom, 0.55978, 0.84);
		else
			Draw(m_iHelmet, 1, m_rcHelemet.right, m_rcHelemet.bottom, 0.61847, 0.84);
	}

}

void CHud3D::DrawFlashlight()
{
	if (!g_bFlashActive)
		glColor4f(1, 1, 1, 0.5);
	else
		glColor4f(1, 1, 1, 1);
	
	// (183, 82) 400 * 23

	float width = m_rcFlashlightBar.right;
	float heigh = m_rcFlashlightBar.bottom;
	float precent = (float)g_iFlashBat / 100;
	
	// 183 -> 582 width = 400
	float pos[4][2];
	memset(pos, 0, sizeof(pos));
	pos[1][0] = pos[2][0] = precent;
	pos[2][1] = pos[3][1] = 1;

	float x = (float)182 / m_rcBg[0].right;
	float y = (float)82 / m_rcBg[0].bottom;

	Draw(m_iFlashlightBar, 0, width * precent, heigh, pos, x, y);
}

void CHud3D::DrawMap()
{
	/*
	98, 117 500 * 500
	*/

	/*if (g_iTeam == 2)
		TEXD_SetColor(255, 87, 98, 255);
	else if (g_iTeam == 1)
		TEXD_SetColor(141, 206, 255, 255);
	else
		*/glColor4f(1, 1, 1, 1);
	
	// map	
	float xoff = (float)98 / m_rcBg[0].right;
	float yoff = (float)117 / m_rcBg[0].bottom;
	
	int offsetx, offsety;
	CalcPoint(g_vecOrigin, offsetx, offsety);

	// overview 上玩家位置
	Vector2D o;
	o.x = 512 + offsetx;
	o.y = 384 + offsety;

	float pos[4][2];
	int a = 200;

	Vector angles;
	angles = gEngfuncs.GetLocalPlayer()->angles;

	if (HudOverview().m_bIsRotate)
	{
		angles.y = -angles.y - 90;
		angles.y = -angles.y;
	}
	
	angles.y += 45;

	float sinf = sin(0.0174444444444444 * angles.y);
	float cosf = cos(0.0174444444444444 * angles.y);

	int offset[2];
	offset[0] = sinf * a/* * (float(466) / 431)*/;
	offset[1] = cosf * a;
	
	if (HudOverview().m_bIsRotate)
	{
		offset[0] = -offset[0];
		offset[1] = -offset[1];
	}
	pos[0][0] = (o.x - offset[0]) / 1024;
	pos[0][1] = (o.y - offset[1]) / 768;

	pos[1][0] = (o.x + offset[1]) / 1024;
	pos[1][1] = (o.y - offset[0]) / 768;

	pos[2][0] = (o.x + offset[0]) / 1024;
	pos[2][1] = (o.y + offset[1]) / 768;

	pos[3][0] = (o.x - offset[1]) / 1024;
	pos[3][1] = (o.y + offset[0]) / 768;

	Vector origin[4];
	int i = 0;
	int width = 500; 
	int heigth = 500;

	origin[0] = m_vecOrigin[i][1] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * yoff + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * xoff;
	origin[3] = origin[0] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom;
	origin[1] = origin[0] + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;
	origin[2] = origin[0] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;

	glBindTexture(GL_TEXTURE_2D, m_iMapBg);
	glBegin(GL_QUADS);
	
	glTexCoord2f(pos[0][0], pos[0][1]);	glVertex3f(origin[0].x, origin[0].y, origin[0].z);
	glTexCoord2f(pos[1][0], pos[1][1]);	glVertex3f(origin[1].x, origin[1].y, origin[1].z);
	glTexCoord2f(pos[2][0], pos[2][1]);	glVertex3f(origin[2].x, origin[2].y, origin[2].z);
	glTexCoord2f(pos[3][0], pos[3][1]);	glVertex3f(origin[3].x, origin[3].y, origin[3].z);
	glEnd();

	// icons
	/*
	(98, 117)
	           (598, 617)
	*/
	/*float x, y;
	Draw(m_iMapIcon[0], 0, m_rcMapIcon[0].right * SCALE_LEFT, m_rcMapIcon[0].bottom * SCALE_LEFT, 0, 0);*/
	float x = float(98 + 500 / 2 - m_rcMapIcon[0].right / 2 * 2) / m_rcBg[0].right;
	float y = float(117 + 500/ 2 - m_rcMapIcon[0].bottom / 2 * 2) / m_rcBg[0].bottom;
	Draw(m_iMapIcon[0], 0, m_rcMapIcon[0].right * 2, m_rcMapIcon[0].bottom * 2, x, y);


	//Draw(m_iMapIcon[0], 0, m_rcBg[0].right, m_rcBg[0].bottom, 0, 0);

	angles.y -= 45;

	if (HudOverview().m_bIsRotate)
		angles.y += 180;

	for (int i = 1; i <= 32; i++)
	{
		if (g_iMod == MOD_DM)
			break;
		
		cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(i);

		gEngfuncs.pfnGetPlayerInfo(i, &g_PlayerInfoList[i]);

		if (i == gEngfuncs.GetLocalPlayer()->index)
			continue;

		if (!pEntity)
			continue;

		int id = pEntity->index;

		if (!g_PlayerInfoList[i].name)
			continue;

		if (!vPlayer[id].team || vPlayer[id].team == 3 || vPlayer[id].team != g_iTeam)
			continue;

		if (!vPlayer[id].alive)
			continue;

		if (pEntity->origin[0] == 0 && pEntity->origin[1] == 0 && pEntity->origin[2] == 0)
			continue;
		
		CalcPoint(pEntity->origin, offsetx, offsety);

		Vector2D o2;
		o2.x = 512 + offsetx;
		o2.y = 384 + offsety;

		// 自己和该玩家在 bmp 上位置的差
		o2 = o2 - o;
		// bmp 上 212 * 212 -> 画出来的 466 * 466
		//o2 = o2 * 2.1981132075471698113207547169811/*(466.0f / (0.70710678118654752440084436210485 * a * 2))*/;
		o2 = o2 * (500.0f / (0.70710678118654752440084436210485 * a * 2));
		// 旋转一下
		//gEngfuncs.Con_Printf("%f\n", angles.y);
		Vector2D o3;
		sinf = sin(0.0174444444444444 * angles.y);
		cosf = cos(0.0174444444444444 * angles.y);
		o3.x = o2.x * cosf - o2.y * sinf;
		o3.y = o2.x * sinf + o2.y * cosf;
		// width = 0.70710678118654752440084436210485 * a * 2; // sin 45 * 150 * 2
		
		o3.x *= 1.45;
		//o3.y *= 0.61275626423690217294430809304645;
		o3.y *= 0.95238095238095238095238095238095;
		//o3.y *= 1.2;
		//o3 = o3 * Hud().m_TABBoard.m_flOffsetScale / 1.631970260223048;

		//(98, 117) (598, 617) 700*800
		// x 0.14 -> 0.86
		// y 0.15 -> 0.77

		// x 0.145 -> 0.855
		// y 0.155 -> 0.765

#define SCALE_MAX	2.0
#define SCALE_MIN	1.2
#define S			0.008
#define LEFT		0.145
#define RIGHT		0.84
#define TOP			0.155
#define BOTTOM		0.765
		
		float scale = SCALE_MAX;
		float x = float(98 + 500 / 2) / m_rcBg[0].right + (o3.x / 1024);
		float y = float(117 + 500 / 2) / m_rcBg[0].bottom + (o3.y / 768);

		
		if (x < LEFT)									{ x = LEFT; scale = SCALE_MIN; }
		else if (x < LEFT + S)							{ scale = SCALE_MAX - (LEFT + S - x) / S * (SCALE_MAX - SCALE_MIN); }
		if (x > RIGHT)									{ x = RIGHT; scale = SCALE_MIN; }
		else if (x > RIGHT - S)							{ scale = SCALE_MAX - (x - (RIGHT - S)) / S * (SCALE_MAX - SCALE_MIN); }
		
		if (y < TOP)									{ y = TOP; scale = SCALE_MIN; }
		else if (y < TOP + S && scale == SCALE_MAX)		{ scale = SCALE_MAX - (TOP + S - y) / S * (SCALE_MAX - SCALE_MIN); }
		if (y > BOTTOM)									{ y = BOTTOM; scale = SCALE_MIN; }
		else if (y > BOTTOM - S && scale == SCALE_MAX)	{ scale = SCALE_MAX - (y - (BOTTOM - S)) / S * (SCALE_MAX - SCALE_MIN); }

		x -= m_rcMapIcon[1].right / 2 * scale / m_rcBg[0].right;
		y -= m_rcMapIcon[1].bottom / 2 * scale / m_rcBg[0].bottom;

		/*Vector angles2 = pEntity->angles;
		angles2.y = 0;
		Vector forward, right;
		gEngfuncs.pfnAngleVectors(angles2, forward, right, NULL);*/

		width = m_rcMapIcon[1].right * scale;
		heigth = m_rcMapIcon[1].bottom * scale;

		//Draw(m_iMapIcon[1], 2, width, heigth, x, y);

		Vector vec[2];
		vec[0] = m_vecOrigin[0][2] - m_vecOrigin[0][1]; // x
		vec[1] = m_vecOrigin[0][0] - m_vecOrigin[0][1]; // y

		origin[0] = m_vecOrigin[0][1] + vec[1] * y + vec[0] * x; // 00 
		//origin[1] = origin[0] + vec[0] * width / m_rcBg[0].right; // 10
		origin[2] = origin[0] + vec[1] * heigth / m_rcBg[0].bottom + vec[0] * width / m_rcBg[0].right; // 11
		//origin[3] = origin[0] + vec[1] * heigth / m_rcBg[0].bottom; // 01

		Vector center = origin[0] + (origin[2] - origin[0]) / 2;
		vec[0] = vec[0].Normalize();
		vec[1] = vec[1].Normalize();

		float degree = pEntity->angles[1];
		
		if (HudOverview().m_bIsRotate)
		{
			degree = -degree - 90;
			degree = -degree;
		}

		if (HudOverview().m_bIsRotate)
			degree += 180;

		degree -= angles.y;
		degree += 45;

		sinf = sin(0.0174444444444444 * degree) * scale;
		cosf = cos(0.0174444444444444 * degree) * scale;

		origin[0] = center - vec[0] * sinf - vec[1] * cosf;
		origin[1] = center + vec[0] * cosf - vec[1] * sinf;
		origin[2] = center + vec[0] * sinf + vec[1] * cosf;
		origin[3] = center - vec[0] * cosf + vec[1] * sinf;

		glBindTexture(GL_TEXTURE_2D, m_iMapIcon[1]);
		glBegin(GL_QUADS);

		glTexCoord2f(0, 0);	glVertex3f(origin[0].x, origin[0].y, origin[0].z);
		glTexCoord2f(1, 0);	glVertex3f(origin[1].x, origin[1].y, origin[1].z);
		glTexCoord2f(1, 1);	glVertex3f(origin[2].x, origin[2].y, origin[2].z);
		glTexCoord2f(0, 1);	glVertex3f(origin[3].x, origin[3].y, origin[3].z);
		glEnd();
	}
}

void CHud3D::DrawNumbers()
{
	glColor4f(1, 1, 1, 0.92);

	// clip + ammo
	if (g_iWeapon != WEAPON_KNIFE)
	{
		if (!g_bInReload)
		{			
			int clip = g_iWeaponData[g_iWeapon].iClip;
			int ammo = HudAmmo().m_iAmmo[g_iWeaponData[g_iWeapon].iAmmoType];
			
			// 1, 5 svdex (5: ammo = 0 hide)
			if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 2)
			{
				clip = -1;
			}
			else if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 3)
			{
				ammo = clip;
				clip = -1;
			}
			/*else if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay)
			{
				ammo += clip;
				clip = -1;
			}*/
			if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 1 || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 5 && HudAmmo().m_iExtraAmmo) || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6 && HudAmmo().m_iExtraAmmo))
			{
				glColor4f(0.58, 0.77, 1, 1);
				DrawNumber(1, HudAmmo().m_iExtraAmmo, 0.2, 0.027, 0.676, 0.550, true, 3);
			}

			if (clip != -1)
			{
				glColor4f(1, 1, 1, 0.1);
				DrawNumber(1, 0, 0.46, 0.062, 0.460, 0.545, true, 3, true);
				DrawNumber(1, 0, 0.22, 0.029, 0.670, 0.605, true, 3, true);
				
				Draw(m_iLine, 1, m_rcLine.right * 0.16, m_rcLine.bottom * 0.16, 0.65, 0.59);
				
				float fSubTime = Hud().m_flTime;
				fSubTime -= (int)fSubTime;
				fSubTime -= 0.5;
				fSubTime = fabsf(fSubTime);
				float a = fSubTime / 0.5 * 0.1 + 0.2;

				auto &CurWeapon = WeaponManager().GetCurWeapon();

				if (float(clip) / CurWeapon.iClip < 0.17)
					glColor4f(1, a, a, 0.92);
				else
					glColor4f(1, 1, 1, 0.92);
				DrawNumber(1, clip, 0.46, 0.062, 0.460, 0.545, true, 3);
								
				if (float(ammo) / CurWeapon.iAmmo < 0.1)
					glColor4f(1, 0.25, 0.25, 0.92);
				else
					glColor4f(1, 1, 1, 0.92);
				DrawNumber(1, ammo, 0.22, 0.029, 0.670, 0.605, true, 3);
				
				if (float(clip) / CurWeapon.iClip < 0.17 && float(ammo) / CurWeapon.iAmmo < 0.1)
					glColor4f(1, 0.25, 0.25, 0.92);
				else
					glColor4f(1, 1, 1, 0.92);
				Draw(m_iLine, 1, m_rcLine.right * 0.16, m_rcLine.bottom * 0.16, 0.65, 0.59);
			}
			else
			{
				glColor4f(1, 1, 1, 0.1);
				DrawNumber(1, 0, 0.44, 0.06, 0.575, 0.545, true, 3, true);

				glColor4f(1, 1, 1, 0.92);
				DrawNumber(1, ammo, 0.44, 0.06, 0.575, 0.545, true, 3);
			}
		}
		else
		{
			TEXD_SetColor(m_colorReload);
			Draw(m_iReloading, 1, m_rcReloading.right * 0.28, m_rcReloading.bottom * 0.28, 0.465, 0.57);
		}
	}
	/*else
	{
		glColor4f(1, 1, 1, 0.1);
		DrawNumber(1, 0, 0.44, 0.06, 0.575, 0.545, true, 3, true);
	}*/
	
	if ((!IS_ZOMBIE_MODE || g_iMod == MOD_ZB5) && (g_iMod != MOD_TDM && g_iMod != MOD_DM))
	{
		glColor4f(1, 1, 1, 0.1);
		DrawNumber(1, 0, 0.34, 0.045, 0.53, 0.365, true, 5, true);

		// money
		TEXD_SetColor(m_colorMoney);
		//glColor4f(1, 1, 1, 0.92);

#define DOLLAR_SCALE 0.32

		if (m_iMoney == m_iOldMoney)
			Draw(m_iDollar[0], 1, m_rcDollar.right * DOLLAR_SCALE, m_rcDollar.bottom * DOLLAR_SCALE, 0.776, 0.352);
		else if (m_iMoney < 0)
			Draw(m_iDollar[2], 1, m_rcDollar.right * DOLLAR_SCALE, m_rcDollar.bottom * DOLLAR_SCALE, 0.776, 0.352);
		else
			Draw(m_iDollar[1], 1, m_rcDollar.right * DOLLAR_SCALE, m_rcDollar.bottom * DOLLAR_SCALE, 0.776, 0.352);

		DrawNumber(1, abs(m_iMoney), 0.34, 0.045, 0.53, 0.365, true, 5);
	}
	// HP AP
	glColor4f(1, 1, 1, 0.92);

	// timer
	if (0/*HudRoundTimer().m_timer / 60 >= 10*/)
	{
		DrawNumber(0, HudRoundTimer().m_timer / 60, 0.3, 0.055, 0.66, 0.83, true, 2, false);
		DrawNumber(0, HudRoundTimer().m_timer % 60, 0.3, 0.055, 0.79, 0.83, true, 2, true);
	}
	else
	{
		DrawNumber(0, HudRoundTimer().m_timer / 60, 0.3, 0.055, 0.63, 0.83, true, 2, false);
		DrawNumber(0, HudRoundTimer().m_timer % 60, 0.3, 0.055, 0.76, 0.83, true, 2, true);
	}
	// score
	if (IS_ZOMBIE_MODE || g_iMod == MOD_NONE || g_iMod == MOD_GHOST)
	{
		TEXD_SetColor(216, 81, 80, 255);
		DrawNumber(0, HudScoreBoard().m_iLeft2, 0.3, 0.055, 0.225, 0.833, true, 2);
		TEXD_SetColor(173, 201, 235, 255);
		DrawNumber(0, HudScoreBoard().m_iRight2, 0.3, 0.055, 0.426, 0.833);
	}
	else if (g_iMod == MOD_TDM)
	{
		TEXD_SetColor(216, 81, 80, 255);
		DrawNumber(0, HudScoreBoard().m_iLeft, 0.22, 0.040, 0.220, 0.843, true, 3, true);
		TEXD_SetColor(173, 201, 235, 255);
		DrawNumber(0, HudScoreBoard().m_iRight, 0.22, 0.040, 0.425, 0.843, false, 3, true);
	}
	

	
	
	/*TEXD_SetColor(216, 81, 80, 255);
	DrawNumber(0, HudScoreBoard().m_iLeft, 0.28, 0.055, 0.225, 0.818, true, 2, 0);
	DrawNumber(0, HudScoreBoard().m_iLeft2, 0.15, 0.03, 0.275, 0.873, true, 2, 0);
	TEXD_SetColor(173, 201, 235, 255);
	DrawNumber(0, HudScoreBoard().m_iRight, 0.28, 0.055, 0.426, 0.818, 0, 2, 0);
	DrawNumber(0, HudScoreBoard().m_iRight2, 0.15, 0.03, 0.426, 0.873, 0, 2, 0);*/

	glColor4f(1, 1, 1, 0.92);
}

void CHud3D::Draw(int id, int i, int width, int heigth, float xoff, float yoff)
{
	Vector origin[4];
	// 左下 左上 右上 右下
	origin[1] = m_vecOrigin[i][1] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * yoff + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * xoff;
	origin[0] = origin[1] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom;
	origin[2] = origin[1] + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;
	origin[3] = origin[1] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;

	DrawFinal(id, origin);
}

void CHud3D::Draw(int id, int i, int width, int heigth, float pos[4][2], float xoff, float yoff)
{
	Vector origin[4];
	origin[0] = m_vecOrigin[i][1] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * yoff + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * xoff;
	origin[3] = origin[0] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom;
	origin[1] = origin[0] + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;
	origin[2] = origin[0] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;
	
	DrawFinal(id, pos, origin);
}

void CHud3D::DrawFinal(int id, Vector origin[4])
{
	glBindTexture(GL_TEXTURE_2D, id);
	glBegin(GL_QUADS);	

	/*glTexCoord2f(0.0f, 1.0f);	glVertex3f(origin[0].x, origin[0].y, origin[0].z);
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(origin[1].x, origin[1].y, origin[1].z);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(origin[2].x, origin[2].y, origin[2].z);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(origin[3].x, origin[3].y, origin[3].z);*/	
	
	glTexCoord2f(0.0f, 1.0f);	glVertex3fv(origin[0]);
	glTexCoord2f(0.0f, 0.0f);	glVertex3fv(origin[1]);
	glTexCoord2f(1.0f, 0.0f);	glVertex3fv(origin[2]);
	glTexCoord2f(1.0f, 1.0f);	glVertex3fv(origin[3]);

	glEnd();
}

void CHud3D::DrawFinal(int id, float pos[4][2], Vector origin[4])
{
	glBindTexture(GL_TEXTURE_2D, id);
	glBegin(GL_QUADS);

	glTexCoord2fv(pos[0]);	glVertex3fv(origin[0]);
	glTexCoord2fv(pos[1]);	glVertex3fv(origin[1]);
	glTexCoord2fv(pos[2]);	glVertex3fv(origin[2]);
	glTexCoord2fv(pos[3]);	glVertex3fv(origin[3]);

	/*glTexCoord2f(pos[0][0], pos[0][1]);	glVertex3f(origin[0].x, origin[0].y, origin[0].z);
	glTexCoord2f(pos[1][0], pos[1][1]);	glVertex3f(origin[1].x, origin[1].y, origin[1].z);
	glTexCoord2f(pos[2][0], pos[2][1]);	glVertex3f(origin[2].x, origin[2].y, origin[2].z);
	glTexCoord2f(pos[3][0], pos[3][1]);	glVertex3f(origin[3].x, origin[3].y, origin[3].z);*/
	glEnd();
}

void CHud3D::CalcPoint(const float *origin, int &x, int &y)
{
	float flOffsetScale = HudOverview().m_flZoom / (768 * 0.761718750 * 0.8) * 24.9 * (0.761718750 / 0.6328125) * (0.761718750 / 0.6328125);
	
	x = (HudOverview().m_vecOrigin.x - origin[0]) * flOffsetScale * 1.631970260223048;
	y = (HudOverview().m_vecOrigin.y - origin[1]) * flOffsetScale * 1.631970260223048;

	if (!HudOverview().m_bIsRotate)
	{
		float tempx = x;
		x = y;
		y = tempx;
	}
	else
	{
		x = -x;
		y = y;
	}
}

void CHud3D::DrawNumber(int part, int num, float sclae, float interval, float xoff, float yoff, bool right_start, int len, bool draw_zero)
{
	if (num < 0)
		num = 0;
	
	int number[10];
	memset(number, 0, sizeof(number));
	char ch[11];
	sprintf(ch, "%d", num);
	int n = strlen(ch);

	for (int i = 0; i < n; i++)
		number[i] = ch[i] - '0';

	if (draw_zero)
	{		
		for (int i = 0; i < (len - n); i++)
		{
			number[i + len - n] = number[i];
			number[i] = 0;
		}
		n = len;
	}

	for (int i = 0; i < n; i++)
	{
		if (!right_start)
			Draw(m_iNumber[number[i]], part, m_rcNumber[0].right * sclae, m_rcNumber[0].bottom * sclae, xoff + i * interval, yoff);
		else
			Draw(m_iNumber[number[i]], part, m_rcNumber[0].right * sclae, m_rcNumber[0].bottom * sclae, xoff + (i + len - n) * interval, yoff);
	}
}

/*Vector ori = gEngfuncs.GetLocalPlayer()->origin;
Vector view_height;
gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(view_height);*/

//zo += view_height.z;

//origin[0] = origin[1] = origin[2] = origin[3] = g_vecEyePos;
//origin[0] = origin[1] = origin[2] = origin[3] = ori + view_height;
//origin[0] = origin[1] = origin[2] = origin[3] = vieworg;

/*TEXD_SetRenderMode(kRenderTransTexture);
//TEXD_SetRenderMode(kRenderNormal);
TEXD_SetColor(255, 255, 255, 255);
TEXD_BindTexture(m_iBg[0]);
int x, y, w, h;
x = ScreenWidth - 400;
y = ScreenHeight - 300;
w = m_rcBg[0].right * 1.2;
h = m_rcBg[0].bottom * 1.2;

glBegin(GL_QUADS);
glTexCoord2f(0, 0);
glVertex2f(x, y);

glTexCoord2f(1, 0);
glVertex2f(x + w, y + 20);

glTexCoord2f(1, 1);
glVertex2f(x + w, y + h + 60);

glTexCoord2f(0, 1);
glVertex2f(x, y + h);
glEnd();

TEXD_EndDraw();*/

//glFrustum(0, 0, 1600, 900, 45, 0);

//float scale = 90 / Hud().m_FOV;
//xo = 80 * scale;

//xo -= (Hud().m_FOV - 90) * 1;
