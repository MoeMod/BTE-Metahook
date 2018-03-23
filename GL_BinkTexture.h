#pragma once
#include "bink/bink.h"
#include "gl/gl.h"

class CGL_BinkTexture
{
public:
	CGL_BinkTexture(const char *szPath);
	~CGL_BinkTexture();
	void Draw(int x, int y, int w = 0, int h = 0);
	void UpdateFrame();
	HBINK GetBinkHandle()
	{
		return m_hBink;
	}
	GLuint GetTextureId()
	{
		return m_iBinkTexture;
	}
	byte *GetBuffer()
	{
		return m_pBinkBuffer;
	}
	bool FPlaySound()
	{
		if (!m_hBink)
			return false;
		return m_hBink->soundon;
	}
	int w()
	{
		if (!m_hBink)
			return 0;
		return m_hBink->Width;
	}
	int h()
	{
		if (!m_hBink)
			return 0;
		return m_hBink->Height;
	}
	operator bool()
	{
		return m_hBink != nullptr;
	}
private:
	void Reset();
	HBINK m_hBink;
	bool m_bHasAlpha;
	size_t m_iBufferSize;
	byte *m_pBinkBuffer;
	GLuint m_iBinkTexture;
	GLuint m_iPboIds[2];
};