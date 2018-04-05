#pragma once

// Backward support (for MetaHook 2.0) only, DO NOT use any more.

#define MAX_DRAWIMAGE 64

struct DrawImageItem
{
	int iFunction;
	int iCheck; // 1 is Use Hud,2 is Use Spr File!
	int iCenter;
	int iSprIndex;
	HSPRITE hl_hSprites;
	color24 color;
	int x; int y;
	int iMode;
	float flStartDisplayTime;
	float flEndDisplayTime;
	int iChanne;
	int iLength;
};

class CHudSPRElements : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);

public:
	void AddElement(const DrawImageItem &item);
	DrawImageItem m_rgDrawImage[MAX_DRAWIMAGE];
};

CHudSPRElements &HudSPRElements();