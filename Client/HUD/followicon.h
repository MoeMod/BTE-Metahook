#pragma once
#if 0
#define VectorCopy(a,b) {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}
#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define VectorLength(a) (sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2] ))
#endif
#define DRAWICON_MAX	64

struct DrawIconItem
{
	HSPRITE hl_hSprites;
	int iSprIndex;      //If iCheck=1
	int x, y, z;
	int iToggle;
	int iDistance;
	int iChannel;
	int r, g, b;
	int iBox;
};

class CHudFollowIconElements : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);

public:
	void Add(DrawIconItem item);

public:
	DrawIconItem m_Icon[DRAWICON_MAX + 1];
};

CHudFollowIconElements &HudFollowIconElements();