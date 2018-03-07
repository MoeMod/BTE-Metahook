#include "base.h"
#include "BaseUI.h"
#include "BinkPlayer.h"

void CBinkPlayer::Init(char *filename)
{
	if(m_hBink)
		return;

	m_hBink = BinkOpen(filename, BINKNOTHREADEDIO | BINKALPHA);

	if(!m_hBink)
		return;

	if (m_p32PixelBuffer)
		free(m_p32PixelBuffer);
	
	m_p32PixelBuffer = new byte [m_hBink->Width * m_hBink->Height * 4];

	m_flLastPaintTime = 0;
	m_iLastFrame = -1;
	m_iCurrentFrame = 0;
	m_bPlaying = false;

	m_iWidth = m_hBink->Width;
	m_iHeight = m_hBink->Height;

	m_R = m_G = m_B = m_iAlpha = 255;
	m_iX = m_iY = 0;
}

void CBinkPlayer::Play(bool bLoop, float flTime)
{
	m_flLastPaintTime = 0;
	m_iLastFrame = -1;
	m_iCurrentFrame = 0;
	m_bPlaying = true;
	m_bLoop = bLoop;
	m_flEndTime = flTime;
}

void CBinkPlayer::Paint(float flTime)
{
	if(!m_bPlaying || !m_hBink) return;
	if(m_bLoop && m_flEndTime < flTime) return;
	
	if(flTime - m_flLastPaintTime > (float)m_hBink->FrameRateDiv / m_hBink->FrameRate)
	{ 
		m_iLastFrame = m_iCurrentFrame;
		m_iCurrentFrame++;

		m_flLastPaintTime = flTime;

		if (m_iCurrentFrame >= m_hBink->Frames)
		{
			if(!m_bLoop)
				Stop();
			else
				m_iCurrentFrame = 0;
		}
	}

	if (m_iCurrentFrame != m_iLastFrame)
	{
		BinkGoto(m_hBink, m_iCurrentFrame, 0);
		BinkDoFrame(m_hBink);

		BinkCopyToBuffer(m_hBink, m_p32PixelBuffer, m_hBink->Width * 4, m_hBink->Height, 0, 0, BINKSURFACE32RA);

		vgui::surface()->DrawSetTextureRGBA(m_iTextureId, m_p32PixelBuffer, m_hBink->Width, m_hBink->Height, true, false);
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vgui::surface()->DrawSetColor(m_R, m_G, m_B, m_iAlpha);
	vgui::surface()->DrawSetTexture(m_iTextureId);
	vgui::surface()->DrawTexturedRect(m_iX, m_iY, m_iWidth + m_iX, m_iHeight + m_iY);
}

void CBinkPlayer::Stop()
{
	m_flLastPaintTime = 0;
	m_iLastFrame = -1;
	m_iCurrentFrame = 0;
	m_bPlaying = false;
	m_bLoop = false;
}
