#include "base.h"
#include "BaseUI.h"
#include "msghook.h"

void CHudZB4UITip::Init()
{	
	
}

void CHudZB4UITip::VidInit()
{	
	if(!m_iBg[0] || !m_iBg[1])
	{
	m_iBg[0] = vgui::surface()->CreateNewTextureID();
	LoadTexture("resource\\hud\\zombie\\z4_hmmsgbg", m_iBg[0], m_rcBg[0]);
	
	m_iBg[1] = vgui::surface()->CreateNewTextureID();
	LoadTextureTga("cstrike\\resource\\hud\\zombie\\z4_zbmsgbg.tga", m_iBg[1], m_rcBg[1].right, m_rcBg[1].bottom);
	}

	//LoadTexture("resource\\hud\\zombie\\z4_hmmsgbg", m_iBg[0], m_rcBg[0]);
	//LoadTexture("resource\\hud\\zombie\\z4_zbmsgbg", m_iBg[1], m_rcBg[1]);

	memset(m_szText, 0, sizeof(m_szText));

	m_iTeam = 2;
	m_flTimeStart = 0.0f;
	m_flTimeEnd = 0.0f;
	m_flDisplayEnd = 0.0f;
	m_iCount = -1;

	LoadText("CSBTE_ZB4_MSG_Human_RoundStart5_1");
	LoadText("CSBTE_ZB4_MSG_Human_RoundStart5_2");
	LoadText("CSBTE_ZB4_MSG_Human_RoundStart5_3");

	LoadText("CSBTE_ZB4_MSG_Human_RoundStart22_1");
	LoadText("CSBTE_ZB4_MSG_Human_RoundStart22_2");
	LoadText("CSBTE_ZB4_MSG_Human_RoundStart22_3");

	LoadText("CSBTE_ZB4_MSG_Human_NightBegin_1");
	LoadText("CSBTE_ZB4_MSG_Human_NightBegin_2");
	LoadText("CSBTE_ZB4_MSG_Human_NightBegin_3");

	LoadText("CSBTE_ZB4_MSG_Human_NightEnd_1");
	LoadText("CSBTE_ZB4_MSG_Human_NightEnd_2");
	LoadText("CSBTE_ZB4_MSG_Human_NightEnd_3");

	LoadText("CSBTE_ZB4_MSG_Zombi_FirstZombi_1");
	LoadText("CSBTE_ZB4_MSG_Zombi_FirstZombi_2");
	LoadText("CSBTE_ZB4_MSG_Zombi_FirstZombi_3");

	LoadText("CSBTE_ZB4_MSG_Zombi_NightBegin_1");
	LoadText("CSBTE_ZB4_MSG_Zombi_NightBegin_2");
	LoadText("CSBTE_ZB4_MSG_Zombi_NightBegin_3");

	LoadText("CSBTE_ZB4_MSG_Zombi_NightEnd_1");
	LoadText("CSBTE_ZB4_MSG_Zombi_NightEnd_2");
	LoadText("CSBTE_ZB4_MSG_Zombi_NightEnd_3");

	m_iNightStart = 100;
	m_iNightEnd = 40;

	g_iZ4Nvg = 0;
}

