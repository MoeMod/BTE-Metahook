#include <metahook.h>
#include "Encode.h"
/* 
	seems doesn't works...
	to be fixed.
*/
bool g_bUpdateVoiceState;

void HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	g_bUpdateVoiceState = true;
	gExportfuncs.HUD_VoiceStatus(entindex, bTalking);
	g_bUpdateVoiceState = false;
}

void Engfunc_GetPlayerInfo(int ent_num, hud_player_info_t *pinfo)
{
	gEngfuncs.pfnGetPlayerInfo(ent_num, pinfo);

	if (!g_bUpdateVoiceState)
		return;

	pinfo->name = UnicodeToANSI(UTF8ToUnicode(pinfo->name));
}