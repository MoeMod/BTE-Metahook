#include <metahook.h>
#include "configs.h"
#include "qgl.h"
#include "shader.h"
#include "LoadDDS.h"
#include "plugins.h"
#include "perf_counter.h"
#include "cvardef.h"
#include "tier0/dbg.h"

#define DDSD_CAPS 0x00000001
#define DDSD_PIXELFORMAT 0x00001000
#define DDPF_FOURCC 0x00000004

#define D3DFMT_DXT1    (('D'<<0)|('X'<<8)|('T'<<16)|('1'<<24))
#define D3DFMT_DXT3    (('D'<<0)|('X'<<8)|('T'<<16)|('3'<<24))
#define D3DFMT_DXT5    (('D'<<0)|('X'<<8)|('T'<<16)|('5'<<24))

#pragma pack(push)
#pragma pack(1)

typedef struct dds_header_s
{
	char Signature[4];

	unsigned int Size1;
	unsigned int Flags1;
	unsigned int Height;
	unsigned int Width;
	unsigned int LinearSize;
	unsigned int Depth;
	unsigned int MipMapCount;
	unsigned int AlphaBitDepth;

	unsigned int NotUsed[10];

	unsigned int Size2;
	unsigned int Flags2;
	unsigned int FourCC;
	unsigned int RGBBitCount;
	unsigned int RBitMask;
	unsigned int GBitMask;
	unsigned int BBitMask;
	unsigned int RGBAlphaBitMask;

	unsigned int ddsCaps1, ddsCaps2, ddsCaps3, ddsCaps4;
	unsigned int TextureStage;
}
dds_header_t;

#pragma pack(pop)

#define SIZE_OF_DXT1(width, height)    ( max(1, ( (width + 3) >> 2 ) ) * max(1, ( (height + 3) >> 2 ) ) * 8 )
#define SIZE_OF_DXT2(width, height)    ( max(1, ( (width + 3) >> 2 ) ) * max(1, ( (height + 3) >> 2 ) ) * 16 )

bool LoadDDS(const char *szFilename, byte *buffer, int bufferSize, int *width, int *height, GLuint *internalformat)
{
	bool debugTime = (developer && (int)developer->value > 2);
	float startTime;

	if (debugTime)
		startTime = gPerformanceCounter.GetCurTime();

	FileHandle_t fp = g_pFileSystem->Open(szFilename, "rb");

	if (!fp)
		return FALSE;

	dds_header_t header;

	if (!g_pFileSystem->Read(&header, sizeof(header), fp))
	{
		*width = 0;
		*height = 0;

		g_pFileSystem->Close(fp);
		return FALSE;
	}

	unsigned int flags = header.Flags1;
	unsigned int FourCC = header.FourCC;
	unsigned int PFFlags = header.Flags2;
	unsigned int linearsize = header.LinearSize;

	if (strncmp(header.Signature, "DDS ", 4))
	{
		gEngfuncs.Con_Printf("LoadDDS: Load %s Failed! (Not a dds file)\n", szFilename);
		g_pFileSystem->Close(fp);
		return FALSE;
	}

	if (!(flags & DDSD_PIXELFORMAT))
	{
		gEngfuncs.Con_Printf("LoadDDS: Load %s Failed! (bad pixel format)\n", szFilename);
		g_pFileSystem->Close(fp);
		return FALSE;
	}

	if (!(flags & DDSD_CAPS))
	{
		gEngfuncs.Con_Printf("LoadDDS: Load %s Failed! (bad file type)\n", szFilename);
		g_pFileSystem->Close(fp);
		return FALSE;
	}

	if (!(PFFlags & DDPF_FOURCC))
	{
		gEngfuncs.Con_Printf("LoadDDS: Load %s Failed! (bad pixel format)\n", szFilename);
		g_pFileSystem->Close(fp);
		return FALSE;
	}


	switch (FourCC)
	{
	case D3DFMT_DXT1:
		*internalformat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		break;
	case D3DFMT_DXT3:
		*internalformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case D3DFMT_DXT5:
		*internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		gEngfuncs.Con_Printf("LoadDDS: %s Only DXT1, DXT3 and DXT5 are supported!\n", szFilename);
		g_pFileSystem->Close(fp);
		return FALSE;
	}

	*width = header.Width;
	*height = header.Height;

	int size = g_pFileSystem->Size(fp) - sizeof(dds_header_t);

	if (size > bufferSize)
	{
		gEngfuncs.Con_Printf("LoadDDS: Load %s Failed! (bad file size)\n", szFilename);
		g_pFileSystem->Close(fp);
		return FALSE;
	}

	int readcount = g_pFileSystem->Read(buffer, size, fp);
	g_pFileSystem->Close(fp);

	if (!readcount)
		return FALSE;

	Assert(1);

	if (debugTime)
	{
	}

	return TRUE;
}

int GetDDSSize(const char *szFilename, int *width, int *height)
{
	FileHandle_t fp = g_pFileSystem->Open(szFilename, "rb");

	if (!fp)
		return FALSE;

	dds_header_t header;

	if (!g_pFileSystem->Read(&header, sizeof(header), fp))
	{
		g_pFileSystem->Close(fp);
		return 0;
	}

	unsigned int compressFormat = header.FourCC;
	int imageSize;
	switch (compressFormat)
	{
	case D3DFMT_DXT1:
		imageSize = SIZE_OF_DXT1(header.Width, header.Height);
		break;
	case D3DFMT_DXT3:
		imageSize = SIZE_OF_DXT2(header.Width, header.Height);
		break;
	case D3DFMT_DXT5:
		imageSize = SIZE_OF_DXT2(header.Width, header.Height);
		break;
	}
	*width = header.Width;
	*height = header.Height;

	g_pFileSystem->Close(fp);

	return imageSize;
}