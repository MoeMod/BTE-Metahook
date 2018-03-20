#include <metahook.h>
#include "configs.h"
#include "Hook_LoadTGA.h"
#include "plugins.h"
#include "perf_counter.h"
#include "cvardef.h"

int (*g_pfnLoadTGA)(const char *szFilename, byte *buffer, int bufferSize, int *width, int *height);
hook_t *g_phLoadTGA;

bool g_bLoadingTGA;
int g_iLastTGAWidth, g_iLastTGAHeight;
byte g_bTGABuffer[1024 * 1024 * 4];

#pragma pack(1)

typedef struct _TargaHeader
{;
	unsigned char id_length, colormap_type, image_type;
	unsigned short colormap_index, colormap_length;
	unsigned char colormap_size;
	unsigned short x_origin, y_origin, width, height;
	unsigned char pixel_size, attributes;
}
TargaHeader;

#pragma pack()

int LoadTGA_New(const char *szFilename, byte *buffer, int bufferSize, int *width, int *height)
{
	g_bLoadingTGA = FALSE;

	FileHandle_t fp = g_pFileSystem->Open(szFilename, "rb");

	if (!fp)
		return g_pfnLoadTGA(szFilename, buffer, bufferSize, width, height);

	TargaHeader header;
	int result = g_pFileSystem->Read(&header, sizeof(header), fp);
	g_pFileSystem->Close(fp);

	if (result == 0)
		return g_pfnLoadTGA(szFilename, buffer, bufferSize, width, height);

	if (header.width <= 256 && header.height <= 256)
		return g_pfnLoadTGA(szFilename, buffer, bufferSize, width, height);

	g_bLoadingTGA = TRUE;
	return g_pfnLoadTGA(szFilename, g_bTGABuffer, header.width * header.height, &g_iLastTGAWidth, &g_iLastTGAHeight);
}

void LoadTGA_InstallHook()
{
	g_pfnLoadTGA = (int(*)(const char *, byte *, int, int *, int *))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, LOADTGA_SIG, sizeof(LOADTGA_SIG) - 1);
	g_phLoadTGA = g_pMetaHookAPI->InlineHook(g_pfnLoadTGA, LoadTGA_New, (void *&)g_pfnLoadTGA);
}