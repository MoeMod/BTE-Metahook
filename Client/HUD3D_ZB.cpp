#include "metahook.h"
#include "bte_const.h"
#include "exportfuncs.h"
#include "message.h"
#include "hud.h"
#include "configs.h"
#include "BaseUI.h"
#include "texdraw.h"
#include "HUD3D_ZB.h"
#include "msg_util.h"
#include "TextureManager.h"

#include "Client/HUD/DrawTGA.h"

CHud3D_ZB gHud3D_ZB;

cvar_t *hud3d_color2;

void CHud3D_ZB::LoadTexture(char *name, int &id, wrect_t &rc)
{
	auto &Texture = TextureManager()[name];
	id = Texture;

	rc.right = Texture.w();
	rc.bottom = Texture.h();
}

void CHud3D_ZB::Init()
{	
	hud3d_color2 = gEngfuncs.pfnGetCvarPointer("hud3d_color");

}

void CHud3D_ZB::VidInit()
{
	m_flNextCalc = 0.0f;

	for (int i = 0; i < 4;i++)
	{ 
		m_icon[i].type = -1;
		m_icon[i].time = 0.0f;
		m_icon[i].flEndTime = gEngfuncs.GetAbsoluteTime();
	}
	//CalcPos(1, m_rcBg[1].right, m_rcBg[1].bottom);

	LoadTexture("resource\\hud3d_v2\\zombie\\SKILLBOARD.tga", m_iBg[0], m_rcBg[0]);
	LoadTexture("resource\\hud3d_v2\\zombie\\SKILLBOARD.tga", m_iBg[2], m_rcBg[0]);
	LoadTexture("resource\\hud3d_v2\\zombie\\HM BAR.tga", m_iBg[1], m_rcBg[1]);
	LoadTexture("resource\\hud3d_v2\\zombie\\ZB BAR.tga", m_iBg[3], m_rcBg[1]);

	LoadTexture("resource\\hud3d_v2\\zombie\\BAR.tga", m_iBar, m_rcBar);
}

