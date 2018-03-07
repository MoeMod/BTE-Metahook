#pragma once

#define MAX_DRAWFONTTEXT 64

struct DrawFontTextItem
{
	int iCenter;
	char szText[2048];
	color24 color;
	int x; int y;
	float flDisplayTime;
	float flStartTime;
	float fFadeTime;
	int iScale;
	int iFillBg;
	int iChanne;
};

class CHudFontTextElements : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);
	void AddElement(DrawFontTextItem item);
};