void CHudZB4UITip::Redraw(float flTime)
{
	if(g_iRoundTime < 15)
		return;
	
	m_iTeam = g_iTeam - 1;

	int time = m_flTimeEnd - flTime + 1;

	if(time > 180)
		return;

	int iX, iY;
	iX = ScreenWidth / 2 - m_rcBg[0].right / 2;
	iY = ScreenHeight * 0.18;

	if (m_iTeam == 1)
	{
		if(time > m_iNightEnd - 10 && time <= m_iNightStart + 10)
		{
			gHUD.m_ZB4.m_Retina[2].SetCanDraw(true);
			gHUD.m_ZB4.m_Retina[2].SetEndTime(flTime + 9999.0);
		}

		if (time >= m_iNightStart && time <= m_iNightStart + 10)
			gHUD.m_ZB4.m_Retina[2].SetColor(255, 255, 255, float(m_iNightStart + 10.0 - (m_flTimeEnd - flTime + 1.0)) / 10.0 * 255);

		if (time >= m_iNightEnd - 10 && time <= m_iNightEnd)
			gHUD.m_ZB4.m_Retina[2].SetColor(255, 255, 255, float((m_flTimeEnd - flTime + 1.0) - m_iNightEnd) / 10.0 * 255 + 255);
	}
	else
	{
		gHUD.m_ZB4.m_Retina[2].SetCanDraw(false);
	}	

	if (g_iZ4Nvg == 0 && g_bAlive)
	{
		g_iZ4Nvg = 1;
		if (time <= m_iNightStart && time >= m_iNightEnd)
			g_iZ4Nvg = 2;
	}

	if(m_iCount != time)
	{
		if(time == 180)
		{
			gEngfuncs.pfnClientCmd("mp3 stop");
			gEngfuncs.pfnClientCmd("mp3 play sound//music//ZB4_start.mp3");			

			gHUD.m_ZB4.m_Icon.AddIcon(0, 0, 2);

			m_flDisplayEnd = 0.0f;			
		}		

		if(time == m_iNightStart + 10)
		{
			g_iZ4Nvg = 3;
			g_flZ4Nvg = gHUD.m_flTime;
		}

		if(time == m_iNightEnd + 10)
		{
			g_iZ4Nvg = 4;
			g_flZ4Nvg = gHUD.m_flTime + 10.0f;
		}

		if(time == m_iNightStart + 5)
		{
			gEngfuncs.pfnClientCmd("mp3 stop");
			gEngfuncs.pfnClientCmd("mp3 play sound//music//Play_BGM_Rex.mp3");
		}

		if(time == m_iNightStart)
		{
			g_iZ4Nvg = 2;

			if(m_iTeam == 0)
			{
				gHUD.m_ZB4.m_Icon.AddIcon(5, 3, 2);

				m_flDisplayEnd = gHUD.m_flTime + 3.0f;

				int iRand = gEngfuncs.pfnRandomLong(0, 2);

				switch (iRand)
				{
					case 0: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_NightBegin_1")); break;
					case 1: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_NightBegin_2")); break;
					default: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_NightBegin_3")); break;
				}

				gEngfuncs.pfnClientCmd("spk sound/events/human_message.wav");
			}
			else
			{
				m_flDisplayEnd = gHUD.m_flTime + 3.0f;

				int iRand = gEngfuncs.pfnRandomLong(0, 2);

				switch (iRand)
				{
					case 0: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Zombi_NightBegin_1")); break;
					case 1: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Zombi_NightBegin_2")); break;
					default: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Zombi_NightBegin_3")); break;
				}

				gEngfuncs.pfnClientCmd("spk sound/events/zombie_message.wav");
				
				gHUD.m_ZB4.m_Icon.AddIcon(5, 2, 2);
				gHUD.m_ZB4.m_Icon.AddIcon(4, 9, 2);
			}
		}

		if(time == m_iNightEnd - 1)
		{
			if(m_iTeam == 0)
			{
				gHUD.m_ZB4.m_Icon.AddIcon(5, 3, 3);

			}
			else
			{
				gHUD.m_ZB4.m_Icon.AddIcon(5, 2, 3);
				gHUD.m_ZB4.m_Icon.AddIcon(4, 9, 3);
			}
		}

		if(time == m_iNightEnd)
		{
			if(m_iTeam == 0)
			{
				gHUD.m_ZB4.m_Icon.AddIcon(5, 3, 3);				

				m_flDisplayEnd = gHUD.m_flTime + 3.0f;

				int iRand = gEngfuncs.pfnRandomLong(0, 2);

				switch (iRand)
				{
					case 0: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_NightEnd_1")); break;
					case 1: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_NightEnd_2")); break;
					default: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_NightEnd_3")); break;
				}

				gEngfuncs.pfnClientCmd("spk sound/events/human_message.wav");
			}
			else
			{
				gHUD.m_ZB4.m_Icon.AddIcon(5, 2, 3);
				gHUD.m_ZB4.m_Icon.AddIcon(4, 9, 3);
				gHUD.m_ZB4.m_Icon.AddIcon(7, 5, 1);
				
				m_flDisplayEnd = gHUD.m_flTime + 3.0f;

				int iRand = gEngfuncs.pfnRandomLong(0, 2);

				switch (iRand)
				{
					case 0: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Zombi_NightEnd_1")); break;
					case 1: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Zombi_NightEnd_2")); break;
					default: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Zombi_NightEnd_3")); break;
				}

				gEngfuncs.pfnClientCmd("spk sound/events/zombie_message.wav");
			}
		}

		if(time == m_iNightEnd - 4 && m_iTeam == 1)
		{
			gHUD.m_ZB4.m_Icon.AddIcon(7, 5, 3);
		}

		if(time == 175)
		{
			m_flDisplayEnd = gHUD.m_flTime + 3.0f;

			int iRand = gEngfuncs.pfnRandomLong(0, 2);

			switch (iRand)
			{
				case 0: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart5_1")); break;
				case 1: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart5_2")); break;
				default: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart5_3")); break;
			}

			gEngfuncs.pfnClientCmd("spk sound/events/human_message.wav");
		}

		if(time == 158 && m_iTeam == 0)
		{
			m_flDisplayEnd = gHUD.m_flTime + 3.0f;

			int iRand = gEngfuncs.pfnRandomLong(0, 2);

			if(m_iTeam == 0)
			{
				switch (iRand)
				{
					case 0: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart22_1")); break;
					case 1: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart22_2")); break;
					default: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Human_RoundStart22_3")); break;
				}
				gEngfuncs.pfnClientCmd("spk sound/events/human_message.wav");
			}
		}

		if(time == 158 && m_iTeam == 1)
		{
			m_flDisplayEnd = gHUD.m_flTime + 3.0f;

			int iRand = gEngfuncs.pfnRandomLong(0, 2);

			if(m_iTeam == 1)
			{
				switch (iRand)
				{
					case 0: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Zombi_FirstZombi_1")); break;
					case 1: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Zombi_FirstZombi_2")); break;
					default: wcscpy(m_szText,GetLangUni("CSBTE_ZB4_MSG_Zombi_FirstZombi_3")); break;
				}
				gEngfuncs.pfnClientCmd("spk sound/events/zombie_message.wav");
			}
			
		}


		if(time > 160)
		{
			char count[4];
			sprintf(count, "%d", time - 160);

			MSG_BeginWrite("TextMsg");
			MSG_WriteByte(4);
			MSG_WriteString("#CSBTE_ZombiSelectCount");
			MSG_WriteString(count);
			MSG_EndWrite();

			if(time <= 171)
			{
				switch (time - 161)
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

		if(time > m_iNightStart)
		{
			if(m_iTeam == 1)
				gHUD.m_ZB4.m_Icon.AddIcon(0, 0, 0);
		}

		m_iCount = time;
	}
	

	

	if(m_flDisplayEnd > gHUD.m_flTime)
	{
		/*TEXD_SetRenderMode(kRenderTransColor);
		vgui::surface()->DrawSetColor(255, 255, 255, 255);
		DrawTexture(m_iBg[m_iTeam], m_rcBg[m_iTeam], iX, iY);
		*/
		int r, g, b, a;

		a = 255;

		if(m_flDisplayEnd - gHUD.m_flTime > 2.6)
			a = abs((m_flDisplayEnd - gHUD.m_flTime - 3.0f) * 255 / 0.4);

		if(a > 255) a = 255;

		if(m_flDisplayEnd - gHUD.m_flTime < 0.5)
			a = (m_flDisplayEnd - gHUD.m_flTime) * 255 * 2;

		
		vgui::surface()->DrawSetColor(255, 255, 255, a);

		DrawTexture(m_iBg[m_iTeam], m_rcBg[m_iTeam], iX, iY);

		/*Tri_Enable(GL_TEXTURE_2D);
		Tri_BindTexture(GL_TEXTURE_2D, m_iBg[m_iTeam]);
		
		glColor4ub(255, 255, 255, a);
	
		Tri_Enable(GL_BLEND);
		Tri_Enable(GL_ALPHA_TEST);
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		Tri_AlphaFunc(GL_GREATER, 0.0);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		
		gEngfuncs.pTriAPI->TexCoord2f(0,1);
		gEngfuncs.pTriAPI->Vertex3f(iX,iY+m_rcBg[m_iTeam].bottom,0);

		gEngfuncs.pTriAPI->TexCoord2f(1,1);
		gEngfuncs.pTriAPI->Vertex3f(iX+m_rcBg[m_iTeam].right,iY+m_rcBg[m_iTeam].bottom,0);
		
		gEngfuncs.pTriAPI->TexCoord2f(1,0);
		gEngfuncs.pTriAPI->Vertex3f(iX+m_rcBg[m_iTeam].right,iY,0);
		
		gEngfuncs.pTriAPI->TexCoord2f(0,0);
		gEngfuncs.pTriAPI->Vertex3f(iX,iY,0);
		gEngfuncs.pTriAPI->End();*/

		int a2;
		
		GetFontColor(m_iTeam, r, g, b, a2);
				
		g_Font.SetWidth(22);
		g_Font.SetColor(r, g, b, int(a * (a2 / 255.0f)));

		iX += 127;
		iY += 80;

		g_Font.DrawString(m_szText, iX, iY, 1000);
	}
}

void CHudZB4UITip::SetTimeEnd(float flTime)
{
	if(flTime < 60)
		return;
	
	m_flTimeStart = gHUD.m_flTime;
	m_flTimeEnd = gHUD.m_flTime + flTime;
}

void CHudZB4UITip::ZombieIconsCheck(float flTime)
{
	int time = m_flTimeEnd - flTime + 1;

	if(time > m_iNightEnd && time <= m_iNightStart)
	{
		gHUD.m_ZB4.m_Icon.AddIcon(5, 2, 2);
		gHUD.m_ZB4.m_Icon.AddIcon(4, 9, 2);
	}
}

void CHudZB4UITip::LoadText(char *text)
{
	wchar_t text2[128];
	
	wcscpy(text2,GetLangUni(text));
	g_Font.LoadStr(text2);
}

void CHudZB4UITip::GetFontColor(int team, int &r, int &g, int &b, int &a)
{
	if(m_iTeam)
	{
		r = 180;
		g = 80;
		b = 75;
		a = 200;
	}
	else
	{
		r = 146;
		g = 158;
		b = 185;
		a = 200;
	}
}