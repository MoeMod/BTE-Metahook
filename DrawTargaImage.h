#pragma once


extern int g_UI_Panel[3][3];
extern int g_UI_Panel2[3][3];
extern int g_UI_Panel_Dark[3][3];
extern int g_UI_Panel_Gray[3][3];
extern int g_UI_Output[3][3];
extern int g_UI_Panel_TeamSuit[3][3];

void GL_LoadPanel(int tid[3], char *szName);
void GL_LoadFullPanel(int tid[3][3], char *szName);
int GL_DrawTGA(int index, int r, int g, int b, int a, int iX, int iY, float scale);
int GL_DrawTGA2(int index, int iX, int iY, int iW, int iH, int alpha);
int GL_DrawPanel(int tid[3], int iX, int iY, int iW, int iH, int alpha);
int GL_DrawFullPanel(int tid[3][3], int iX, int iY, int iW, int iH, int alpha);