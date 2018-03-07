#pragma once

#include "bink/bink.h"

class CBinkPlayer
{
public:
	CBinkPlayer()
	{
		m_p32PixelBuffer = NULL;
	}

	~CBinkPlayer()
	{
		if (m_p32PixelBuffer)
			delete[] m_p32PixelBuffer;
	}
	void Init(char *filename);
	void VidInit() { Stop(); };
	void Play(bool bLoop, float flTime);
	void Stop();
	void Paint(float flTime);
	void SetSize(int iWidth, int iHeight)
	{
		m_iWidth = iWidth;
		m_iHeight = iHeight;
	}
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	int GetOriginalWidth() { return (int)m_hBink->Width; }
	int GetOriginalHeight() { return (int)m_hBink->Height; }
	void SetPos(int iX, int iY)
	{
		m_iX = iX;
		m_iY = iY;
	}
	void SetColor(int r, int g, int b, int a)
	{
		m_R = r;
		m_G = g;
		m_B = b;
		m_iAlpha = a;
	}

private:
	HBINK m_hBink;

	byte *m_p24PixelBuffer;
	byte *m_p32PixelBuffer;

	int m_iTextureId;

	float m_flLastPaintTime;

	unsigned m_iLastFrame;
	unsigned m_iCurrentFrame;

	bool m_bPlaying;
	bool m_bLoop;

	int m_iWidth;
	int m_iHeight;

	int m_R;
	int m_G;
	int m_B;
	int m_iAlpha;

	int m_iX;
	int m_iY;

	float m_flEndTime;

};