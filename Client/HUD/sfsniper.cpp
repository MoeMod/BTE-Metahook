#include "metahook.h"
#include <cvardef.h>

#include "engfuncs.h"
#include "exportfuncs.h"
#include "plugins.h"
#include "BaseUI.h"
#include "hud.h"
#include "message.h"
#include "bink/bink.h"

#include <gl/gl.h>
#include "TriAPI.h"
#include <triangleapi.h>

#include "sfsniper.h"

static CHudSfsniperScope g_HudSfsniperScope;
CHudSfsniperScope &HudSfsniperScope()
{
	return g_HudSfsniperScope;
}

void CHudSfsniperScope::Init(void)
{
	m_pSniperScopeBinkMem = NULL;
}

CHudSfsniperScope::~CHudSfsniperScope()
{
	if (m_pSniperScopeBinkMem)
		free(m_pSniperScopeBinkMem);
	m_pSniperScopeBinkMem = NULL;
}

void CHudSfsniperScope::VidInit(void)
{
	m_iSniperScopeBinkTextureIndex = vgui::surface()->CreateNewTextureID();
	m_hSniperScopeBinkHandle = BinkOpen("cstrike\\sprites\\sfscope.bik", 135266304);
	m_flSniperScopePlayStartTime = 0.0;
	m_pSniperScopeBinkMem = (BYTE *)malloc(m_hSniperScopeBinkHandle->Width * m_hSniperScopeBinkHandle->Height * 4);
}

void CHudSfsniperScope::Run(void)
{
	if (m_flSniperScopePlayStartTime || !m_hSniperScopeBinkHandle)
		return;
	m_flSniperScopePlayStartTime = Hud().m_flTime;
	m_iScopeFrame = 1;
}

void CHudSfsniperScope::End(void)
{
	m_flSniperScopePlayStartTime = 0.0;
}

