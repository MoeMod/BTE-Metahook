#pragma once

#define RGB_YELLOWISH 0x00FFA000 // 255,160,0
#define RGB_REDISH 0x00FF1010 // 255,160,0
#define RGB_GREENISH 0x0000A000 // 0,160,0
#define RGB_LIGHTBLUE 0x0072C5FF //114, 197, 255

#define HIDEMONEY_NONE	0
#define HIDEMONEY_NODRAW	1
#define HIDEMONET_EXTRAAMMO	2


extern float g_fCountDown;
extern char g_sCountDownText[256];
extern int g_MoneyPosIco;

void DisplayInitialize(void);
void DisplayRedraw(float flTime, int iIntermission);
void EngFunc_SPR_DrawAdditive(int frame, int x, int y, const wrect_t *prc);
void EngFunc_FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a);
client_sprite_t *EngFunc_SPR_GetList(char *psz, int *piCount);