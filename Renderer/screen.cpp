#include <metahook.h>

#include "screen.h"

static SCREENINFO gScreenInfo{0};

// can be used after ScreenInfo() first called
int &ScreenWidth = gScreenInfo.iWidth;
int &ScreenHeight = gScreenInfo.iHeight;

// call to initalize and get ScreenInfo
SCREENINFO &ScreenInfo()
{
	if (!gScreenInfo.iSize)
	{
		gScreenInfo.iSize = sizeof(SCREENINFO);
		gEngfuncs.pfnGetScreenInfo(&gScreenInfo);
	}
	return gScreenInfo;
}