void CHud3D_ZB::CalcPos(int i, int width, int height)
{
	//VectorAdd(v_angles, punchangle, v_angles);
	Vector forward, right, up;
	gEngfuncs.pfnAngleVectors(v_angles, forward, right, up);


#define SCALE_LEFT	0.15 * 0.2
#define SCALE_RIGHT	0.15 * 0.2

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
			xo = 80; yo = -77.5; zo = 10;
			sinf = sin(M_PI / 10); cosf = cos(M_PI / 10);
		}
		else // 16:9
		{
			xo = 80; yo = -77.5; zo = -6;
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
			xo = 80; yo = 70.5; zo = 8;
			sinf = sin(M_PI / 13); cosf = cos(M_PI / 13);
		}
		else // 16:9
		{
			xo = 80; yo = 70.5; zo = -8;
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

void CHud3D_ZB::RoundStart()
{
	m_flNextCalc = gEngfuncs.GetAbsoluteTime() + 2.0F;

	m_flAtk[0] = m_flAtk[1] = 1.0;

	m_flAdd[0] = 0.01;
	m_flAdd[1] = 0.01;

	/*if (g_bAlive)
	{

	}*/
}

void CHud3D_ZB::Reset()
{
	m_flPrecent[0] = m_flPrecent[1] = 0.0F;
	m_flNextCalc = -1;

	/*MSG_BeginWrite("TutorText");
	MSG_WriteString("#CSBTE_ZB5_Totur_RoundStart");
	MSG_WriteByte(0);
	MSG_WriteShort(0); // TutorMessageEventId
	MSG_WriteShort(0); // IsDead
	MSG_WriteShort(1 << 0); // Type
	MSG_EndWrite();*/

	
}

void CHud3D_ZB::SetAdd(int i, float f)
{
	m_flAdd[i] += f;
}

void CHud3D_ZB::SetAtk(int i, float f)
{
	m_flAtk[i] += f;

	m_flPrecent[0] = (m_flAtk[0] - 1) / (m_flMax - 1);
	m_flPrecent[1] = (m_flAtk[1] - 1) / (m_flMax - 1);
}

void CHud3D_ZB::Think(float flTime)
{
	if (flTime < m_flNextCalc || m_flNextCalc == -1)
		return;
	
	m_flNextCalc = flTime + 1.0F;
	
	if (g_iMod == MOD_ZB5)
	{ 
		cl_entity_t *entity = NULL;

		m_flMax = 1.0F;
		Vector origin[33];
		bool vaild[33];

		for (int id = 1; id <= 32; id++)
		{
			entity = gEngfuncs.GetEntityByIndex(id);
			if (entity->player)
			{
				m_flMax += 0.1F;

				if (vPlayer[id].team != TEAM_TERRORIST)
					vaild[id] = true;
				else
					vaild[id] = false;

				origin[id] = entity->origin;
			}
		}

		int count = 0;
		for (int id = 1; id <= 32; id++)
		{
			if (!vaild[id])
				continue;

			for (int id2 = id + 1; id2 <= 32; id2++)
			{
				if (!vaild[id2])
					continue;

				if ((origin[id] - origin[id2]).Length() < 400.0)
					count++;
			}
		}

		if (count >= 2 && count <= 4)
			m_flAdd[0] = 0.02;
		else if (count >= 5 && count <= 8)
			m_flAdd[0] = 0.03;
		else if (count >= 9 && count <= 12)
			m_flAdd[0] = 0.04;
		else if (count >= 13)
			m_flAdd[0] = 0.06;

		m_flAtk[0] += m_flAdd[0];
		m_flAtk[1] += m_flAdd[1];

		m_flAtk[0] = m_flAtk[0] > (m_flMax * 1.3) ? (m_flMax * 1.3) : m_flAtk[0];
		m_flAtk[1] = m_flAtk[1] > m_flMax ? m_flMax : m_flAtk[1];

		m_flPrecent[0] = (m_flAtk[0] - 1) / (m_flMax * 1.3 - 1);
		m_flPrecent[1] = (m_flAtk[1] - 1) / (m_flMax - 1);
	}
}

void CHud3D_ZB::Redraw(float flTime)
{
	if (g_iMod != MOD_ZB3 && g_iMod != MOD_ZB5)
		return;

	Think(flTime);
	
	if (!gConfigs.bEnableNewHud || Hud().m_FOV < 90)
		return;	

	//Think(flTime);

	if (!g_bAlive)
		return;

	CalcPos(0, m_rcBg[0].right, m_rcBg[0].bottom);
	CalcPos(1, m_rcBg[1].right, m_rcBg[1].bottom);

	glDisable(GL_DEPTH_TEST);
	//glDepthMask(false);
	TEXD_SetRenderMode(kRenderTransColor);

	/*glColor4f(0.6, 0.6, 0.6, 0.8);
	DrawBg();*/

	if (hud3d_color2->value)
	{
		if (g_iTeam == TEAM_TERRORIST)
			TEXD_SetColor(98, 184, 255, 255);
		else
			TEXD_SetColor(255, 80, 80, 255);
	}
	else
		glColor4f(1, 1, 1, 1.0);

	DrawBg();

	glColor4f(1, 1, 1, 1.0);
	DrawSkill(flTime);
	DrawBar();

	glEnable(GL_DEPTH_TEST);

	TEXD_SetRenderMode(kRenderNormal);
}

void CHud3D_ZB::DrawBg()
{
	if (g_iTeam != TEAM_CT)
	{
		Draw(m_iBg[0], 0, m_rcBg[0].right, m_rcBg[0].bottom, 0, -198.0 / m_rcBg[0].bottom);
		Draw(m_iBg[1], 1, m_rcBg[1].right, m_rcBg[1].bottom, 0, 0);
	}
	else
	{
		Draw(m_iBg[2], 0, m_rcBg[0].right, m_rcBg[0].bottom, 0, -198.0 / m_rcBg[0].bottom);
		Draw(m_iBg[3], 1, m_rcBg[1].right, m_rcBg[1].bottom, 0, 0);
	}
}

void CHud3D_ZB::DrawSkill(float flTime)
{
	// (52, 92) 190 * 190
	// (52, 290) 190 * 190
	// (52, 488) 190 * 190

	float width = 190;
	float heigh = 190;
	//float precent = (float)g_iFlashBat / 100;

	float pos[4][2];
	pos[0][0] = 0; pos[0][1] = 0;
	pos[1][0] = 1; pos[1][1] = 0;
	pos[2][0] = 1; pos[2][1] = 1;
	pos[3][0] = 0; pos[3][1] = 1;

	float x = (float)53 / m_rcBg[0].right;
	

	/*for (int i = 0; i <= 2; i++)
		Draw(m_iSkill[2 - i], 0, width, heigh, pos, x, y + (198.0F / m_rcBg[0].bottom) * i);*/

	for (int i = 0; i < 4; i++)
	{
		if (m_icon[i].type <= 0)
			continue;

		if (m_icon[i].type == 3 && flTime >= m_icon[i].flEndTime)
		{
			//m_icon[i].type = 0;
			//m_icon[i].flEndTime = flTime + 20.0F;

			m_icon[i].type = 2;
			m_icon[i].flEndTime = flTime + 2.0F;
		}

		if (m_icon[i].type == 2 && flTime >= m_icon[i].flEndTime)
		{
			m_icon[i].type = 1;
		}

		float y = (float)(488 - 198 * i) / m_rcBg[0].bottom;

		if (m_icon[i].type >= 3) // reload / disable
		{
			glColor4f(1, 1, 1, 0.2);
			Draw(m_icon[i].texture, 0, width, heigh, pos, x, y);
		}

		if (m_icon[i].type == 1)
		{
			glColor4f(1, 1, 1, 1);
			Draw(m_icon[i].texture, 0, width, heigh, pos, x, y);
		}

		if (m_icon[i].type == 2) // blink
		{
			int t = m_icon[i].flEndTime - flTime;
			float a = abs((m_icon[i].flEndTime - flTime - t) - 0.5F) / 0.5 * 0.9 + 0.1;
			glColor4f(1, 1, 1, a);
			Draw(m_icon[i].texture, 0, width, heigh, pos, x, y);
		}

		if (m_icon[i].type == 3) // reload
		{
			glColor4f(1, 1, 1, 1);

			float precent = (m_icon[i].flEndTime - flTime) / m_icon[i].time;
			precent = precent > 1.0F ? 1.0F : precent;

			float pos[4][2];
			pos[0][0] = 0; pos[0][1] = precent;
			pos[1][0] = 1; pos[1][1] = precent;
			pos[2][0] = 1; pos[2][1] = 1;
			pos[3][0] = 0; pos[3][1] = 1;

			Draw(m_icon[i].texture, 0, width, heigh * (1 - precent), pos, x, y + heigh * precent / m_rcBg[0].bottom);
		}
	}


	/*float x = (float)53 / m_rcBg[0].right;
	float y = (float)92 / m_rcBg[0].bottom;

	Draw(m_iSkill[2], 0, width, heigh, pos, x, y);

	x = (float)53 / m_rcBg[0].right;
	y = (float)290 / m_rcBg[0].bottom;

	Draw(m_iSkill[1], 0, width, heigh, pos, x, y);

	x = (float)53 / m_rcBg[0].right;
	y = (float)488 / m_rcBg[0].bottom;

	Draw(m_iSkill[0], 0, width, heigh, pos, x, y);*/
}

void CHud3D_ZB::DrawBar()
{
	// (113, 72) 106 * 768

	glColor4f(1, 1, 1, 1);
	
	float width = m_rcBar.right;
	float heigh = m_rcBar.bottom;
	float precent = m_flPrecent[g_iTeam == TEAM_CT];

	float pos[4][2];
	pos[0][0] = 0; pos[0][1] = 1 - precent;
	pos[1][0] = 1; pos[1][1] = 1 - precent;
	pos[2][0] = 1; pos[2][1] = 1;
	pos[3][0] = 0; pos[3][1] = 1;

	float x = (float)113 / m_rcBg[1].right;
	float y = (float)(72 + heigh * (1 - precent)) / m_rcBg[1].bottom;

	Draw(m_iBar, 1, width, heigh * precent, pos, x, y);
}

void CHud3D_ZB::Draw(int id, int i, int width, int heigth, float xoff, float yoff)
{
	Vector origin[4];
	// 左下 左上 右上 右下
	origin[1] = m_vecOrigin[i][1] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * yoff + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * xoff;
	origin[0] = origin[1] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom;
	origin[2] = origin[1] + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;
	origin[3] = origin[1] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;

	DrawFinal(id, origin);
}

void CHud3D_ZB::Draw(int id, int i, int width, int heigth, float pos[4][2], float xoff, float yoff)
{
	Vector origin[4];
	origin[0] = m_vecOrigin[i][1] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * yoff + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * xoff;
	origin[3] = origin[0] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom;
	origin[1] = origin[0] + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;
	origin[2] = origin[0] + (m_vecOrigin[i][0] - m_vecOrigin[i][1]) * heigth / m_rcBg[i].bottom + (m_vecOrigin[i][2] - m_vecOrigin[i][1]) * width / m_rcBg[i].right;

	DrawFinal(id, pos, origin);
}

void CHud3D_ZB::DrawFinal(int id, Vector origin[4])
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

void CHud3D_ZB::DrawFinal(int id, float pos[4][2], Vector origin[4])
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

void CHud3D_ZB::SetSkill(char *pszName, int iSlot, int type, float time)
{
	if (iSlot < 1 || iSlot>4)
		return;
	m_icon[iSlot - 1].texture = g_Texture[Hud().m_TGA.FindTexture(pszName)].iTexture;
	m_icon[iSlot - 1].type = type;
	m_icon[iSlot - 1].time = time;
	m_icon[iSlot - 1].flEndTime = gEngfuncs.GetAbsoluteTime() + time;
}