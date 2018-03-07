#include <base.h>
#include <metahook.h>
#include <Interface\vgui\IEngineVGui.h>
#include <Interface\IEngineSurface.h>
#include <vgui_controls\Controls.h>
#include "qgl.h"
#include "gl/glext.h"
#include "TriAPI.h"
#include "shader.h"
#include "plugins.h"
#include "exportfuncs.h"
#include "util.h"

#include "LoadTGA.h"
#include "LoadBMP.h"
#include "LoadDDS.h"
#include "LoadDTX.h"
#include "FreeImage\FreeImage.h"

GLuint GL_LoadTexture(const char *szPath, int *pwidth, int *pheight)
{
	int iTexIndex = 0;
	unsigned iFileSize = g_pFileSystem->Size(szPath);
	const char *szExt = FileExtension(szPath);
	if (!szExt)
	{
		LogToFile("GL_LoadTexture() : Fail Load %s (bad path? should have extension.)", szPath);
		return 0;
	}

	static byte buffer[1024 * 1024 * 4];

	int w, h;
	if (!stricmp(szExt, "tga"))
	{
		if (!GetTGASize(szPath, &w, &h))
		{
			LogToFile("GL_LoadTexture() : Fail Load %s (Not found)", szPath);
			return 0;
		}

		byte *rgba = buffer;
		if (w >= 1024 || h >= 1024)
		{
			try
			{
				rgba = new byte[w*h * 4];
			}
			catch (...)
			{
				LogToFile("GL_LoadTexture() : Fail Load %s (bad alloc)", szPath);
				return 0;
			}
		}
		if (!LoadTGA(szPath, rgba, w * h * 4, &w, &h))
		{
			if (rgba != buffer)
				delete[] rgba;
			LogToFile("GL_LoadTexture() : Fail Load %s", szPath);
			return 0;
		}
		iTexIndex = vgui::surface()->CreateNewTextureID();
		qglBindTexture(GL_TEXTURE_2D, iTexIndex);

		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (g_iBPP == 32)
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_EXT, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		else
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1_EXT, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

		if (rgba != buffer)
			delete[] rgba;

		if (pwidth)
			*pwidth = w;
		if (pheight)
			*pheight = h;
		return iTexIndex;
	}
	else if (!stricmp(szExt, "bmp"))
	{
		if (!GetBMPSize(szPath, &w, &h))
		{
			LogToFile("GL_LoadTexture() : Fail Load %s", szPath);
			return iTexIndex;
		}

		byte *rgba = buffer;
		if (w >= 1024 || h >= 1024)
		{
			try
			{
				rgba = new byte[w*h * 4];
			}
			catch (...)
			{
				LogToFile("GL_LoadTexture() : Fail Load %s (bad alloc)", szPath);
				return iTexIndex;
			}
		}

		if (!LoadBMP(szPath, rgba, w * h * 4, &w, &h))
		{
			if (rgba != buffer)
				delete[] rgba;
			LogToFile("GL_LoadTexture() : Fail Load %s (Error Format)", szPath);
			return iTexIndex;
		}
		iTexIndex = vgui::surface()->CreateNewTextureID();
		qglBindTexture(GL_TEXTURE_2D, iTexIndex);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (g_iBPP == 32)
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_EXT, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		else
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1_EXT, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		if (rgba != buffer)
			delete[] rgba;

		if (pwidth)
			*pwidth = w;
		if (pheight)
			*pheight = h;
		return iTexIndex;
	}
	else if (!stricmp(szExt, "dds"))
	{
		int iSize = GetDDSSize(szPath, &w, &h);
		if (!iSize)
		{
			LogToFile("GL_LoadTexture() : Fail Load %s ", szPath);
			return iTexIndex;
		}

		byte *data = buffer;
		if (iSize * 4 > 1024 * 1024 * 4)
		{
			try
			{
				data = new byte[iSize * 4];
			}
			catch (...)
			{
				LogToFile("GL_LoadTexture() : Fail Load %s (bad alloc)", szPath);
				return iTexIndex;
			}
		}
		GLuint internalformat;
		if (!LoadDDS(szPath, data, iSize * 4, &w, &h, &internalformat))
		{
			if (data != buffer)
				delete[] data;
			LogToFile("GL_LoadTexture() : Fail Load %s (Error Format)", szPath);
			return iTexIndex;
		}
		static PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");

		iTexIndex = vgui::surface()->CreateNewTextureID();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, iTexIndex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, internalformat, w, h, 0, iSize, data);

		if (data != buffer)
			delete[] data;

		if (pwidth)
			*pwidth = w;
		if (pheight)
			*pheight = h;
		return iTexIndex;
	}
	else if (!stricmp(szExt, "dtx"))
	{
		if (!GetDTXSize(szPath, &w, &h))
		{
			LogToFile("GL_LoadTexture() : Fail Load %s ", szPath);
			return iTexIndex;
		}

		byte *rgba = buffer;
		if (w >= 1024 || h >= 1024)
		{
			try
			{
				rgba = new byte[w*h * 4];
			}
			catch (...)
			{
				LogToFile("GL_LoadTexture() : Fail Load %s (bad alloc)", szPath);
				return iTexIndex;
			}
		}

		int iFormat, iSize;
		if (!LoadDTX(szPath, buffer, sizeof(buffer), &w, &h, &iFormat, &iSize))
		{
			if (rgba != buffer)
				delete[] rgba;
			LogToFile("GL_LoadTexture() : Fail Load %s (Error Format)", szPath);
			return iTexIndex;
		}

		iTexIndex = vgui::surface()->CreateNewTextureID();
		Tri_Enable(GL_TEXTURE_2D);
		Tri_BindTexture(GL_TEXTURE_2D, iTexIndex);
		Tri_SetTextureParam();
		if (iFormat == GL_RGBA)
		{
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		}
		else if (iFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || iFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT || iFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
		{
			qglCompressedTexImage2D(GL_TEXTURE_2D, 0, iFormat, w, h, 0, iSize, rgba);
		}

		if (rgba != buffer)
			delete[] rgba;

		if (pwidth)
			*pwidth = w;
		if (pheight)
			*pheight = h;
		return iTexIndex;
	}
	else
	{
		do
		{
			char szFullPath[256];
			g_pFileSystem->GetLocalPath(szPath, szFullPath, sizeof(szFullPath) - 1);
			FREE_IMAGE_FORMAT FileFormat = FreeImage_GetFileType(szFullPath, 0);
			if (FileFormat == FIF_UNKNOWN)
			{
				break;
			}
			FIBITMAP *bitmap = FreeImage_Load(FileFormat, szFullPath, 0);
			FIBITMAP *pImage = FreeImage_ConvertTo32Bits(bitmap);
			FreeImage_FlipVertical(pImage);
			w = FreeImage_GetWidth(pImage);
			h = FreeImage_GetHeight(pImage);
			byte *bgra = FreeImage_GetBits(pImage);

			iTexIndex = vgui::surface()->CreateNewTextureID();
			qglBindTexture(GL_TEXTURE_2D, iTexIndex);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			if (g_iBPP == 32)
				qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_EXT, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bgra);
			else
				qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1_EXT, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bgra);

			FreeImage_Unload(pImage);
			FreeImage_Unload(bitmap);
			if (pwidth)
				*pwidth = w;
			if (pheight)
				*pheight = h;
			return iTexIndex;
		} while (0); // EASY TO JUMP OUT
	}
	LogToFile("GL_LoadTexture() : Fail Load %s (Unsupported format)", szPath);
	return 0;
}

void GL_DrawTexture(GLuint index, int r, int g, int b, int a, int iX, int iY, int iWidth, int iHeight)
{
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Tri_BindTexture(GL_TEXTURE_2D, index);
	if (g_iVideoMode)
		glColor4ub(r, g, b, a);
	else
		gEngfuncs.pTriAPI->Color4ub(r, g, b, a);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(iX, iY + iHeight, 0);
	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(iX + iWidth, iY + iHeight, 0);
	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(iX + iWidth, iY, 0);
	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(iX, iY, 0);
	gEngfuncs.pTriAPI->End();
}