#include "hud.h"
#include "util.h"
#include "drawimage.h"

static CHudSPRElements g_HudSPRElements;

CHudSPRElements &HudSPRElements()
{
	return g_HudSPRElements;
}

void CHudSPRElements::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}

void CHudSPRElements::VidInit(void)
{
	memset(m_rgDrawImage, 0, sizeof(m_rgDrawImage));
}

void CHudSPRElements::AddElement(const DrawImageItem &rgTempDrawImage)
{
	if (rgTempDrawImage.iChanne <= 0 || rgTempDrawImage.iChanne >= MAX_DRAWIMAGE)
	{
		for (int i = 0; i < MAX_DRAWIMAGE; i++)
		{
			if (!m_rgDrawImage[i].flEndDisplayTime)
			{
				m_rgDrawImage[i] = rgTempDrawImage;
				break;
			}
		}
	}
	else
		m_rgDrawImage[rgTempDrawImage.iChanne] = rgTempDrawImage;
}

void CHudSPRElements::Draw(float time)
{
	for (int i = 0; i < MAX_DRAWIMAGE; i++)
	{
		if (!m_rgDrawImage[i].flEndDisplayTime)
			continue;

		if (time >= m_rgDrawImage[i].flEndDisplayTime)
		{
			m_rgDrawImage[i].flEndDisplayTime = 0.0;
			continue;
		}

		int r, g, b, a;
		r = m_rgDrawImage[i].color.r;
		g = m_rgDrawImage[i].color.g;
		b = m_rgDrawImage[i].color.b;

		switch (m_rgDrawImage[i].iMode)
		{
			case 1:
			{
				float flHasDisplayTime = time - m_rgDrawImage[i].flStartDisplayTime;
				float flNeedDisplayTime = m_rgDrawImage[i].flEndDisplayTime - m_rgDrawImage[i].flStartDisplayTime;
				a = int(flHasDisplayTime / flNeedDisplayTime * 255);
				ScaleColors(r, g, b, a);
				break;
			}
			case 2:
			{
				float flHasDisplayTime = time - m_rgDrawImage[i].flStartDisplayTime;
				float flNeedDisplayTime = m_rgDrawImage[i].flEndDisplayTime - m_rgDrawImage[i].flStartDisplayTime;
				a = 255 - int(flHasDisplayTime / flNeedDisplayTime * 255);
				ScaleColors(r, g, b, a);
				break;
			}
			case 3:
			{
				float flHasDisplayTime = time - m_rgDrawImage[i].flStartDisplayTime;
				float flNeedDisplayTime = m_rgDrawImage[i].flEndDisplayTime - m_rgDrawImage[i].flStartDisplayTime;
				a = int(flHasDisplayTime / flNeedDisplayTime * 510);
				if (a > 255)
					a = 255 - (a - 255);
				ScaleColors(r, g, b, a);
				break;
			}
		}
		
		if (m_rgDrawImage[i].iCheck>0)
		{
			m_rgDrawImage[i].hl_hSprites = Hud().GetSprite(m_rgDrawImage[i].iSprIndex);
		}
		gEngfuncs.pfnSPR_Set(m_rgDrawImage[i].hl_hSprites, r, g, b);
		
		if (!m_rgDrawImage[i].iCenter)
		{
			switch (m_rgDrawImage[i].iFunction)
			{
				case 0:
				{
					if(m_rgDrawImage[i].iCheck>0)
					{
						if(m_rgDrawImage[i].iLength>-1)
						{
							wrect_t *TempRect;
							TempRect = &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex);
							TempRect->right = m_rgDrawImage[i].iLength;
							gEngfuncs.pfnSPR_Draw(0, m_rgDrawImage[i].x, m_rgDrawImage[i].y,TempRect);
						}
						else
						{
							gEngfuncs.pfnSPR_Draw(0, m_rgDrawImage[i].x, m_rgDrawImage[i].y,&Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex));
						}
					}
					else
					{
						gEngfuncs.pfnSPR_Draw(0, m_rgDrawImage[i].x, m_rgDrawImage[i].y, NULL);
					}
					break;
				}
				case 1:
				{
					if(m_rgDrawImage[i].iCheck>0)
					{
						if(m_rgDrawImage[i].iLength>-1)
						{
							wrect_t *TempRect;
							TempRect = &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex);
							TempRect->right = m_rgDrawImage[i].iLength;
							gEngfuncs.pfnSPR_DrawAdditive(0, m_rgDrawImage[i].x, m_rgDrawImage[i].y, TempRect);
						}
						else
						{
							gEngfuncs.pfnSPR_DrawAdditive(0, m_rgDrawImage[i].x, m_rgDrawImage[i].y, &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex));
						}
					}
					else
					{
						gEngfuncs.pfnSPR_DrawAdditive(0, m_rgDrawImage[i].x, m_rgDrawImage[i].y, NULL);
					}
					break;
				}
				case 2:
				{
					if(m_rgDrawImage[i].iCheck>0)
					{
						if(m_rgDrawImage[i].iLength>-1)
						{
							wrect_t *TempRect;
							TempRect = &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex);
							TempRect->right = m_rgDrawImage[i].iLength;
							gEngfuncs.pfnSPR_DrawHoles(0, m_rgDrawImage[i].x, m_rgDrawImage[i].y, TempRect);
						}
						else
						{
							gEngfuncs.pfnSPR_DrawHoles(0, m_rgDrawImage[i].x, m_rgDrawImage[i].y, &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex));
						}
					}
					else
					{
						gEngfuncs.pfnSPR_DrawHoles(0, m_rgDrawImage[i].x, m_rgDrawImage[i].y, NULL);
					}
					break;
				}
			}
		}
		else
		{
			switch (m_rgDrawImage[i].iFunction)
			{
				case 0:
				{
					if(m_rgDrawImage[i].iCheck>0)
					{
						if(m_rgDrawImage[i].iLength>-1)
						{
							wrect_t *TempRect;
							TempRect = &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex);
							TempRect->right = m_rgDrawImage[i].iLength;
							gEngfuncs.pfnSPR_Draw(0, m_rgDrawImage[i].x - gEngfuncs.pfnSPR_Width(m_rgDrawImage[i].hl_hSprites, 0) / 2, m_rgDrawImage[i].y - gEngfuncs.pfnSPR_Height(m_rgDrawImage[i].hl_hSprites, 0) / 2, TempRect);
						}
						else
						{
							gEngfuncs.pfnSPR_Draw(0, m_rgDrawImage[i].x - gEngfuncs.pfnSPR_Width(m_rgDrawImage[i].hl_hSprites, 0) / 2, m_rgDrawImage[i].y - gEngfuncs.pfnSPR_Height(m_rgDrawImage[i].hl_hSprites, 0) / 2, &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex));
						}
					}
					else
					{
						gEngfuncs.pfnSPR_Draw(0, m_rgDrawImage[i].x - gEngfuncs.pfnSPR_Width(m_rgDrawImage[i].hl_hSprites, 0) / 2, m_rgDrawImage[i].y - gEngfuncs.pfnSPR_Height(m_rgDrawImage[i].hl_hSprites, 0) / 2, NULL);
					}
					break;
				}
				case 1:
				{
					if(m_rgDrawImage[i].iCheck>0)
					{
						if(m_rgDrawImage[i].iLength>-1)
						{
							wrect_t *TempRect;
							TempRect = &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex);
							TempRect->right = m_rgDrawImage[i].iLength;
							gEngfuncs.pfnSPR_DrawAdditive(0, m_rgDrawImage[i].x - gEngfuncs.pfnSPR_Width(m_rgDrawImage[i].hl_hSprites, 0) / 2, m_rgDrawImage[i].y - gEngfuncs.pfnSPR_Height(m_rgDrawImage[i].hl_hSprites, 0) / 2, TempRect);
						}
						else
						{
							gEngfuncs.pfnSPR_DrawAdditive(0, m_rgDrawImage[i].x - gEngfuncs.pfnSPR_Width(m_rgDrawImage[i].hl_hSprites, 0) / 2, m_rgDrawImage[i].y - gEngfuncs.pfnSPR_Height(m_rgDrawImage[i].hl_hSprites, 0) / 2, &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex));
						}
					}
					else
					{
						gEngfuncs.pfnSPR_DrawAdditive(0, m_rgDrawImage[i].x - gEngfuncs.pfnSPR_Width(m_rgDrawImage[i].hl_hSprites, 0) / 2, m_rgDrawImage[i].y - gEngfuncs.pfnSPR_Height(m_rgDrawImage[i].hl_hSprites, 0) / 2, NULL);
					}
					break;
				}
				case 2:
				{
					if(m_rgDrawImage[i].iCheck>0)
					{
						if(m_rgDrawImage[i].iLength>-1)
						{
							wrect_t *TempRect;
							TempRect = &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex);
							TempRect->right = m_rgDrawImage[i].iLength;
							gEngfuncs.pfnSPR_DrawHoles(0, m_rgDrawImage[i].x - gEngfuncs.pfnSPR_Width(m_rgDrawImage[i].hl_hSprites, 0) / 2, m_rgDrawImage[i].y - gEngfuncs.pfnSPR_Height(m_rgDrawImage[i].hl_hSprites, 0) / 2, TempRect);
						}
						else
						{
							gEngfuncs.pfnSPR_DrawHoles(0, m_rgDrawImage[i].x - gEngfuncs.pfnSPR_Width(m_rgDrawImage[i].hl_hSprites, 0) / 2, m_rgDrawImage[i].y - gEngfuncs.pfnSPR_Height(m_rgDrawImage[i].hl_hSprites, 0) / 2, &Hud().GetSpriteRect(m_rgDrawImage[i].iSprIndex));
						}
					}
					else
					{
						gEngfuncs.pfnSPR_DrawHoles(0, m_rgDrawImage[i].x - gEngfuncs.pfnSPR_Width(m_rgDrawImage[i].hl_hSprites, 0) / 2, m_rgDrawImage[i].y - gEngfuncs.pfnSPR_Height(m_rgDrawImage[i].hl_hSprites, 0) / 2, NULL);
					}
					break;
				}
			}
		}
	}
}