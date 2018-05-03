#include "metahook.h"
#include "bte_const.h"
#include "hud.h"
#include "BaseUI.h"
#include "r_efx.h"
#include "studio_event.h"
#include "studio.h"
#include "eventscripts.h"
#include "util.h"
#include "exportfuncs.h"

#include "weapons.h"
#include "Client/PlayerClassManager.h"
#include "Client/WeaponManager.h"

#include "Client/HUD/zb4ui.h"
#include "Client/HUD/playbink.h"

int g_iButton = 0;
int g_iBTEWeapon = 0;
//char g_szCurWeapon[32];
wchar_t g_szCurWeapon2[64];
//int g_iCurWeapon; // BTE weapon id
int g_iWeaponStat = 0;
int g_iBl3Muzz = 10;
int g_iSpoce = 0;
int g_bInReload = 0;

PlayerInfo vPlayer[36];
extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];

char g_szLocation[2048];
bool g_bAlive;
int g_iWeapon;
Vector g_vecOrigin, g_vecAngles, g_vecEyePos,g_vecVAngles;
Vector g_vecEye;
Vector g_vecEndPos;
int g_iViewEntityBody, g_iViewEntityRenderMode, g_iViewEntityRenderFX, g_iViewEntityRenderAmout;
color24 g_byViewEntityRenderColor;
cvar_t *pCvar_TipIcon,*pCvar_Blood,*pCvar_Name,*pCvar_DrawBink,*pCvar_DrawPanel,*pCvar_DrawScope,*mh_scoreboard,*pCvar_DrawMenu,*pCvar_ZombieMenu;
cvar_t *cl_righthand;
cvar_t *cl_dynamiccrosshair;
cvar_t *cl_crosshair_color;
cvar_t *cl_crosshair_size;
cvar_t *cl_crosshair_translucent;
cvar_t *cl_crosshair_type;
cvar_t *cl_crosshair_hit;
cvar_t *cl_gunsmoke;

cvar_t *tracerred;
cvar_t *tracergreen;
cvar_t *tracerblue;
cvar_t *traceralpha;
cvar_t *tracerlength;
cvar_t *traceroffset;
cvar_t *tracerspeed;

cvar_t *viewmodel_widescreen;

cvar_t *cl_gunstreak;

cvar_t *hud_fastswitch_wheel;

client_t cl;

char g_sTattoo[128];
int g_iTattooTex[2][3];
int g_iTattooSet_f;
int g_iTattooSet_m;
char g_sTattooSmall[128];
int g_iTattooSmallTex[2][3];
int g_iTattooSmallEnable = 0;
int g_iCountBg;
float g_fMuzzleTime;
char g_szWeaponSkin[20][20];
int g_iWeaponSkin[20][2];
int g_iSetupSkin = 0;
int g_tga_Scope = 0;
int g_tga_Scope_Target = 0;
int g_tga_custom_scope = 0;
int g_tga_custom_scope_m200 = 0;

int g_isZombie;
bool g_iszb4kicking;

