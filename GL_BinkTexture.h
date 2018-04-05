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
	void GetTextureCoords(float *coords);
	bool hasAlphaChannel();
	void setPause(bool state);
	void setFrame(unsigned int frame);
	void setLoop(bool state);
	HBINK GetBinkHandle()
	{
		return m_bikHeader;
	}
	GLuint GetTextureId()
	{
		return m_glTexture;
	}
	bool FPlaySound()
	{
		if (!m_bikHeader)
			return false;
		return m_bikHeader->soundon;
	}
	int w()
	{
		if (!m_bikHeader)
			return 0;
		return m_bikHeader->Width;
	}
	int h()
	{
		if (!m_bikHeader)
			return 0;
		return m_bikHeader->Height;
	}
	operator bool()
	{
		return m_bikHeader != nullptr;
	}
private:
	HBINK m_bikHeader;				// bink file header
	bool m_hasAlphaChannel;			// video has an alpha channel ?
	U32 m_bikWidth;					// width of video
	U32 m_bikHeight;				// height of video
	U32 m_bikPitch;					// byte size of one row of pixels
	U32 m_bikCopyFlags;				// use in BinkCopyToBuffer()
	GLuint m_glPixelBuffers[2];		// opengl pixel buffer object
	GLsizei m_glPixelBufferSize;	// byte size of pixel buffer
	GLuint m_glTexture;				// opengl texture object
	GLsizei m_glTextureWidth;		// width of opengl texture
	GLsizei m_glTextureHeight;		// height of opengl texture
	GLenum m_glTextureFormat;		// use in glTexImage/glTexSubImage
	GLenum m_glInternalTextureFormat;
	GLfloat m_glTextureCoords[4];	// { left, right, top, bottom }
	bool m_loop;
};