#include <metahook.h>
#include <cvardef.h>
#include "plugins.h"

void CVAR_Register(void)
{
	
}

void CVAR_Reset(void)
{
	cvar_t *name = gEngfuncs.pfnGetCvarPointer("name");

	if (name)
		name->flags &= ~FCVAR_PRINTABLEONLY;
}

void CVAR_Init(void)
{
	CVAR_Register();
	CVAR_Reset();
}