char texture[512];
/*
void LoadTattoo(void)
{
	char name[128];
	char *sex[6] = { "male", "female"};
	char *type[3] = {"long","short"};
	for(int i=0;i<2;i++)
	{
		for(int j=0;j<2;j++)
		{
			sprintf(name,"tattoo\\%s_%s_%s",g_sTattoo,sex[i],type[j]);
			
			g_iTattooTex[i][j] = Hud().m_TGA.GetTGA(name);
			if(g_sTattooSmall[0])
			{
				sprintf(name,"tattoo\\%s_%s_%s",g_sTattooSmall,sex[i],type[j]);
				g_iTattooSmallTex[i][j] = Hud().m_TGA.GetTGA(name);
			}
		}
	}
	sprintf(name,"tattoo\\%s_%s_%s",g_sTattoo,"male","origin");
	g_iTattooTex[SEX_MALE][HAND_ORIGIN] = Hud().m_TGA.GetTGA(name);
	sprintf(name,"tattoo\\%s_%s_%s",g_sTattooSmall,"male","origin");
	g_iTattooSmallTex[SEX_MALE][HAND_ORIGIN] = Hud().m_TGA.GetTGA(name);	
}
void RebuildTattoo(model_t *pModel)
{
	// Rebuild Skin
	int iCheck = 0;
	if(pModel && pModel->name)
	{
		for(int i =1;i<21;i++)
		{
			if(strlen(g_szWeaponSkin[i]) && strstr(pModel->name,g_szWeaponSkin[i]))
			{
				//if(g_iWeaponSkin[i][0])
				//{
				//	iCheck = 1;
				//	break;
				//}
				//else if(g_iWeaponSkin[i][1])
				//{
				//	iCheck = 2;
				//	break;
				//}
				//else if(g_iWeaponSkin[i][2])
				//{
				//	iCheck = 0;
				//	break;
				//}
				iCheck = g_iWeaponSkin[i][1];
			}
		}
	}
	g_iSetupSkin = iCheck;
	if(pModel)
	{ 
		studiohdr_t	*pstudio;
		mstudiotexture_t	*ptexture;
		pstudio = (studiohdr_t*)IEngineStudio.Mod_Extradata(pModel);
		if( !pstudio )
			return;
		ptexture = (mstudiotexture_t *)(((byte *)pstudio) + pstudio->textureindex);	
		for(int i = 0; i < pstudio->numtextures; i++ )
		{
			if(ptexture[i].name[0] == '#' && ptexture[i].name[1] == 'F')
			{
				switch (ptexture[i].name[3])
				{
					case 'S' : 
					{
						ptexture[i].index = g_MHTga[g_iTattooTex[SEX_FEMALE][HAND_SHORT]].texid;
						ptexture[i].height = 512;
						ptexture[i].width = 512;

						if(g_iTattooSmallEnable && g_iVideoMode)
						{
							int g_iTattooTest = g_iTattooSmallTex[SEX_FEMALE][HAND_SHORT];
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D,ptexture[i].index);
							glTexSubImage2D(GL_TEXTURE_2D,0,0,508-g_MHTga[g_iTattooTest].height,g_MHTga[g_iTattooTest].width,g_MHTga[g_iTattooTest].height,GL_RGBA,GL_UNSIGNED_BYTE,g_MHTga[g_iTattooTest].data);
						}
						break;
					}
					case 'L' : 
					{
						ptexture[i].index =g_MHTga[g_iTattooTex[SEX_FEMALE][HAND_LONG]].texid;
						ptexture[i].height = 512;
						ptexture[i].width = 512;
						if(g_iTattooSmallEnable && g_iVideoMode)
						{
							int g_iTattooTest = g_iTattooSmallTex[SEX_FEMALE][HAND_LONG];
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D,ptexture[i].index);
							glTexSubImage2D(GL_TEXTURE_2D,0,0,508-g_MHTga[g_iTattooTest].height,g_MHTga[g_iTattooTest].width,g_MHTga[g_iTattooTest].height,GL_RGBA,GL_UNSIGNED_BYTE,g_MHTga[g_iTattooTest].data);
						}
						break;
					}					
					default :
					{
						LogToFile("[纹身管理]:错误的纹理 %s(未知的类型)",ptexture[i].name[2]);
						break;
					}
				}
			}
			else if(ptexture[i].name[0] == '#' && ptexture[i].name[1] == 'M')
			{
				switch (ptexture[i].name[3])
				{
					case 'S' : 
					{
						ptexture[i].index = g_MHTga[g_iTattooTex[SEX_MALE][HAND_SHORT]].texid;
						ptexture[i].height = 512;
						ptexture[i].width = 512;
						if(g_iTattooSmallEnable && g_iVideoMode)
						{
							int g_iTattooTest = g_iTattooSmallTex[SEX_MALE][HAND_SHORT];
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D,ptexture[i].index);
							glTexSubImage2D(GL_TEXTURE_2D,0,0,508-g_MHTga[g_iTattooTest].height,g_MHTga[g_iTattooTest].width,g_MHTga[g_iTattooTest].height,GL_RGBA,GL_UNSIGNED_BYTE,g_MHTga[g_iTattooTest].data);
						}
						break;
					}
					case 'L' : 
					{
						ptexture[i].index = g_MHTga[g_iTattooTex[SEX_MALE][HAND_LONG]].texid;
						ptexture[i].height = 512;
						ptexture[i].width = 512;
						if(g_iTattooSmallEnable && g_iVideoMode)
						{
							int g_iTattooTest = g_iTattooSmallTex[SEX_MALE][HAND_LONG];
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D,ptexture[i].index);
							glTexSubImage2D(GL_TEXTURE_2D,0,0,508-g_MHTga[g_iTattooTest].height,g_MHTga[g_iTattooTest].width,g_MHTga[g_iTattooTest].height,GL_RGBA,GL_UNSIGNED_BYTE,g_MHTga[g_iTattooTest].data);
						}
						break;
					}					
					case 'O' : 
					{
						ptexture[i].index = g_MHTga[g_iTattooTex[SEX_MALE][HAND_ORIGIN]].texid;
						ptexture[i].height = 512;
						ptexture[i].width = 512;
						if(g_iTattooSmallEnable && g_iVideoMode)
						{
							int g_iTattooTest = g_iTattooSmallTex[SEX_MALE][HAND_ORIGIN];
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D,ptexture[i].index);
							glTexSubImage2D(GL_TEXTURE_2D,0,0,508-g_MHTga[g_iTattooTest].height,g_MHTga[g_iTattooTest].width,g_MHTga[g_iTattooTest].height,GL_RGBA,GL_UNSIGNED_BYTE,g_MHTga[g_iTattooTest].data);
						}
						break;
					}
					default :
					{
						LogToFile("[纹身管理]:错误的纹理 %s(未知的类型)",ptexture[i].name[2]);
						break;
					}
				}
			}
			else if(ptexture[i].name[0] == '#' && ptexture[i].name[1] == '5' && ptexture[i].name[11] == 'H')
			{
				ptexture[i].index = g_MHTga[g_iTattooTex[SEX_MALE][ptexture[i].name[31]?HAND_LONG:HAND_SHORT]].texid;
				ptexture[i].height = 512;
				ptexture[i].width = 512;
				if(g_iTattooSmallEnable && g_iVideoMode)
				{
					int g_iTattooTest = g_iTattooSmallTex[SEX_MALE][ptexture[i].name[31]?HAND_LONG:HAND_SHORT];
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,ptexture[i].index);
					glTexSubImage2D(GL_TEXTURE_2D,0,0,508-g_MHTga[g_iTattooTest].height,g_MHTga[g_iTattooTest].width,g_MHTga[g_iTattooTest].height,GL_RGBA,GL_UNSIGNED_BYTE,g_MHTga[g_iTattooTest].data);
				}
			}
			else if(ptexture[i].name[0] == '#' && ptexture[i].name[1] == '5' && ptexture[i].name[11] == 'G')
			{
				ptexture[i].index = g_MHTga[g_iTattooTex[SEX_FEMALE][ptexture[i].name[28]?HAND_LONG:HAND_SHORT]].texid;
				ptexture[i].height = 512;
				ptexture[i].width = 512;
				if(g_iTattooSmallEnable && g_iVideoMode)
				{
					int g_iTattooTest = g_iTattooSmallTex[SEX_FEMALE][ptexture[i].name[28]?HAND_LONG:HAND_SHORT];
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,ptexture[i].index);
					glTexSubImage2D(GL_TEXTURE_2D,0,0,508-g_MHTga[g_iTattooTest].height,g_MHTga[g_iTattooTest].width,g_MHTga[g_iTattooTest].height,GL_RGBA,GL_UNSIGNED_BYTE,g_MHTga[g_iTattooTest].data);
				}
			}
		}
	}
}
*/

