#include <metahook.h>
#include "Renderer.h"

IRenderer *g_pRenderer = NULL;
HINTERFACEMODULE g_hRenderer = NULL;

void Renderer_Init(void)
{
#if 0
	char filepath[MAX_PATH];
	sprintf(filepath, "renderer.dll");

	g_hRenderer = Sys_LoadModule(filepath);

	if (!g_hRenderer)
		return;

	g_pRenderer = (IRenderer *)((CreateInterfaceFn)Sys_GetFactory(g_hRenderer))(RENDERER_API_VERSION, NULL);
#endif
}

void Renderer_Shutdown(void)
{
	if (g_pRenderer)
		Sys_FreeModule(g_hRenderer);
}

