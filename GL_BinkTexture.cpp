#include <metahook.h>
#include "GL_BinkTexture.h"
#include "plugins.h"
#include <vgui_controls\Controls.h>
#include "gl/gl.h"
#include "pbo.h"
#include "bink/bink.h"
#include "util.h"


CGL_BinkTexture::CGL_BinkTexture(const char *szPath)
{
	Reset();
	char fullPath[256];
	if (!g_pFileSystem->GetLocalPath(szPath, fullPath, sizeof(fullPath)))
	{
		LogToFile("LoadBink() : Fail Load %s (File not found)", szPath);
		return;
	}
	BinkSoundUseWaveOut();
	m_hBink = BinkOpen(fullPath, BINKSNDTRACK | BINKSURFACE32RA);
	if (!m_hBink)
	{
		LogToFile("LoadBink() : Fail Load %s (NOT a bink file)", szPath);
		m_hBink = nullptr;
		return;
	}

	int BufferSize = 4 * m_hBink->Width * m_hBink->Height;
	m_iBinkTexture = vgui::surface()->CreateNewTextureID();
	BinkSetSoundOnOff(m_hBink, true);
	
	glBindTexture(GL_TEXTURE_2D, m_iBinkTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_hBink->Width, m_hBink->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenBuffersARB(2, m_iPboIds);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_iPboIds[0]);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, BufferSize, nullptr, GL_STREAM_DRAW_ARB);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_iPboIds[1]);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, BufferSize, nullptr, GL_STREAM_DRAW_ARB);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	
}

CGL_BinkTexture::~CGL_BinkTexture()
{
	if (m_hBink)
	{
		BinkClose(m_hBink);
	}
}

void CGL_BinkTexture::UpdateFrame()
{
	if (!m_hBink)
		return;

	BinkDoFrame(m_hBink);
	glBindTexture(GL_TEXTURE_2D, m_iBinkTexture);
	
	if (BinkWait(m_hBink))
		return;
	
	while (BinkShouldSkip(m_hBink))
	{
		BinkNextFrame(m_hBink);
		BinkDoFrame(m_hBink);
	}
	if (m_hBink->FrameNum > m_hBink->Frames)
	{
		BinkGoto(m_hBink, 0, 0);
	}
	static int index = 0;
	index ^= 1;
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_iPboIds[!index]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_hBink->Width, m_hBink->Height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_iPboIds[index]);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_hBink->Width * 4 * m_hBink->Height, nullptr, GL_STREAM_DRAW_ARB);    // flush data
	if ((void *&)m_pBinkBuffer = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE_ARB))
	{
		BinkCopyToBuffer(m_hBink, m_pBinkBuffer, m_hBink->Width * 4, m_hBink->Height, 0, 0, BINKSURFACE32RA | BINKCOPYALL);
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
	}
	BinkNextFrame(m_hBink);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	
}

void CGL_BinkTexture::Reset()
{
	m_hBink = nullptr;
	m_pBinkBuffer = nullptr;
	m_iBinkTexture = NULL;
	m_iPboIds[0] = m_iPboIds[1] = NULL;
}

void CGL_BinkTexture::Draw(int x, int y, int w, int h)
{
	if (!m_hBink)
		return;
	UpdateFrame();
	glBindTexture(GL_TEXTURE_2D, m_iBinkTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(x, y, 0);//ul
	glTexCoord2f(1, 0); glVertex3f(x + w, y, 0);//ru
	glTexCoord2f(1, 1); glVertex3f(x + w, y + h, 0);//lr
	glTexCoord2f(0, 1); glVertex3f(x, y + h, 0);//ll
	glEnd();
}