void CheckViewEntity(void)
{
	cl_entity_t *viewent;
	viewent = gEngfuncs.GetViewModel();

	if (!viewent->model->name)
		return;

	model_t *model = IEngineStudio.Mod_ForName(viewent->model->name, false);

	studiohdr_t	*pstudio;
	mstudiotexture_t	*ptexture;
	pstudio = (studiohdr_t*)IEngineStudio.Mod_Extradata(model);
	ptexture = (mstudiotexture_t *)(((byte *)pstudio) + pstudio->textureindex);	

	int id = gEngfuncs.GetLocalPlayer()->index;

	if (IS_FIRSTPERSON_SPEC)
		id = g_iUser2;

	if (PlayerClassManager()[id])
		g_iViewEntityBody = PlayerClassManager()[id].sex - 1;

	//gEngfuncs.SetViewAngles();
	if (viewent)
	{
		if(viewent->model->name && !strstr(viewent->model->name, "v_foot"))
		{
			//g_iCurWeapon = 0;
			
			char szCurWeapon[32];
			strcpy(szCurWeapon, viewent->model->name + 9);
			szCurWeapon[strlen(szCurWeapon) - 4] = 0;

			if (strstr(szCurWeapon, "_2") || !strcmp(szCurWeapon, "knife_w"))
			{
				szCurWeapon[strlen(szCurWeapon) - 2] = 0;
			}
			else
			{
				//g_iShowCustomCrosshair = 0;
				Hud().m_iHideHUDDisplay &= ~HIDEWEAPON_CROSSHAIR;
				//Hud().m_Ammo.m_flNextDrawExtraAmmo = Hud().m_flTime + 0.01f;
			}

			wcscpy(g_szCurWeapon2, GetWeaponNameFormat(szCurWeapon));

			WeaponManager().SetCurWeapon(szCurWeapon);

			if (g_iMod == MOD_ZSE)
			{
				if (g_bAlive)
				{
					if (strstr(viewent->model->name, "zombi"))
					{
						HudBinkPlayer().SetPos(0.5 * ScreenWidth, 0.4 * ScreenHeight, 1);
						HudBinkPlayer().SetColor(255, 255, 255);
						HudBinkPlayer().BinkInstance("cstrike\\resource\\zombi\\origin.bik", 0, 0);
						
						g_iZBNvg = 1;
						g_isZombie = 1;
					}
					else
						g_isZombie = 0;
				}
			}

			if (g_iMod == MOD_ZB4)
			{
				if(strstr(viewent->model->name,"z4normal"))
					HudZombieMod4().m_Power.SetZbSkill(0, 11);

				if(strstr(viewent->model->name,"z4light"))
					HudZombieMod4().m_Power.SetZbSkill(1, 60);

				if(strstr(viewent->model->name,"z4heavy"))
					HudZombieMod4().m_Power.SetZbSkill(2, 25);

				if(strstr(viewent->model->name,"z4hide"))
					HudZombieMod4().m_Power.SetZbSkill(3, 20);

				if(strstr(viewent->model->name,"z4humpback"))
					HudZombieMod4().m_Power.SetZbSkill(4, 45);
			}
			if(strstr(viewent->model->name,"infinity"))
			{
				g_iBTEWeapon = WPN_INFINITYEX;
			}
			else if(strstr(viewent->model->name,"m2.mdl"))
			{
				g_iBTEWeapon = WPN_M2;
			}
			else if(strstr(viewent->model->name,"sfmg"))
			{
				g_iBTEWeapon = WPN_SFMG;
			}
			else if(strstr(viewent->model->name,"sfsniper")) 
			{	
				g_iBTEWeapon = WPN_SFSNIPER;
			}
			else if(strstr(viewent->model->name,"cheytaclrrs"))
			{
				g_iBTEWeapon = WPN_M200;
			}
			
			else if(strstr(viewent->model->name,"janusmk5"))
			{
				g_iBTEWeapon = WPN_JANUS5;
			}/*
			else if(strstr(viewent->model->name,"janus7"))
			{
				g_iBTEWeapon = WPN_JANUS7;
			}*/
			else if(strstr(viewent->model->name,"janus"))
			{
				g_iBTEWeapon = WPN_JANUS;
			}
			else if (strstr(viewent->model->name, "destroyer"))
			{
				g_iBTEWeapon = WPN_DESTROYER;
			}
			else if (strstr(viewent->model->name, "gauss"))
			{
				g_iBTEWeapon = WPN_GAUSS;
			}
			else if (strstr(viewent->model->name, "buffawp"))
			{
				g_iBTEWeapon = WPN_BUFFAWP;
			}
			else if (strstr(viewent->model->name, "bloodhunter"))
			{
				g_iBTEWeapon = WPN_BLOODHUNTER;
			}
			else if (strstr(viewent->model->name, "stormgiant"))
			{
				g_iBTEWeapon = WPN_STORMGIANT;
			}
			else if (strstr(viewent->model->name, "dualsword"))
			{
				g_iBTEWeapon = WPN_DUALSWORD;
			}
			else if (strstr(viewent->model->name, "zombibomb"))
			{
				g_iBTEWeapon = WPN_ZOMBIBOMB;
			}
			else if (strstr(viewent->model->name, "gunkata"))
			{
				g_iBTEWeapon = WPN_GUNKATA;
			}
			else if (strstr(viewent->model->name, "v_foot"))
			{
				g_iszb4kicking = true;
			}
			else g_iszb4kicking = g_iBTEWeapon = 0;
		}		
	}		
}

