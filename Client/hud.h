/*
	HUD and based instances
*/

#pragma once

#include <assert.h>
#include <string.h>

#include "wrect.h"

#include "cvardef.h"
#include "client.h"
#include "Renderer/screen.h"
#include <vector>

typedef float vec_t;

#define MAX_SPRITE_NAME_LENGTH 24

#define DHN_DRAWZERO 1
#define DHN_2DIGITS 2
#define DHN_3DIGITS 4
#define DHN_4DIGITS 8
#define DHN_5DIGITS 16

#define MIN_ALPHA 100
#define FADE_TIME 100

typedef struct {
	int x, y;
} POSITION;

class CHudBase
{
public:
	POSITION m_pos;
	int m_iFlags;
	int   m_type;
	CHudBase();
	virtual		~CHudBase() {}
	virtual void Init(void) { return; }
	virtual void VidInit(void) { return; }
	virtual void Draw(float flTime) { return; }
	virtual void Think(void) { return; }
	virtual void Reset(void) { return; }
	virtual void InitHUDData(void) {}		// called every time a server is connected to
};
/*
struct HUDLIST {
	CHudBase	*p;
	HUDLIST		*pNext;
};*/

typedef int(*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);

extern pfnUserMsgHook pmInitHUD;
extern pfnUserMsgHook pmResetHUD;
extern cvar_t *hud_draw;

class CHud
{
private:
	//HUDLIST *m_pHudList;
	std::vector<CHudBase *> m_List;
	HSPRITE *m_rghSprites;
	wrect_t *m_rgrcRects;
	char *m_rgszSpriteNames;
	client_sprite_t *m_pSpriteList = NULL;
	int m_iSpriteCountAllRes;
	int m_iSpriteCount;
public:
	int m_iIntermission;
	float m_flTime, m_flOldTime;
public:
	int Redraw(float time, int intermission);
	int GetStudioModelInterface(int iVersion, struct r_studio_interface_s **ppStudioInterface, struct engine_studio_api_s *pEngineStudio);
	void Init(void);
	void ShutDown(void);
	void Frame(double flHostFrameTime);
	int VidInit(void);
	int UpdateClientData(client_data_t *cdata, float time);
	void Think(void);
	void AddHudElem(CHudBase *phudelem);
	int MsgFunc_InitHUD(const char *pszName, int iSize, void *pbuf)
	{
		for (CHudBase *p : m_List)
		{
			p->InitHUDData();
		}

		return pmInitHUD(pszName, iSize, pbuf);
	}
	int MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf)
	{
		for (CHudBase *p : m_List)
		{
			p->Reset();
		}

		return pmResetHUD(pszName, iSize, pbuf);
	}
	CHud();
	~CHud()
	{
		ShutDown();

		delete[] m_rghSprites;
		delete[] m_rgrcRects;
		delete[] m_rgszSpriteNames;
	}
	HSPRITE GetSprite(int index)
	{
		return (index < 0) ? 0 : m_rghSprites[index];
	}

	wrect_t& GetSpriteRect(int index)
	{
		return m_rgrcRects[index];
	}
	inline int GetSpriteIndex(const char *SpriteName)
	{
		for (int i = 0; i < m_iSpriteCount; i++)
		if (strncmp(SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH) == 0)
			return i;

		return -1;
	}
	int DrawHudString(int xpos, int ypos, int iMaxX, char *szIt, int r, int g, int b);
	int DrawHudNumberString(int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b);
	int DrawHudStringReverse(int xpos, int ypos, int iMinX, char *szString, int r, int g, int b);
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b);
	int DrawHudNumber(int x, int y, int iNumber, int r, int g, int b);
	int DrawHudNumber2(int x, int y, bool DrawZero, int iDigits, int iNumber, int r, int g, int b);
	int DrawHudNumber2(int x, int y, int iNumber, int r, int g, int b);
	int GetNumBits(int iNumber);
public:
	SCREENINFO m_scrinfo;
	double m_flTimeDelta;
	float m_FOV;
	int m_iRes;
	int m_iHideHUDDisplay;
	int m_iFlags;
	int m_iWeaponBits;
	int m_HUD_number_0 = 0;
	int m_iFontHeight = 0, m_iFontWidth = 0;
	int m_iWeapon = 0;
	BOOL m_bCreateSmoke;

public:
	vec3_t m_vecOrigin, m_vecAngles;

public:	// HUD Elements (pimpl, see hud.cpp)
	class CHudFollowIconElements &m_FollowIcon;
	class CHudFontTextElements &m_FontText;
	class CHudSPRElements &m_SPR;
	class CHudTGAElements &m_TGA;
};

//extern CHud gHUD;
extern CHud &Hud();
extern CHud &gHUD;

//#define ScreenWidth Hud().m_scrinfo.iWidth
//#define ScreenHeight Hud().m_scrinfo.iHeight

#define HUD_ACTIVE (1<<1)

extern struct cvar_s *cl_righthand;
extern struct cvar_s *cl_radartype;
extern struct cvar_s *cl_dynamiccrosshair;
extern struct cvar_s *cl_crosshair_color;
extern struct cvar_s *cl_crosshair_size;
extern struct cvar_s *cl_crosshair_translucent;
extern struct cvar_s *cl_crosshair_type;
extern struct cvar_s *cl_killeffect;
extern struct cvar_s *cl_killmessage;
extern struct cvar_s *cl_headname;
extern struct cvar_s *cl_newmenu;
extern struct cvar_s *cl_newmenu_drawbox;
extern struct cvar_s *cl_newradar;
extern struct cvar_s *cl_newradar_size;
extern struct cvar_s *cl_newradar_r;
extern struct cvar_s *cl_newradar_g;
extern struct cvar_s *cl_newradar_b;
extern struct cvar_s *cl_newradar_a;
extern struct cvar_s *cl_newchat;
extern struct cvar_s *cl_shadows;
extern struct cvar_s *cl_scoreboard;
extern struct cvar_s *cl_fog_skybox;
extern struct cvar_s *cl_fog_density;
extern struct cvar_s *cl_fog_r;
extern struct cvar_s *cl_fog_g;
extern struct cvar_s *cl_fog_b;
extern struct cvar_s *cl_minmodel;
extern struct cvar_s *cl_min_t;
extern struct cvar_s *cl_min_ct;
extern struct cvar_s *cl_corpsestay;
extern struct cvar_s *cl_corpsefade;

#define MAX_HOSTAGES 24
#define MAX_TEAM_NAME 16

struct hostage_info_t
{
	bool dead;
	Vector origin;
	int health;
	float radarflash;
	int radarflashon;
	int radarflashes;
	char teamname[MAX_TEAM_NAME];
};