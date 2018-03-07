#pragma once
#define MAX_TGA 700
#define MH_MAX_TGA	750

#include "imagelib.h"
#include <vector>

struct DrawTgaItem
{
	char szName[64];
	int iFunction;
	int iCenter;
	int r, g, b;
	int x, y, w, h;
	int iMode;
	float flStartDisplayTime;
	float flEndDisplayTime;
	int iChanne;
	int iMessage;
	int iMHTgaID;
	float fScale;
};

struct Tga
{
	char szName[256];
	int iTexture;
	int iWidth;
	int iHeight;
};

extern imagedata_s g_MHTga[MH_MAX_TGA];
extern std::vector<Tga> g_Texture;

class CHudTGAElements : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);
	void LoadList(void);

public:
	int GetTGA(char *name, bool f = false);
	int GetBMP(char *name);
	int FindTexture(char *name);

public:
	void AddElement(DrawTgaItem element);
	

public:
	int m_iCurrentTexture;
	int m_iCurrentTGA;

private:
	BOOL m_bLoadedTGA;
};

CHudTGAElements &HudTGAElements();