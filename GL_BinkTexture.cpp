#include <metahook.h>
#include <tuple>
#include "GL_BinkTexture.h"
#include "plugins.h"
#include <vgui_controls\Controls.h>
#include "gl/gl.h"
#include "pbo.h"
#include "bink/bink.h"
#include "util.h"
#include "LoadTexture.h"
#include "Color.h"

static inline unsigned log(unsigned n)
{
	__asm bsr eax, n
}

static inline unsigned suggestTexSize(unsigned n)
{
	return (1 << (log(n) + !!(n & (n - 1))));
}

CGL_BinkTexture::CGL_BinkTexture(const char *fileName)
{
	char fullPath[256];
	if (!g_pFileSystem->GetLocalPath(fileName, fullPath, sizeof(fullPath)))
	{
		LogToFile("LoadBink() : Fail Load %s (File not found)", fileName);
		m_bikHeader = nullptr;
		return;
	}

	static bool g_bink_initialized = false;
	if (!g_bink_initialized) {
		g_bink_initialized = true;
		BinkSoundUseWaveOut();
	}
	m_bikHeader = BinkOpen(fullPath, BINKALPHA | BINKSNDTRACK);
	if (m_bikHeader == nullptr) {
		return;
	}
	//BinkPause(m_bikHeader, true);
	m_hasAlphaChannel = (m_bikHeader->BinkType & BINKALPHA) ? true : false;
	m_bikWidth = m_bikHeader->Width;
	m_bikHeight = m_bikHeader->Height;
	m_bikPitch = m_bikWidth * (m_hasAlphaChannel ? 4 : 3);
	m_bikCopyFlags = BINKCOPYALL | (m_hasAlphaChannel ? BINKSURFACE32RA : BINKSURFACE24R);
	// create pixel buffer
	m_glPixelBufferSize = m_bikPitch * m_bikHeight;
	glGenBuffersARB(2, m_glPixelBuffers);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBuffers[0]);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBufferSize, nullptr, GL_STREAM_DRAW_ARB);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBuffers[1]);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBufferSize, nullptr, GL_STREAM_DRAW_ARB);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	// create texture
	m_glTextureFormat = (m_hasAlphaChannel ? GL_RGBA : GL_RGB);
	m_glInternalTextureFormat = (m_hasAlphaChannel ? GL_RGBA : GL_RGB);
	m_glTextureWidth = suggestTexSize(m_bikWidth);
	m_glTextureHeight = suggestTexSize(m_bikHeight);
	//glGenTextures(1, &m_glTexture);
	m_glTexture = vgui::surface()->CreateNewTextureID();
	glBindTexture(GL_TEXTURE_2D, m_glTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, m_glInternalTextureFormat, m_glTextureWidth, m_glTextureHeight, 0, m_glTextureFormat, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
	// calculate texture coords
	m_glTextureCoords[0] = 0.0f;
	m_glTextureCoords[1] = (GLfloat)m_bikWidth / (GLfloat)m_glTextureWidth;
	m_glTextureCoords[2] = 0.0f;
	m_glTextureCoords[3] = (GLfloat)m_bikHeight / (GLfloat)m_glTextureHeight;
	// player control
	m_loop = true;

}

CGL_BinkTexture::~CGL_BinkTexture()
{
	if (m_bikHeader) {
		BinkClose(m_bikHeader);
		m_bikHeader = nullptr;
	}
	m_hasAlphaChannel = false;
	m_bikWidth = 0;
	m_bikHeight = 0;
	m_bikPitch = 0;
	m_bikCopyFlags = 0;
	for (int i = 0; i < 2; ++i) {
		if (m_glPixelBuffers[i]) {
			glDeleteBuffersARB(1, &m_glPixelBuffers[i]);
			m_glPixelBuffers[i] = 0;
		}
	}
	m_glPixelBufferSize = 0;
	if (m_glTexture) {
		glDeleteTextures(1, &m_glTexture);
		m_glTexture = 0;
	}
	m_glTextureWidth = 0;
	m_glTextureHeight = 0;
	m_glTextureFormat = 0;
}

void CGL_BinkTexture::UpdateFrame()
{
	if (m_bikHeader == nullptr) {
		return;
	}
	BinkDoFrame(m_bikHeader);
	glBindTexture(GL_TEXTURE_2D, GetTextureId());
	if (BinkWait(m_bikHeader)) {
		return;
	}
	while (BinkShouldSkip(m_bikHeader)) {
		BinkNextFrame(m_bikHeader);
		BinkDoFrame(m_bikHeader);
	}
	if (m_bikHeader->FrameNum >= m_bikHeader->Frames) {
		if (m_loop) {
			BinkGoto(m_bikHeader, 0, 0);
		}
	}
	// swap pixel buffer
	static int index = 0;
	index ^= 1;
	// copy pixel from front buffer to texture
	glBindTexture(GL_TEXTURE_2D, m_glTexture);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBuffers[!index]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_bikWidth, m_bikHeight, m_glTextureFormat, GL_UNSIGNED_BYTE, nullptr);
	// copy pixel from bink to back buffer
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBuffers[index]);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBufferSize, nullptr, GL_STREAM_DRAW_ARB);
	void *ptr = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE_ARB);
	if (ptr) {
		BinkCopyToBuffer(m_bikHeader, ptr, m_bikPitch, m_bikHeight, 0, 0, m_bikCopyFlags);
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
	}
	BinkNextFrame(m_bikHeader);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

void CGL_BinkTexture::Draw(int x, int y, int w, int h)
{
	if (!m_bikHeader)
		return;
	UpdateFrame();

	GLfloat coords[4];
	GetTextureCoords(coords);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(255, 255, 255, 255);
	glBindTexture(GL_TEXTURE_2D, GetTextureId());
	vgui::surface()->DrawSetColor(Color(255, 255, 255, 255));
	vgui::surface()->DrawSetTexture(GetTextureId());

	glBegin(GL_QUADS);
	glTexCoord2f(coords[0], coords[2]); glVertex3f(x, y, 0);//ul
	glTexCoord2f(coords[1], coords[2]); glVertex3f(x + w, y, 0);//ru
	glTexCoord2f(coords[1], coords[3]); glVertex3f(x + w, y + h, 0);//lr
	glTexCoord2f(coords[0], coords[3]); glVertex3f(x, y + h, 0);//ll
	glEnd();
}

void CGL_BinkTexture::GetTextureCoords(float *coords)
{
	if (coords) {
		for (int i = 0; i < 4; ++i) {
			coords[i] = m_glTextureCoords[i];
		}
	}
}

bool CGL_BinkTexture::hasAlphaChannel()
{
	return m_hasAlphaChannel;
}

void CGL_BinkTexture::setPause(bool state)
{
	if (m_bikHeader) {
		BinkPause(m_bikHeader, state ? 1 : 0);
	}
}

void CGL_BinkTexture::setFrame(unsigned int frame)
{
	if (m_bikHeader) {
		BinkGoto(m_bikHeader, frame, 0);
	}
}

void CGL_BinkTexture::setLoop(bool state)
{
	m_loop = state;
}