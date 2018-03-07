#pragma once

#define MENU_TEAM 2
#define MENU_MAPBRIEFING 4
#define MENU_CLASS_T 26
#define MENU_CLASS_CT 27

void MessageInitialize(void);
int EngFunc_HookUserMsg(char *szMsgName, pfnUserMsgHook pfn);
extern int g_iMoney;
extern int g_iBuyTime;
extern int g_Menu_Drawing;
extern int g_Menu_WaitingForMore;
extern int g_BinkScopeColor;
extern int g_iRoundTime;
extern int g_iTeam;
extern int g_iZ4Nvg, g_iZ4Nvg2;
extern float g_flZ4Nvg, g_flZ4Nvg2;
extern float g_flZESFade;
extern int g_bInBuyzone;
extern vec3_t g_vecHostagePos[8];