void CHudSfsniperScope::Draw(float flTime)
{
	if (!m_flSniperScopePlayStartTime || !m_hSniperScopeBinkHandle)
		return;

	if (Hud().m_flTime - m_flSniperScopePlayStartTime > (float(m_hSniperScopeBinkHandle->FrameRateDiv) / float(m_hSniperScopeBinkHandle->FrameRate)))
	{
		if (g_BinkScopeColor)
		{
			m_iScopeFrame++;
		}
		m_flSniperScopePlayStartTime = Hud().m_flTime;
		if (m_iScopeFrame > m_hSniperScopeBinkHandle->Frames)
		{
			m_iScopeFrame = 1;
		}
		BinkGoto(m_hSniperScopeBinkHandle, m_iScopeFrame, 0);
		BinkDoFrame(m_hSniperScopeBinkHandle);
		BinkCopyToBuffer(m_hSniperScopeBinkHandle, m_pSniperScopeBinkMem, m_hSniperScopeBinkHandle->Width * 4, m_hSniperScopeBinkHandle->Height, 0, 0, BINKSURFACE32RA);
		vgui::surface()->DrawSetTextureRGBA(m_iSniperScopeBinkTextureIndex, m_pSniperScopeBinkMem, m_hSniperScopeBinkHandle->Width, m_hSniperScopeBinkHandle->Height, 1, true);
	}

	float LENGTH_SCOPE = ScreenHeight / 2.2925373134328358208955223880597;

	int iX, iY, iWidth, iHeight;
	iWidth = int(m_hSniperScopeBinkHandle->Width * 6 / 5 * (ScreenWidth / 1024.0));
	iHeight = int(m_hSniperScopeBinkHandle->Height * 6 / 5 * (ScreenHeight / 768.0));

	if (iWidth < iHeight)
		iHeight = iWidth;
	else if (iHeight < iWidth)
		iWidth = iHeight;

	iX = ScreenWidth / 2 - iWidth / 2;
	iY = ScreenHeight / 2 - iHeight / 2;

	if (!g_BinkScopeColor)
	{
		m_iSniperScopeBinkColor[0] = 171;
		m_iSniperScopeBinkColor[1] = 89;
		m_iSniperScopeBinkColor[2] = 78;

#define LENGTH_SCOPE_TARGET	120
#define LENGTH_SCOPE_BORDER	75

		Tri_Enable(GL_TEXTURE_2D);
		Tri_Enable(GL_BLEND);
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Tri_BindTexture(GL_TEXTURE_2D, g_MHTga[g_tga_Scope_Target].texid);

		if (g_iVideoMode)
		{
			glColor4ub(255, 255, 255, 255);
		}
		else gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE_TARGET, ScreenHeight / 2 - LENGTH_SCOPE_BORDER, 0);

		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE_BORDER, ScreenHeight / 2 - LENGTH_SCOPE_BORDER, 0);

		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE_BORDER, ScreenHeight / 2 - LENGTH_SCOPE_TARGET, 0);

		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE_TARGET, ScreenHeight / 2 - LENGTH_SCOPE_TARGET, 0);
		gEngfuncs.pTriAPI->End();
		// 2
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE_TARGET, ScreenHeight / 2 - LENGTH_SCOPE_BORDER, 0);

		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE_BORDER, ScreenHeight / 2 - LENGTH_SCOPE_BORDER, 0);

		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE_BORDER, ScreenHeight / 2 - LENGTH_SCOPE_TARGET, 0);

		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE_TARGET, ScreenHeight / 2 - LENGTH_SCOPE_TARGET, 0);
		gEngfuncs.pTriAPI->End();
		// 3
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE_TARGET, ScreenHeight / 2 + LENGTH_SCOPE_BORDER, 0);

		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE_BORDER, ScreenHeight / 2 + LENGTH_SCOPE_BORDER, 0);

		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE_BORDER, ScreenHeight / 2 + LENGTH_SCOPE_TARGET, 0);

		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE_TARGET, ScreenHeight / 2 + LENGTH_SCOPE_TARGET, 0);
		gEngfuncs.pTriAPI->End();
		// 4
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE_BORDER, ScreenHeight / 2 + LENGTH_SCOPE_TARGET, 0);

		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE_BORDER, ScreenHeight / 2 + LENGTH_SCOPE_BORDER, 0);

		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE_TARGET, ScreenHeight / 2 + LENGTH_SCOPE_BORDER, 0);

		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE_TARGET, ScreenHeight / 2 + LENGTH_SCOPE_TARGET, 0);
		gEngfuncs.pTriAPI->End();
	}
	else
	{
		//m_iSniperScopeBinkColor[0] = 84;
		//m_iSniperScopeBinkColor[1] = 135;
		//m_iSniperScopeBinkColor[2] = 130;
		m_iSniperScopeBinkColor[0] = 96;
		m_iSniperScopeBinkColor[1] = 195;
		m_iSniperScopeBinkColor[2] = 190;
	}

	Tri_Enable(GL_TEXTURE_2D);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	Tri_BindTexture(GL_TEXTURE_2D, g_MHTga[g_tga_Scope].texid);
	if (g_iVideoMode)
	{
		glColor4ub(255, 255, 255, 255);
	}
	else gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE, ScreenHeight / 2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2, ScreenHeight / 2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2, ScreenHeight / 2 - LENGTH_SCOPE, 0);

	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE, ScreenHeight / 2 - LENGTH_SCOPE, 0);
	gEngfuncs.pTriAPI->End();
	// 2
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE, ScreenHeight / 2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2, ScreenHeight / 2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2, ScreenHeight / 2 - LENGTH_SCOPE, 0);

	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE, ScreenHeight / 2 - LENGTH_SCOPE, 0);
	gEngfuncs.pTriAPI->End();
	// 3
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE, ScreenHeight / 2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2, ScreenHeight / 2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2, ScreenHeight / 2 + LENGTH_SCOPE, 0);

	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 - LENGTH_SCOPE, ScreenHeight / 2 + LENGTH_SCOPE, 0);
	gEngfuncs.pTriAPI->End();
	// 4
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2, ScreenHeight / 2 + LENGTH_SCOPE, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2, ScreenHeight / 2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE, ScreenHeight / 2, 0);

	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(ScreenWidth / 2 + LENGTH_SCOPE, ScreenHeight / 2 + LENGTH_SCOPE, 0);
	gEngfuncs.pTriAPI->End();

	Tri_Enable(GL_TEXTURE_2D);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vgui::surface()->DrawSetTexture(m_iSniperScopeBinkTextureIndex);
	vgui::surface()->DrawSetColor(m_iSniperScopeBinkColor[0], m_iSniperScopeBinkColor[1], m_iSniperScopeBinkColor[2], 255);
	vgui::surface()->DrawTexturedRect(iX, iY, iX + iWidth, iY + iHeight);

	gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth / 2 - LENGTH_SCOPE + 1, ScreenHeight, 0, 0, 0, 255);
	gEngfuncs.pfnFillRGBABlend(ScreenWidth / 2 + LENGTH_SCOPE - 1, 0, ScreenWidth / 2 - LENGTH_SCOPE + 10, ScreenHeight, 0, 0, 0, 255);
	gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE + 1, 0, 0, 0, 255);
	gEngfuncs.pfnFillRGBABlend(0, ScreenHeight / 2 + LENGTH_SCOPE, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE + 10, 0, 0, 0, 255);
}