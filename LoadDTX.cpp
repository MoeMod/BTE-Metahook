#include <metahook.h>
#include "LoadDTX.h"
#include "plugins.h"
#include "perf_counter.h"
#include "cvardef.h"
#include "gl/gl.h"
#include "gl/glext.h"

#pragma pack(1)

struct DtxHeader
{
	unsigned int iResType;
	int iVersion;
	unsigned short usWidth;
	unsigned short usHeight;
	unsigned short usMipmaps;
	unsigned short usSections;
	int iFlags;
	int iUserFlags;
	unsigned char ubExtra[12];
	char szCommandString[128];
};
#pragma pack()

bool LoadDTX(const char *pszFileName, unsigned char *pBuffer, int iBufferSize, int *pWidth, int *pHeight, int *pInternalFormat, int *pImageSize)
{
	FileHandle_t pFile = g_pFileSystem->Open(pszFileName, "rb");

	if (!pFile)
		return false;

	bool debugTime = (developer && (int)developer->value > 2);
	float startTime;

	if (debugTime)
		startTime = gPerformanceCounter.GetCurTime();
	
	DtxHeader Header;
	memset(&Header, 0, sizeof(Header));

	g_pFileSystem->Read(&Header, sizeof(Header), pFile);

	if (Header.iResType != 0 || Header.iVersion != -5 || Header.usMipmaps == 0)
	{
		g_pFileSystem->Close(pFile);
		return false;
	}

	*pWidth = Header.usWidth;
	*pHeight = Header.usHeight;

	int iBpp = Header.ubExtra[2];
	int iSize;
	
	if (iBpp == 3)
	{
		iSize = Header.usWidth * Header.usHeight * 4;
		*pInternalFormat = GL_BGRA;
		//*pInternalFormat = GL_RGBA;
	}
	else if (iBpp == 4)
	{
		iSize = (Header.usWidth * Header.usHeight) >> 1;
		*pInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	}
	else if (iBpp == 5)
	{
		iSize = Header.usWidth * Header.usHeight;
		*pInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	}
	else if (iBpp == 6)
	{
		iSize = Header.usWidth * Header.usHeight;
		*pInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	}
	else
	{
		iSize = 0;
	}

	*pImageSize = iSize;

	if (iSize == 0 || iSize > iBufferSize)
	{
		g_pFileSystem->Close(pFile);
		return false;
	}

	g_pFileSystem->Read(pBuffer, iSize, pFile);

	/*if (iBpp == 3)
	{
		for (int i = 0; i < iSize; i += 4)
		{
			pBuffer[i + 0] ^= pBuffer[i + 2];
			pBuffer[i + 2] ^= pBuffer[i + 0];
			pBuffer[i + 0] ^= pBuffer[i + 2];
		}
	}*/

	g_pFileSystem->Close(pFile);
	
	if (debugTime)
		gEngfuncs.Con_Printf("LoadDTX: load %s take %.4f sec.\n", pszFileName, gPerformanceCounter.GetCurTime() - startTime);
	
	return true;
}

bool GetDTXSize(const char *pszFileName, int *width, int *height)
{
	FileHandle_t pFile = g_pFileSystem->Open(pszFileName, "rb");

	if (!pFile)
		return false;

	bool debugTime = (developer && (int)developer->value > 2);
	float startTime;

	if (debugTime)
		startTime = gPerformanceCounter.GetCurTime();

	DtxHeader Header;
	memset(&Header, 0, sizeof(Header));

	if (!g_pFileSystem->Read(&Header, sizeof(Header), pFile))
		return false;

	if (Header.iResType != 0 || Header.iVersion != -5 || Header.usMipmaps == 0)
	{
		g_pFileSystem->Close(pFile);
		return false;
	}

	*width = Header.usWidth;
	*width = Header.usHeight;

	g_pFileSystem->Close(pFile);
	return true;
}