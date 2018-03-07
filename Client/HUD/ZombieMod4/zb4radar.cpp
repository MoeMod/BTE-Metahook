#include "base.h"
#include "BaseUI.h"
#include "configs.h"

void CHudZB4UIRadar::Init()
{
	m_iBackground = g_Texture[gHUD.m_TGA.FindTexture("resource\\hud\\zombie\\z4_radarbg")].iTexture;
	m_iCover = g_Texture[gHUD.m_TGA.FindTexture("resource\\hud\\zombie\\z4_radarline")].iTexture;
}

void CHudZB4UIRadar::Redraw()
{	
	if (gConfigs.bEnableNewHud)
		return;

	if(!g_bAlive || gHUD.m_FOV <= 40.0)
		return;
	
	vgui::surface()->DrawSetTexture(m_iCover);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vgui::surface()->DrawTexturedRect(0, 0, 201, 200);
}

void CHudZB4UIRadar::DrawBackground()
{
	if (gConfigs.bEnableNewHud)
		return;
	
	if(!g_bAlive || gHUD.m_FOV <= 40.0)
		return;

	vgui::surface()->DrawSetTexture(m_iBackground);
	TEXD_SetRenderMode(kRenderTransTexture);
	vgui::surface()->DrawSetColor(255, 255, 255, 150);
	vgui::surface()->DrawTexturedRect(0, 0, 201, 200);
}