#pragma once

void TEXD_BeginDraw(void);
void TEXD_EndDraw(void);
void TEXD_BindTexture(int texid);
void TEXD_SetColor(int r, int g, int b, int a);
void TEXD_SetColor(colorVec color);
void TEXD_SetRenderMode(int mode);
void TEXD_DrawTexture(int x, int y, float w, float h, wrect_t *prc, float size = 1, int mode = 0);
void TEXD_DrawTexture2(int x, int y, float w, float h, float size = 1, int mode = 0);