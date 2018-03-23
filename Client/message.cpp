#include "base.h"
#include "exportfuncs.h"
#include "hud.h"
#include "configs.h"
#include "r_efx.h"
#include "msghook.h"
#include "parsemsg.h"
#include "HUD3D.h"
#include "HUD3D_ZB.h"
#include "msg_util.h"
#include "weapons.h"
#include "display.h"
#include "util.h"

#include "GameUI/CSBTEMapLoading.h"

#include "Client/HUD/zse.h"
#include "Client/HUD/zb4ui.h"
#include "Client/HUD/zb3ui.h"
#include "Client/HUD/zombieescape.h"
#include "Client/HUD/zbsboard.h"
#include "Client/HUD/ZombieCountDown.h"
#include "Client/HUD/ZombieSelect.h"
#include "Client/HUD/gdboard.h"
#include "Client/HUD/deathrun.h"
#include "Client/HUD/nvg.h"
#include "Client/HUD/playbink.h"
#include "Client/HUD/deathmsg.h"
#include "Client/HUD/BuffClassDamage.h"
#include "Client/HUD/buffawp.h"
#include "Client/HUD/sfsniper.h"
#include "Client/HUD/destroyer.h"
#include "Client/HUD/DrawTABPanel.h"
#include "Client/HUD/scoreboard.h"
#include "Client/HUD/Statistics.h"
#include "Client/HUD/respawnbar.h"
#include "Client/HUD/quest.h"
#include "Client/HUD/hudmenu.h"
#include "Client/HUD/DrawRetina.h"
#include "Client/HUD/hud_saytext.h"
#include "Client/HUD/ammo.h"
#include "Client/HUD/roundtimer.h"
#include "Client/HUD/health.h"

#include "Client/HUD/drawimage.h"
#include "Client/HUD/FontText.h"
#include "Client/HUD/followicon.h"
#include "Client/HUD/DrawTGA.h"

#include "MGUI/mgui.h"
#include "MGUI/BTEPanel.h"
#include "MGUI/TeamMenu.h"

pfnUserMsgHook pmTeamScore;
pfnUserMsgHook pmResetHUD;
pfnUserMsgHook pmHealth;
pfnUserMsgHook pmBattery;
pfnUserMsgHook pmArmorType;
pfnUserMsgHook pmScoreAttrib;
pfnUserMsgHook pmHideWeapon;
pfnUserMsgHook pmLocation;
pfnUserMsgHook pmWeaponList;
pfnUserMsgHook pmCurWeapon;
pfnUserMsgHook pmAmmoX;
pfnUserMsgHook pmDeathMsg;
pfnUserMsgHook pmTeamInfo;
pfnUserMsgHook pmScoreInfo;
pfnUserMsgHook pmSetFOV;
pfnUserMsgHook pmSendAudio;
pfnUserMsgHook pmShowMenu;
pfnUserMsgHook pmStatusValue;
pfnUserMsgHook pmMoney;
pfnUserMsgHook pmScreenFade;
pfnUserMsgHook pmNVGToggle;
pfnUserMsgHook pmServerName;
pfnUserMsgHook pmTextMsg;
pfnUserMsgHook pmRoundTime;
pfnUserMsgHook pmShowTimer;
pfnUserMsgHook pmHLTV;
pfnUserMsgHook pmStatusIcon;
pfnUserMsgHook pmHostagePos;
pfnUserMsgHook pmHostageK;
pfnUserMsgHook pmGameMode;
pfnUserMsgHook pmBombDrop;
pfnUserMsgHook pmBombPickup;
pfnUserMsgHook pmFlashBat;
pfnUserMsgHook pmFlashlight;
pfnUserMsgHook pmAmmoPickup;
pfnUserMsgHook pmSayText;
pfnUserMsgHook pmInitHUD;
pfnUserMsgHook pmVGUIMenu;

char g_szFontBuf[512];
int g_iMoney=0,g_iBuyTime=0;
int g_Menu_Drawing=0;
int g_Menu_WaitingForMore=0;
int g_BinkScopeColor=1;
int g_iRoundTime;
int g_iTeam;
int g_iZ4Nvg, g_iZ4Nvg2;
float g_flZ4Nvg, g_flZ4Nvg2;
int g_bInBuyzone;
vec3_t g_vecHostagePos[8];

void MessageInitialize(void)
{
	g_Menu_Drawing = 0;
	memset(g_szLocation, 0, sizeof(g_szLocation));
	memset(g_szFontBuf,0,sizeof(g_szFontBuf));
}

int MsgFunc_StatusValue(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iCol = READ_BYTE();
	int iIsFriend = READ_SHORT();
	if(iCol == 1)
	{
		if(iIsFriend  < 2)
		{
			g_BinkScopeColor = 1;
		}
		else
		{
			//if(Hud().m_FOV<=40.0 && g_iBTEWeapon == WPN_SFSNIPER) gEngfuncs.pfnPlaySoundByName("weapons/sfsniper_insight1.wav", 1.0);
			g_BinkScopeColor = 0;
		}
	}

	return pmStatusValue(pszName, iSize, pbuf);
}
int MsgFunc_TeamScore(const char *pszName, int iSize, void *pbuf)
{	
	BEGIN_READ(pbuf, iSize);
	char teamtext[12];
	strcpy(teamtext, READ_STRING());
	
	if(g_iMod != MOD_GD)
	{
		if(strstr(teamtext,"CT"))
		{
			HudTABBoard().m_iTeamScore[0] = HudScoreBoard().m_iRight = READ_SHORT();
		}
		else if(strstr(teamtext,"TERRORIST"))
		{
			HudTABBoard().m_iTeamScore[1] = HudScoreBoard().m_iLeft = READ_SHORT();
		}
		
		HudScoreBoard().m_iCenter = HudScoreBoard().m_iLeft + HudScoreBoard().m_iRight +1;
	}
	return pmTeamScore(pszName, iSize, pbuf);
}

int MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf)
{
	return HudDeathNotice().MsgFunc_DeathMsg(pszName, iSize, pbuf);
}
int MsgFunc_InitHUD(const char *pszName, int iSize, void *pbuf)
{
	return Hud().MsgFunc_InitHUD(pszName, iSize, pbuf);
}
int MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf)
{	
	MessageInitialize();
	return Hud().MsgFunc_ResetHUD(pszName, iSize, pbuf);
}
int MsgFunc_ShowMenu(const char *pszName, int iSize, void *pbuf)
{
	if (HudMenu().MsgFunc_ShowMenu(pszName, iSize, pbuf))
		return 1;
	else
		return pmShowMenu(pszName, iSize, pbuf);
}

int MsgFunc_VGUIMenu(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	byte MenuID = READ_BYTE();
	if (MenuID == MENU_TEAM)
	{
		g_TeamMenu.Show();
		g_TeamMenu.SwitchTeam(2);
		return 1;
	}
	else if (MenuID == MENU_CLASS_T)
	{
		g_TeamMenu.Show();
		g_TeamMenu.SwitchTeam(1);
		return 1;
	}
	else if (MenuID == MENU_CLASS_CT)
	{
		g_TeamMenu.Show();
		g_TeamMenu.SwitchTeam(2);
		return 1;
	}
	
	return pmVGUIMenu(pszName, iSize, pbuf);
}

int MsgFunc_SendAudio(const char *pszName, int iSize, void *pbuf)
{	
	BEGIN_READ(pbuf, iSize);
	int iSender = READ_BYTE();
	READ_STRING();
	READ_SHORT();
	if(iSender != gEngfuncs.GetLocalPlayer()->index)
	{
		vPlayer[iSender].fAudioTime = Hud().m_flTime + 2.0;
	}

	return pmSendAudio(pszName, iSize, pbuf);
}
int MsgFunc_Location(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iPlayerID = READ_BYTE();
	if (iPlayerID == gEngfuncs.GetLocalPlayer()->index)
		strcpy(g_szLocation, READ_STRING());

	return pmLocation(pszName, iSize, pbuf);
}

int MsgFunc_SetFOV(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize); 
	Hud().m_FOV = (float) READ_BYTE();

	if (Hud().m_FOV <= 40.0/* && g_bAlive*/)
	{
		if(g_iBTEWeapon == WPN_SFSNIPER)
		{
			//gEngfuncs.pfnPlaySoundByName("weapons/sfsniper_zoom.wav", 1.0);
			// Start
			HudSfsniperScope().Run();
		}
		else if (g_iBTEWeapon == WPN_DESTROYER)
		{
			HudDestroyerSniperScope().Run();
		}
		else if (g_iBTEWeapon == WPN_BUFFAWP)
		{
			HudBuffAWPSniperScope().Run();
		}
		else
		{
			//Stop
			HudSfsniperScope().End();
			HudDestroyerSniperScope().End();
			HudBuffAWPSniperScope().End();
		}
		if(pCvar_DrawScope->value != 0 && g_iBTEWeapon != WPN_SFSNIPER)
		{
			g_iSpoce = 1;
		}
	}
	else if (Hud().m_FOV > 40.0)
	{
		// Stop
		HudSfsniperScope().End();
		HudDestroyerSniperScope().End();
		HudBuffAWPSniperScope().End();
		g_iSpoce = 0;
	}

	return pmSetFOV(pszName, iSize, pbuf);
}
int MsgFunc_Health(const char *pszName, int iSize, void *pbuf)
{	
	BEGIN_READ(pbuf, iSize);
	int iHealth = READ_SHORT();
	
	if (iHealth == -1)
	{
		BEGIN_READ(pbuf, iSize);
		iHealth = READ_BYTE();
	}

	if (iHealth <= 0)
	{
		if (g_iBTEWeapon == WPN_SFSNIPER)
			HudSfsniperScope().End();

		if (g_iBTEWeapon == WPN_DESTROYER)
			HudDestroyerSniperScope().End();

		if (g_iBTEWeapon == WPN_BUFFAWP)
			HudBuffAWPSniperScope().End();

		if(g_iMod != MOD_TDM)
			BTEPanel_BuyMenu_Close();

		g_iHookVAngle = 0;
		g_iShowCustomCrosshair = 0;

	}

	if (iHealth != HudHealth().m_iHealth)
	{
		HudHealth().m_iHealth = iHealth;
		HudHealth().m_flHealthFade = FADE_TIME;

		if (iHealth >= 10000)
			HudHealth().m_iHealthFlags = DHN_5DIGITS | DHN_DRAWZERO;
		else if (iHealth >= 1000)
			HudHealth().m_iHealthFlags = DHN_4DIGITS | DHN_DRAWZERO;
		else
			HudHealth().m_iHealthFlags = DHN_3DIGITS | DHN_DRAWZERO;
	}

	if (iHealth > 100)
		return 1;
	else
		return pmHealth(pszName, iSize, pbuf);
}
int MsgFunc_Battery(const char *pszName, int iSize, void *pbuf)
{	
	BEGIN_READ(pbuf, iSize);
	int iArmor = READ_SHORT();

	if (iArmor != HudHealth().m_iArmor)
	{
		HudHealth().m_iArmor = iArmor;
		HudHealth().m_flArmorFade = FADE_TIME;

		if (iArmor >= 10000)
			HudHealth().m_iArmorFlags = DHN_5DIGITS | DHN_DRAWZERO;
		else if (iArmor >= 1000)
			HudHealth().m_iArmorFlags = DHN_4DIGITS | DHN_DRAWZERO;
		else
			HudHealth().m_iArmorFlags = DHN_3DIGITS | DHN_DRAWZERO;

		if (iArmor <= 0)
			HudHealth().m_iArmorType = 0;
	}

	return pmBattery(pszName, iSize, pbuf);
}
int MsgFunc_ArmorType(const char *pszName, int iSize, void *pbuf)
{	
	BEGIN_READ(pbuf, iSize);
	HudHealth().m_iArmorType = READ_BYTE();
	return pmArmorType(pszName, iSize, pbuf);
}
int MsgFunc_ScoreAttrib(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iPlayerID = READ_BYTE();
	int iFlags = READ_BYTE();
	g_PlayerExtraInfo[iPlayerID].iFlag = iFlags;
	if (iFlags & SCOREATTRIB_DEAD) vPlayer[iPlayerID].alive = 0;
	else vPlayer[iPlayerID].alive = 1;
	if (iPlayerID == gEngfuncs.GetLocalPlayer()->index)
	{
		if (iFlags & SCOREATTRIB_DEAD)
			g_bAlive = false;
		else
			g_bAlive = true;
	}
	if(g_PlayerExtraInfo[iPlayerID].iFlag & SCOREATTRIB_HERO)
	{
		vPlayer[iPlayerID].iHero = 1;
	}
	else vPlayer[iPlayerID].iHero = 0;
	return pmScoreAttrib(pszName, iSize, pbuf);
}

int MsgFunc_HideWeapon(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	Hud().m_iHideHUDDisplay = READ_BYTE();
	return pmHideWeapon(pszName, iSize, pbuf);
}
int MsgFunc_WeaponList(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	char szName[MAX_WEAPON_NAME];
	strcpy(szName, READ_STRING());
	int iAmmoType = READ_CHAR();
	READ_BYTE();
	READ_CHAR();
	READ_BYTE();
	READ_CHAR();
	READ_CHAR();
	int iWeaponID = READ_CHAR();

	g_iWeaponData[iWeaponID].iAmmoType = iAmmoType;
	strcpy(g_iWeaponData[iWeaponID].szName, szName);

	char szValue[128];

	GetPrivateProfileStringA(szName + 7, "AmmoDisplay", "0", szValue, 127, "cstrike/weapons_res.ini");
	g_iWeaponData[iWeaponID].iAmmoDisplay = atoi(szValue);

	GetPrivateProfileStringA(szName + 7, "HideCrosshair", "0", szValue, 127, "cstrike/weapons_res.ini");
	g_iWeaponData[iWeaponID].iHideCrosshair = atoi(szValue);

	GetPrivateProfileStringA(szName + 7, "Crosshair", "0", szValue, 127, "cstrike/weapons_res.ini");
	g_iWeaponData[iWeaponID].iCrosshair = atoi(szValue);

	return pmWeaponList(pszName, iSize, pbuf);
}
int MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iState = READ_BYTE();
	int iId = READ_CHAR();
	int iClip = READ_SHORT();
	if (iClip <= 0)
	{
		BEGIN_READ(pbuf, iSize);
		iState = READ_BYTE();
		iId = READ_CHAR();
		iClip = READ_CHAR();
	}
	g_iWeaponData[iId].iClip = iClip;
	if (iState)
	{
		g_iWeapon = iId;
		HudAmmo().m_fFade = 200.0f;
		/*
		int iLocalIndex = gEngfuncs.GetLocalPlayer()->index;
		if (g_iMod == MOD_ZB3 && vPlayer[iLocalIndex].team == 2)
		{
			if (iId == WEAPON_HEGRENADE)
				TextureManager()->SetZombiBomb(false);
			else if (iId == WEAPON_SMOKEGRENADE)
				TextureManager()->SetZombiBomb(true);
		}
		*/
	}
	return pmCurWeapon(pszName, iSize, pbuf);
}
int MsgFunc_AmmoX(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_SHORT();

	if (iCount <= 0)
	{
		BEGIN_READ(pbuf, iSize);
		iIndex = READ_BYTE();
		iCount = READ_BYTE();
	}

	HudAmmo().m_iAmmo[iIndex] = iCount;
	return pmAmmoX(pszName, iSize, pbuf);
}

/*int MsgFunc_ScreenFade(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int duration = READ_SHORT();
	int holdtime = READ_SHORT();
	int fadetype = READ_SHORT();
	if(duration==(1<<12)*2 && holdtime==(1<<10)*10 && fadetype== 0x0004)
		HudZombieMod3().m_iNVG = 1;
	else
		HudZombieMod3().m_iNVG = 0;

	return pmScreenFade(pszName, iSize, pbuf);
}*/
int MsgFunc_ScoreInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf,iSize);
	int iFrag[33];
	int idx = READ_BYTE();
	HudGunDeathBoard().m_iFrag[idx] = READ_SHORT();
	g_PlayerExtraInfo[idx].frags= HudGunDeathBoard().m_iFrag[idx];
	g_PlayerExtraInfo[idx].deaths = READ_SHORT();
	READ_SHORT();
	READ_SHORT();

	for(int jj=1;jj<33;jj++)
	{
		iFrag[jj] = HudGunDeathBoard().m_iFrag[jj];
	}
	int  i,j,temp,n=33;
	for(i=1;i<n-1;i++) 
	{
		for(j=i+1;j<n;j++)
		{
			if(iFrag[i]<iFrag[j])
			{
				temp=iFrag[i]; 
				iFrag[i]=iFrag[j]; 
				iFrag[j]=temp; 
			}
		}
	}
	int iRevLast=0;
	for(i=0;i<3;i++)
	{
		for(int k=1 ;k<33;k++)
		{
			if(HudGunDeathBoard().m_iFrag[k] == iFrag[i+1]&&iRevLast!=k)
			{
				HudGunDeathBoard().m_iRank[i] = k;
				iRevLast=k;
				break;
			}
		}
	}
	return pmScoreInfo(pszName, iSize, pbuf);
}
int MsgFunc_TeamInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf,iSize);
	int px = READ_BYTE();
	char * teamtext = READ_STRING();
	if (!teamtext) { vPlayer[px].team = 0; }
	else
	{
		if(!strcmp(teamtext,"CT"))
			vPlayer[px].team = 1;
		else if(!strcmp(teamtext,"TERRORIST"))
			vPlayer[px].team = 2;
		else if(!strcmp(teamtext,"SPECTATOR"))
			vPlayer[px].team = 3;
		else
			vPlayer[px].team = 0;
	}
	if(px == gEngfuncs.GetLocalPlayer()->index)
	{
		g_iTeam = vPlayer[px].team;
		
		HudZombieMod3().CheckTeam(vPlayer[px].team);
		HudZombieMod4().m_ScoreBoard.SetTeam(g_iTeam);
		HudZombieMod4().m_Icon.SetTeam(g_iTeam);
	}

	return pmTeamInfo(pszName, iSize, pbuf);
}
int MsgFunc_Money(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf,iSize);
	g_iMoney = READ_LONG();
	BTEPanel_BuyMenu_SetMoney();
	return pmMoney(pszName, iSize, pbuf);
}
int MsgFunc_MetaHook(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	char szFunction[2048];
	strcpy(szFunction, READ_STRING());

	if (!strcmp(szFunction, "DrawImage"))
	{
		DrawImageItem rgTempDrawImage;
		rgTempDrawImage.iFunction = 0;
		rgTempDrawImage.iCheck = READ_BYTE();
		rgTempDrawImage.iCenter = READ_BYTE();

		char szSprites[2048];
		if(rgTempDrawImage.iCheck>0)
		{
			sprintf(szSprites, "%s",READ_STRING());
			rgTempDrawImage.iSprIndex = Hud().GetSpriteIndex(szSprites);
		}
		else
		{
			sprintf(szSprites, "sprites/%s.spr", READ_STRING());
			rgTempDrawImage.hl_hSprites = gEngfuncs.pfnSPR_Load(szSprites);
		}

		rgTempDrawImage.x = READ_COORD() / 100 * ScreenWidth;
		rgTempDrawImage.y = READ_COORD() / 100 * ScreenHeight;
		rgTempDrawImage.color.r = READ_BYTE();
		rgTempDrawImage.color.g = READ_BYTE();
		rgTempDrawImage.color.b = READ_BYTE();
		rgTempDrawImage.iMode = READ_BYTE();
		rgTempDrawImage.flStartDisplayTime = Hud().m_flTime;
		rgTempDrawImage.flEndDisplayTime = READ_COORD() / 10 + Hud().m_flTime;
		rgTempDrawImage.iChanne = READ_BYTE();
		rgTempDrawImage.iLength = READ_SHORT();
		HudSPRElements().AddElement(rgTempDrawImage);
	}
	else if (!strcmp(szFunction, "DrawAdditiveImage"))
	{
		DrawImageItem rgTempDrawImage;
		rgTempDrawImage.iFunction = 1;
		rgTempDrawImage.iCheck = READ_BYTE();
		rgTempDrawImage.iCenter = READ_BYTE();

		char szSprites[2048];
		if(rgTempDrawImage.iCheck>0)
		{
			sprintf(szSprites, "%s",READ_STRING());
			rgTempDrawImage.iSprIndex = Hud().GetSpriteIndex(szSprites);
		}
		else
		{
			sprintf(szSprites, "sprites/%s.spr", READ_STRING());
			rgTempDrawImage.hl_hSprites = gEngfuncs.pfnSPR_Load(szSprites);
		}
		rgTempDrawImage.x = READ_COORD() / 100 * ScreenWidth;
		rgTempDrawImage.y = READ_COORD() / 100 * ScreenHeight;
		rgTempDrawImage.color.r = READ_BYTE();
		rgTempDrawImage.color.g = READ_BYTE();
		rgTempDrawImage.color.b = READ_BYTE();
		rgTempDrawImage.iMode = READ_BYTE();
		rgTempDrawImage.flStartDisplayTime = Hud().m_flTime;
		rgTempDrawImage.flEndDisplayTime = READ_COORD() / 10 + Hud().m_flTime;
		rgTempDrawImage.iChanne = READ_BYTE();
		rgTempDrawImage.iLength = READ_SHORT();
		if(strstr("zbzbkill",szSprites)) return 1;
		if(strstr(szSprites,"boss")) return 1;

		HudSPRElements().AddElement(rgTempDrawImage);
	}
	else if (!strcmp(szFunction, "DrawHolesImage"))
	{
		DrawImageItem rgTempDrawImage;
		rgTempDrawImage.iFunction = 2;
		rgTempDrawImage.iCheck = READ_BYTE();
		rgTempDrawImage.iCenter = READ_BYTE();

		char szSprites[2048];
		if(rgTempDrawImage.iCheck>0)
		{
			sprintf(szSprites, "%s",READ_STRING());
			rgTempDrawImage.iSprIndex = Hud().GetSpriteIndex(szSprites);
		}
		else
		{
			sprintf(szSprites, "sprites/%s.spr", READ_STRING());
			rgTempDrawImage.hl_hSprites = gEngfuncs.pfnSPR_Load(szSprites);
		}

		rgTempDrawImage.x = READ_COORD() / 100 * ScreenWidth;
		rgTempDrawImage.y = READ_COORD() / 100 * ScreenHeight;
		rgTempDrawImage.color.r = READ_BYTE();
		rgTempDrawImage.color.g = READ_BYTE();
		rgTempDrawImage.color.b = READ_BYTE();
		rgTempDrawImage.iMode = READ_BYTE();
		rgTempDrawImage.flStartDisplayTime = Hud().m_flTime;
		rgTempDrawImage.flEndDisplayTime = READ_COORD() / 10 + Hud().m_flTime;
		rgTempDrawImage.iChanne = READ_BYTE();
		if(strstr(szSprites,"boss")) return 1;
		HudSPRElements().AddElement(rgTempDrawImage);
	}
	else if (!strcmp(szFunction, "DrawFontText"))
	{
		static int iContinueRead;
		static struct DrawFontTextItem rgTempDrawText;
		char szBuf[200];
		strcpy(szBuf, READ_STRING());
		rgTempDrawText.iCenter = READ_BYTE();
		rgTempDrawText.x = READ_COORD() / 100 * ScreenWidth;
		rgTempDrawText.y = READ_COORD() / 100 * ScreenHeight;
		rgTempDrawText.color.r = READ_BYTE();
		rgTempDrawText.color.g = READ_BYTE();
		rgTempDrawText.color.b = READ_BYTE();
		rgTempDrawText.iScale = READ_BYTE();
		rgTempDrawText.flDisplayTime = READ_COORD() / 10 + Hud().m_flTime;
		rgTempDrawText.flStartTime = Hud().m_flTime;
		rgTempDrawText.fFadeTime = READ_COORD() / 10 ;
		rgTempDrawText.iFillBg = READ_BYTE();
		rgTempDrawText.iChanne = READ_BYTE();
		rgTempDrawText.flDisplayTime += rgTempDrawText.fFadeTime*2;
		iContinueRead = READ_BYTE();
		if(iContinueRead)
		{
			sprintf(g_szFontBuf,"%s%s",g_szFontBuf,szBuf);
		}
		else
		{
			sprintf(g_szFontBuf,"%s%s",g_szFontBuf,szBuf);
			rgTempDrawText.szText[0] = '\0';
			sprintf(rgTempDrawText.szText,"%s",g_szFontBuf);
			Hud().m_FontText.AddElement(rgTempDrawText);
			g_szFontBuf[0] = '\0';
		}
	}
	else if(!strcmp(szFunction, "DrawTargaImage"))
	{
		DrawTgaItem rgTempTga;
		strcpy(rgTempTga.szName, READ_STRING());
		rgTempTga.iFunction = READ_BYTE();
		rgTempTga.iCenter = READ_BYTE();
		rgTempTga.r = READ_BYTE();
		rgTempTga.g = READ_BYTE();
		rgTempTga.b = READ_BYTE();
		rgTempTga.x = READ_COORD() / 100 * ScreenWidth;
		rgTempTga.y = READ_COORD() / 100 * ScreenHeight;
		rgTempTga.w = 0;
		rgTempTga.h = 0;
		rgTempTga.iMode = READ_BYTE();
		rgTempTga.iChanne = READ_SHORT();
		rgTempTga.flStartDisplayTime = Hud().m_flTime;
		rgTempTga.flEndDisplayTime = READ_COORD() / 10 + Hud().m_flTime;
		rgTempTga.fScale = 1.0;
		Hud().m_TGA.AddElement(rgTempTga);
	}
	/*else if (!strcmp(szFunction, "ModRunning"))
	{
		int iNew = READ_BYTE();
		if (g_iMod != iNew)
		{
			g_iMod = iNew;
			BTEPanel_BuyMenu_Reset();
		}
	}
	else if (!strcmp(szFunction, "WeaponLimit"))
	{
		g_iWeaponLimit = READ_BYTE();
	}*/
	else if (!strcmp(szFunction, "TeamSuit"))
	{
		int iPackage = READ_BYTE();
		char szToken[256];
		sprintf(szToken,"%s",READ_STRING());
		//LogToFile(szToken);
		char szWpn[3][16];
		int iIndex = 0;
		int iPos = 0;
		for(size_t i=0 ; i<strlen(szToken);i++)
		{
			if(szToken[i] == '#')
			{
				szWpn[iIndex][iPos] = '\0';
				iIndex ++;
				iPos = 0;
				continue;
			}
			szWpn[iIndex][iPos] = szToken[i];
			iPos ++;
		}
		//BTEPanel_TeamSuit_UpdateItems(iPackage,szWpn[0],szWpn[1],szWpn[2]);
		BTEPanel_TeamSuit_Show();
		g_mgui_candraw = 1;
	}
#if 0
	else if (!strcmp(szFunction, "BuyMenu"))
	{
		cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();
		int iLocalIndex = pPlayer->index;

		if((g_iMod == MOD_ZB || g_iMod == MOD_ZB2 || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4) && vPlayer[iLocalIndex].team != 1) return 1;
		
		if((HudHealth().m_iHealth > 0 || g_iMod == MOD_TDM || g_iMod == MOD_DM))
		{
			char szToken[256];
			int iType = READ_BYTE();
			int iPage = READ_BYTE();
			int iHaveNextPage = READ_BYTE();
			sprintf(szToken,"%s",READ_STRING());
			if(szToken[0] == '\0')
			{			
				BTEPanel_BuyMenu_UpdateItems(-1,"");
				BTEPanel_BuyMenu_SetPage(0,0);
				//BTEPanel_BuyMenu_SetHaveNextPage(0,0);
			}
			else
			{
				char szWpn[9][32];
				int iIndex = 0;
				int iPos = 0;

				BTEPanel_BuyMenu_SetPage(iPage,iType);
				BTEPanel_BuyMenu_SetHaveNextPage(iPage>0,iHaveNextPage);

				for(int i=0 ; i<strlen(szToken);i++)
				{
					if(szToken[i] == '#')
					{
						szWpn[iIndex][iPos] = '\0';
						iIndex ++;
						iPos = 0;
						continue;
					}
					szWpn[iIndex][iPos] = szToken[i];
					iPos ++;
				}
				//szWpn[iIndex][iPos] = '\0';
				for(int i=0;i<=9;i++)
					BTEPanel_BuyMenu_UpdateItems(i,szWpn[i]);
			}
			g_mgui_candraw = 1;
		}
		
	}
#endif
	else if (!strcmp(szFunction, "ZombieMenu"))
	{
		// Backward support, NOT used any longer
		char szToken[512];
		sprintf(szToken,"%s",READ_STRING());

		char szZombie[14][32];
		memset(szZombie,0,sizeof(szZombie));
		int iIndex = 0;
		int iPos = 0;
		int iCount = 0;

		for(size_t i=0 ; i<strlen(szToken);i++)
		{
			if(szToken[i] == '#')
			{
				szZombie[iIndex][iPos] = '\0';
				iIndex ++;
				iCount++;
				iPos = 0;
				continue;
			}
			szZombie[iIndex][iPos] = szToken[i];
			iPos ++;
		}

		HudZBSelect().SetCount(iCount);
		
		for (int i = 0; i<iIndex; i++)
		{
			if(szZombie[i][0]!=0)
			{
				HudZBSelect().AddZombie(szZombie[i]);
			}
		}

		HudZBSelect().SetDrawTime(cl.time);
		
	}
	else if (!strcmp(szFunction, "SpecialEvent"))
	{
		enum Event
		{
			EVENT_DR_START = 2,
			EVENT_DR_END,
			EVENT_DR_RESET,
			EVENT_BALROG,
			EVENT_SFMG,
			EVENT_SFMG2,
			EVENT_INF,
			EVENT_INF2,
			EVENT_BL3,
			EVENT_BL32,
			EVENT_M16,
			EVENT_M162,
		};

		int iEvent = READ_BYTE();
		/*if(iEvent == EVENT_BALROG)
		{
			TEMPENTITY *pEnt;
			struct model_s *pModel;
			cl_entity_s* pEntity = gEngfuncs.GetViewModel();
			pModel = IEngineStudio.Mod_ForName( "sprites/custommuzzleflash/muz_balrog.spr", 0);
			pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh((float *)&pEntity->attachment[1], pModel);
			pEnt->entity.curstate.rendermode = kRenderTransAdd;
			pEnt->entity.curstate.renderamt = 200;
			pEnt->entity.curstate.scale = 0.2;
			pEnt->entity.curstate.frame = (float)gEngfuncs.pfnRandomLong(0,1);
			pEnt->die += Hud().m_flTime + 0.01f;
			pEnt->entity.curstate.iuser4 = 1;
			Engfunc_Call_AddVisibleEntity( &(pEnt->entity));
			
		}
		else */if(iEvent == EVENT_DR_START)
		{
			HudDeathRunUI().Start();
		}
		else if(iEvent == EVENT_DR_END)
		{
			HudDeathRunUI().Stop();
		}
		else if(iEvent == EVENT_DR_RESET)
		{
			HudDeathRunUI().ResetUI();
		}
		else if(iEvent == EVENT_SFMG || iEvent == EVENT_SFMG2)
		{
			g_iWeaponStat = iEvent;
		}
		else if(iEvent == EVENT_BL3 || iEvent == EVENT_BL32)
		{
			g_iBl3Muzz = iEvent;
		}
		else if(iEvent == EVENT_M16 || iEvent == EVENT_M162)
		{
			g_iBl3Muzz = iEvent;
		}
		else
			g_iWeaponStat = iEvent;
	}
	else if (!strcmp(szFunction, "DRRank"))
	{
		HudDeathRunUI().ParseMessage();
	}
	else if (!strcmp(szFunction, "DrawExtraAmmo"))
	{
		int iAmount = READ_BYTE();
		//if (HudAmmo().m_iExtraAmmo != iAmount)
		//{
		if (HudAmmo().m_iExtraAmmo != iAmount)
			HudAmmo().m_fFade = 200.0f;
		HudAmmo().m_iExtraAmmo = iAmount;
		//}
	}
	else if (!strcmp(szFunction, "DrawScoreBoard"))
	{
		HudScoreBoard().m_iLeft = READ_SHORT();
		HudScoreBoard().m_iCenter = READ_SHORT();
		HudScoreBoard().m_iRight = READ_SHORT();
		HudScoreBoard().m_iLeft2 = READ_SHORT();
		HudScoreBoard().m_iRight2 = READ_BYTE();
		HudScoreBoard().m_iMode = READ_BYTE();
	}
	else if (!strcmp(szFunction, "SetViewEntityBody"))
	{
		g_iViewEntityBody = READ_BYTE();
	}
	else if (!strcmp(szFunction, "SetViewEntityRender"))
	{
		g_iViewEntityRenderMode = READ_BYTE();
		g_iViewEntityRenderFX = READ_BYTE();
		g_iViewEntityRenderAmout = READ_BYTE();
		g_byViewEntityRenderColor.r = READ_BYTE();
		g_byViewEntityRenderColor.g = READ_BYTE();
		g_byViewEntityRenderColor.b = READ_BYTE();
	}
	else if (!strcmp(szFunction, "ChangeTattoo"))
	{
		// NO MORE USE
		/*
		char name[32];
		int iType;
		sprintf(name,"%s",READ_STRING());
		if(name[0] != 0)
		{
			iType = READ_BYTE();
			if(iType == 1)
			{
				sprintf(g_sTattoo,"%s",name);
			}
			else 
			{
				sprintf(g_sTattooSmall,"%s",name);
				g_iTattooSmallEnable = 1;
			}
			LoadTattoo();
		}*/
	}
	else if (!strcmp(szFunction, "PlayBink"))
	{
		char szBink[64];
		sprintf(szBink, "metahook\\bik\\%s",READ_STRING());
		int iX = READ_COORD() / 100 * ScreenWidth;
		int iY = READ_COORD() / 100 * ScreenHeight;
		READ_BYTE();
		READ_BYTE();
		READ_BYTE();
		READ_BYTE();
		READ_BYTE();
		HudBinkPlayer().SetPos(iX, iY, READ_BYTE());
		HudBinkPlayer().SetColor(255, 255, 255);

		HudBinkPlayer().BinkInstance(szBink,0,0);
	}
	else if(!strcmp(szFunction,"RespawnBar"))
	{
		HudRespawnBar().SetStatus(READ_BYTE());
		HudRespawnBar().SetTime(READ_COORD() / 10 + Hud().m_flTime);
		HudRespawnBar().SetStartTime(Hud().m_flTime);
	}
	else if (!strcmp(szFunction, "Quest"))
	{
		char sNpcName[32];
		char sNpcPic[32];
		char sMission[32];
		sprintf(sNpcName,"CSBTE_QUEST_NPC_NAME%d",READ_BYTE());
		sprintf(sNpcPic, "resource\\hud\\quest\\quest_npc%d",READ_BYTE());
		sprintf(sMission, "CSBTE_QUEST_MISSION%d",READ_BYTE());
		HudQuest().Add(sNpcName, sNpcPic, sMission);
	}
	else if(!strcmp(szFunction,"DrawCountDown"))
	{
		sprintf(g_sCountDownText,"%s",READ_STRING());
		g_fCountDown = float(READ_BYTE())+ Hud().m_flTime;
	}
	else if(!strcmp(szFunction,"DrawCountDownReset"))
	{
		g_fCountDown = 0;
		HudZombieEscape().ResetUI();
	}
	else if(!strcmp(szFunction,"WeaponSkin"))
	{
		int ID = READ_BYTE();
		sprintf(g_szWeaponSkin[ID],"%s",READ_STRING());
		int iSer = READ_BYTE();
		//int iSer2 = 1-iSer;
		//int iSer3 = 0-iSer;
		//g_iWeaponSkin[ID][iSer] = 1; // 皮肤3
		//g_iWeaponSkin[ID][iSer2] = 0; // 皮肤2
		//g_iWeaponSkin[ID][iSer3] = 2; // 皮肤1
		g_iWeaponSkin[ID][1] = iSer;
	}
	else if (!strcmp(szFunction, "DrawFollowIcon"))
	{
		DrawIconItem rgTemp;
		char szSprites[2048];
		sprintf(szSprites, "%s",READ_STRING());
		if(strstr(szSprites,"djb_spboxicon"))
		{
			rgTemp.iBox = 1;
		}
		else if(strstr(szSprites,"bte_bombtargetA"))
		{
			rgTemp.iBox = 2;
		}
			else if(strstr(szSprites,"bte_bombtargetB"))
		{
			rgTemp.iBox = 3;
		}
		else
		{
			rgTemp.iBox = 0;
			rgTemp.iSprIndex = Hud().GetSpriteIndex(szSprites);
		}
		rgTemp.x = READ_SHORT();
		rgTemp.y = READ_SHORT();
		rgTemp.z = READ_SHORT();
		rgTemp.iToggle = READ_BYTE();
		rgTemp.iDistance = READ_BYTE();
		rgTemp.iChannel = READ_BYTE();
		rgTemp.r = READ_BYTE();
		rgTemp.g = READ_BYTE();
		rgTemp.b = READ_BYTE();
		Hud().m_FollowIcon.Add(rgTemp);
	}
	else if (!strcmp(szFunction, "DrawFollowIconReset"))
	{
		Hud().m_FollowIcon.VidInit();
	}
	else if (!strcmp(szFunction, "ZB3Data"))
	{
		int iDataType = READ_BYTE();
		int iData = READ_BYTE();
		if(iDataType !=5)
		{
			HudZombieMod3().ParseData(iDataType, iData);
		}
		else
		{
			if (HudAmmo().m_iExtraAmmo != iData)
				HudAmmo().m_fFade = 200.0f;
			HudAmmo().m_iExtraAmmo = iData;
		}
	}
	else if (!strcmp(szFunction, "ZB3UI"))
	{
		char skull[128];
		sprintf(skull,"%s",READ_STRING());
		int iSlot = READ_BYTE();
		int iType = READ_BYTE();
		int iData = READ_BYTE();
		HudZombieMod3().ParseIcon(skull,iSlot,iType,iData);
	}
	else if (!strcmp(szFunction, "ZBSTeamScore"))
	{
		int score = READ_LONG();

		HudZBSBoard().SetTeamScore(score);
	}
	else if (!strcmp(szFunction, "ZBSPlayerScore"))
	{
		int score = READ_LONG();
		int flash = READ_BYTE();

		HudZBSBoard().SetPlayerScore(score, flash);
	}
	else if (!strcmp(szFunction, "ZBSRoundNums"))
	{
		int nums = READ_LONG();

		HudZBSBoard().SetRoundNums(nums);
	}
	else if (!strcmp(szFunction, "DrawRetina"))
	{
		char szText[128];
		sprintf(szText,"%s",READ_STRING());
		//gEngfuncs.Con_Printf(szText);
		int id = Hud().m_TGA.GetTGA(szText, false);
		if (!id)
			Hud().m_TGA.GetTGA(szText, true);
		int draw = READ_BYTE();
		int full = READ_BYTE();
		int flash = READ_BYTE();
		int channe = READ_SHORT();
		float time = READ_COORD() / 10 + Hud().m_flTime;
		HudRetina().ParseData(szText, draw, full, flash, channe, time);
	}
	else if (!strcmp(szFunction, "SendData"))
	{
		int type = READ_BYTE();
		int data = READ_LONG();
		char data2[128];
		sprintf(data2,"%s",READ_STRING());
		switch (type)
		{
			case 1: 
			{
				g_iBuyTime = data + (int)Hud().m_flTime;
				break;
			}
			case 2:
			{
				BTEPanel_BuyMenu_UpdateItems(-1,"");
				BTEPanel_BuyMenu_SetPage(0,0);
				BTEPanel_BuyMenu_SetQuickBuy(data,data2);
				break;
			}
		}
		/*if(type > 10)
		{
			int id = type - 10;
			int iSlot = data;

			sprintf(g_szWeapon[id][iSlot], "%s", data2);
			g_iCustomWeapon[id][iSlot] = FindWeaponData(data2);
		}*/


	}

	return 1;
}
int MsgFunc_NVGToggle(const char *pszName, int iSize, void *pbuf)
{
	//BEGIN_READ(pbuf, iSize);
	
	//if(READ_BYTE())
		//HudAmmo().SetNvgOn(true);
	//else
		//HudAmmo().SetNvgOn(false);

	//return pmNVGToggle(pszName, iSize, pbuf);
	return HudNVG().MsgFunc_NVGToggle(pszName, iSize, pbuf);
}

void Cmd_Buy_Advanced(void);
extern void EV_CreateBloodhunterEffect();

int MsgFunc_MetaHook2(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iMsgType = READ_BYTE();
	
	switch (iMsgType)
	{
	case 0:
		{
			int iNew = READ_BYTE();
			if (g_iMod != iNew)
			{
				g_iMod = iNew;
				BTEPanel_BuyMenu_Reset();
			}			

			if(g_iMod == MOD_ZB4)
			{
				Hud().m_HUD_number_0 = Hud().GetSpriteIndex("z4_number_0");
			}
			break;
		}
	case 1:
		{
			int a = READ_BYTE();
			int b = READ_BYTE();
			
			HudZombieMod4().m_ScoreBoard.SetDamage(a * 255 + b);
			break;
		}
	case 2:
		{
			int power = READ_BYTE();
			float flashtime = READ_BYTE() / 10.0f;
			HudZombieMod4().m_Power.SetPower(power, flashtime);
			HudZombieMod4().m_Icon.SetPower(power);
			break;
		}
	case 4:
		{
			int slot = READ_BYTE();
			int id = READ_BYTE();
			int status = READ_BYTE();
			HudZombieMod4().m_Icon.AddIcon(slot, id, status);
			break;
		}
	case 5:
		{
			int start = READ_BYTE();
			int end = READ_BYTE();
			HudZombieMod4().m_Tip.SetNightTime(start, end);
			break;
		}
	case 6:
		{
			int id = READ_BYTE();
			int damage = READ_BYTE();
			vPlayer[id].iDamage = damage;
			break;
		}
	case 7:
		{
			int Nvg = READ_BYTE();			
			
			if(Nvg >= 5)
			{
				if(Nvg == 5)
				{
					memset(g_vecVAngle, 0, sizeof(g_vecVAngle));
					g_flZ4Nvg2 = Hud().m_flTime + 4.0;
				}
				g_iZ4Nvg2 = Nvg;
			}
			else
			{
				g_iZ4Nvg = Nvg;
			}			

			break;
		}
	case 3:
		{
			int type = READ_BYTE();
			switch (type)
			{
			case 0: // accshoot开始
				{
					HudZombieMod4().m_Icon.AddIcon(1, 1, 1);
					HudZombieMod4().m_Retina[1].SetCanDraw(true);
					HudZombieMod4().m_Retina[1].SetEndTime(Hud().m_flTime + 9999.0);
					break;
				}
			case 1: // accshoot结束
				{
					HudZombieMod4().m_Icon.AddIcon(1, 1, 3);
					HudZombieMod4().m_Retina[1].SetCanDraw(false);
					break;
				}
			case 2: // 变成僵尸
				{
					HudZombieMod4().m_Retina[0].SetCanDraw(false);
					HudZombieMod4().m_Retina[1].SetCanDraw(false);
					
					HudZombieMod4().m_Bink.Play(false, Hud().m_flTime);
					HudZombieMod4().m_Icon.AddIcon(0, 0, 0);
					HudZombieMod4().m_Tip.ZombieIconsCheck(Hud().m_flTime);
					HudZombieMod3().m_iNVG = 1;
					g_iZ4Nvg = 1;
					/*
					HudZBSelect().SetIcon(0, "z4normal");
					HudZBSelect().SetIcon(1, "z4light");
					HudZBSelect().SetIcon(2, "z4heavy");
					HudZBSelect().SetIcon(3, "z4hide");
					HudZBSelect().SetIcon(4, "z4humpback");
					HudZBSelect().SetDrawTime(Hud().m_flTime);*/
					/*MSG_BeginWrite("ScreenFade");
					MSG_WriteShort((1<<12)*2); // duration
					MSG_WriteShort((1<<10)*10); // hold time
					MSG_WriteShort(0x0004); // fade type
					MSG_WriteByte(85); // red
					MSG_WriteByte(70); // green
					MSG_WriteByte(134); // blue
					MSG_WriteByte(80); // alpha
					MSG_EndWrite();*/
					break;
				}
			case 3: // speedup开始
				{
					HudZombieMod4().m_Icon.AddIcon(7, 4, 1);
					HudZombieMod4().m_Retina[0].SetCanDraw(true);
					HudZombieMod4().m_Retina[0].SetEndTime(Hud().m_flTime + 9999.0);
					break;
				}
			case 4: // speedup结束
				{
					HudZombieMod4().m_Icon.AddIcon(7, 4, 3);
					HudZombieMod4().m_Retina[0].SetCanDraw(false);
					break;
				}
			case 5: // zb dash开始
				{
					HudZombieMod4().m_Icon.AddIcon(7, 4, 1);
					HudZombieMod4().m_Retina[3].SetCanDraw(true);
					HudZombieMod4().m_Retina[3].SetEndTime(Hud().m_flTime + 9999.0);
					break;
				}
			case 6: // zb dash结束
				{
					HudZombieMod4().m_Icon.AddIcon(7, 4, 3);
					HudZombieMod4().m_Retina[3].SetCanDraw(false);
					break;
				}
			case 7: // zb hide开始
				{
					HudZombieMod4().m_Icon.AddIcon(7, 4, 1);
					HudZombieMod4().m_Icon.AddIcon(9, 8, 1);
					HudZombieMod4().m_Retina[4].SetCanDraw(true);
					HudZombieMod4().m_Retina[4].SetEndTime(Hud().m_flTime + 9999.0);

					/*g_iViewEntityRenderMode = 5;
					g_iViewEntityRenderFX = 0;
					g_iViewEntityRenderAmout = 100;
					g_byViewEntityRenderColor.r = 100;
					g_byViewEntityRenderColor.g = 100;
					g_byViewEntityRenderColor.b = 100;*/

					//g_iSetupSkin = 1;

					break;
				}
			case 8: // zb hide结束
				{
					HudZombieMod4().m_Icon.AddIcon(7, 4, 3);
					HudZombieMod4().m_Icon.AddIcon(9, 8, 3);
					HudZombieMod4().m_Retina[4].SetCanDraw(false);

					/*g_iViewEntityRenderMode = 0;
					g_iViewEntityRenderFX = 0;
					g_iViewEntityRenderAmout = 255;
					g_byViewEntityRenderColor.r = 255;
					g_byViewEntityRenderColor.g = 255;
					g_byViewEntityRenderColor.b = 255;*/

					//g_iSetupSkin = 0;

					break;
				}
			case 9: // zb heavy dash开始
				{
					HudZombieMod4().m_Icon.AddIcon(7, 5, 1);
					HudZombieMod4().m_Icon.AddIcon(9, 7, 1);
					HudZombieMod4().m_Retina[5].SetCanDraw(true);
					HudZombieMod4().m_Retina[5].SetEndTime(Hud().m_flTime + 9999.0);
					break;
				}
			case 10: // zb heavy dash结束
				{
					HudZombieMod4().m_Icon.AddIcon(7, 5, 3);
					HudZombieMod4().m_Icon.AddIcon(9, 7, 3);
					HudZombieMod4().m_Retina[5].SetCanDraw(false);
					break;
				}
			case 11: // zb humpback dash开始
				{
					HudZombieMod4().m_Icon.AddIcon(7, 4, 1);
					HudZombieMod4().m_Icon.AddIcon(9, 7, 1);
					HudZombieMod4().m_Retina[3].SetCanDraw(true);
					HudZombieMod4().m_Retina[3].SetEndTime(Hud().m_flTime + 9999.0);
					break;
				}
			case 12: // zb humpback dash结束
				{
					HudZombieMod4().m_Icon.AddIcon(7, 4, 3);
					HudZombieMod4().m_Icon.AddIcon(9, 7, 3);
					HudZombieMod4().m_Retina[3].SetCanDraw(false);
					break;
				}
			}
			break;
		}
		case 8:
		{
			// round start
			gHud3D.ResetHPAP(0.5f);

			g_fCountDown = 0;
			HudZombieEscape().ResetUI();

			int time = READ_BYTE();
			g_iBuyTime = Hud().m_flTime + time;

			if (g_iMod == MOD_ZB5)
			{ 
				gHud3D_ZB.Reset();
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_2", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_3", 3, 2, 3);
				if (g_bAlive)
					UTIL_TutorText("#CSBTE_Totur_ZB5_RoundStart", 1 << 0, 5.0);
			}

			if (g_iMod == MOD_ZB3)
			{
				gHud3D_ZB.Reset();
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, 20);

			}
			break;
		}

		case 9:
		{
			int type = READ_BYTE();
			switch (type)
			{
			case 0: // 变成僵尸
				{
					HudZombieMod3().m_iNVG = 1;
					
					break;
				}
			}
			break;
		}

		case 10:
		{
			HudZSEUI().SendData();
			break;
		}
		case 11:
			break;
		case 12: // z3 accshoot
		{
			int local = gEngfuncs.GetLocalPlayer()->index;
			int id = READ_BYTE();
			//HudZombieMod3().ParseData(13 + 1, id);
			HudZombieMod3().AddHeadIcon(id, "sprites/zb_skill_headshot.spr", 4.0f, 0.0);

			if (id == local)
			{
				HudZombieMod3().ParseData(ZB3DATA_RETINA, 7);
				HudZombieMod3().ParseIcon("resource\\zombi\\humanskill_hm_hd", 2, 5, 0);
				//HudRetina().ParseData("resource\\zombi\\damagedouble", 1, 0, 1, 0, 5.0 + Hud().m_flTime);
			}
			break;
		}
		case 13: // z3 speedup
		{
			//HudZombieMod3().ParseData(6, 7);
			int local = gEngfuncs.GetLocalPlayer()->index;

			
			HudZombieMod3().ParseIcon("resource\\zombi\\humanskill_hm_spd", 1, 5, 0);
			HudRetina().ParseData("resource\\zombi\\zombispeedup", 1, 0, 1, 1, 10.0 + Hud().m_flTime);

			break;
		}
		case 14: // z3 stop
		{
			int iSkillType = READ_BYTE();
			HudZombieMod3().ParseData(ZB3DATA_RETINASTOP, 1);
			/*
			if (iSkillType)
				HudZombieMod3().ParseIcon("resource\\zombi\\humanskill_hm_spd", 1, 4, 0);
			else
				HudZombieMod3().ParseIcon("resource\\zombi\\humanskill_hm_hd", 2, 4, 0);*/
			
			break;
		}

		case 15: // z5 hm skill 1 use
		{
			gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1", 1, 3, 20.0F);

			break;
		}

		case 16: // z5 hm skill 2 use
		{
			gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_2", 2, 4);

			break;
		}

		case 17: // z5 hm skill 3 use
		{
			gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_3", 3, 4);

			break;
		}

		case 18: // tank zb skill init
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombicrazy", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1", 3, -1);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, TANK_ZB);
			}

			break;
		}

		case 19: // tank zb skill use
		{
			float cd = READ_BYTE();
			float time = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombicrazy", 1, 3, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_zombicrazy", 1, 3, cd);

			HudRetina().ParseData("resource\\zombi\\zombicrazy", 1, 1, 1, 1, time + Hud().m_flTime);

			break;
		}

		case 20: // speed zb skill init
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombihiding", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1", 3, -1);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, SPEED_ZB);
			}

			break;
		}

		case 21: // speed zb skill use
		{
			float cd = READ_BYTE();
			float time = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombihiding", 1, 3, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_zombihiding", 1, 3, cd);

			HudRetina().ParseData("resource\\zombi\\zombihiding", 1, 0, 1, 1, time + Hud().m_flTime);

			break;
		}

		case 22: // heavy zb skill init
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombitrap", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1.tga", 3, -1);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, HEAVY_ZB);
			}

			break;
		}

		case 23: // heavy zb skill use
		{
			float cd = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombitrap", 1, 3, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_zombitrap", 1, 3, cd);

			//HudRetina().ParseData(Hud().m_TGA.GetTGA(resource\\zombi\\zombitrap"), 1, 0, 1, 1, 1.0 + Hud().m_flTime);
			HudRetina().ParseData("resource\\zombi\\stone", 1, 1, 1, 1, 1.0 + Hud().m_flTime);
			break;
		}

		case 24: // heal zb skill init
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombiheal", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1.tga", 3, -1);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, HEAL_ZB);
			}

			break;
		}

		case 25: // heal zb skill use
		{
			float cd = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombiheal", 1, 3, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_zombiheal", 1, 3, cd);

			//HudRetina().ParseData(Hud().m_TGA.GetTGA("resource\\zombi\\zombiheal"), 1, 1, 0, 2, time);

			break;
		}

		case 26: // deimos zb skill init
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_tentacle", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1.tga", 3, -1);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, DEIMOS_ZB);
			}

			break;
		}

		case 27: // deimos zb skill use
		{
			float cd = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_tentacle", 1, 3, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_tentacle", 1, 3, cd);

			HudRetina().ParseData("resource\\zombi\\zombitentacle", 1, 0, 0, 1, 1.5 + Hud().m_flTime);

			break;
		}

		case 28: // china zb skill init
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombistiffen", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1.tga", 3, -1);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, CHINA_ZB);
			}

			break;
		}

		case 29: // china zb skill use
		{
			float cd = READ_BYTE();
			float time = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombistiffen", 1, 1, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_zombistiffen", 1, 3, cd);

			HudRetina().ParseData("resource\\zombi\\zombicrazy", 1, 1, 1, 1, time + Hud().m_flTime);

			break;
		}

		case 30: // boomer zb skill init
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombiheal", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1.tga", 3, 2);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, BOMMER_ZB);
			}

			break;
		}

		case 31: // boomer zb skill use (heal)
		{
			float cd = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombiheal", 1, 3, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_recoverylast", 1, 3, cd);

			HudRetina().ParseData("resource\\zombi\\zombiheal", 1, 1, 0, 2, 1.5 + Hud().m_flTime);

			break;
		}

		case 32: // boomer zb skill use (defup)
		{
			float cd = READ_BYTE();
			float time = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombiguard", 2, 3, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_zombiguard", 2, 3, cd);

			HudRetina().ParseData("resource\\zombi\\zombitrap", 1, 0, 1, 1, time + Hud().m_flTime);

			break;
		}

		case 33: // boomer zb respawn
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombiheal", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\HM_SKILL_1.tga", 3, 2);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_RESETSKILL, BOMMER_ZB);
			}

			break;
		}

		case 34: // resident zb skill init
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombijumpup", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\zombieskill_zombipenetration.tga", 3, 2, 3);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, RESIDENT_ZB);
			}

			break;
		}

		case 35: // Jumpupm (Previously I am a super skill for resident QAQ)
		{
			float cd = READ_BYTE();
			float time = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_jumpupm", 2, 3, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_jumpupm", 2, 3, cd);

			HudRetina().ParseData("resource\\zombi\\zombijumpup", 1, 0, 1, 1, time + Hud().m_flTime);

			break;
		}

		case 36: // Resident zb skill use (0.0)
		{
			float cd = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombipenetration", 1, 5, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_zombipenetration", 1, 5, 0);

			HudRetina().ParseData("resource\\zombi\\zombitentacle", 1, 0, 1, 1, 2.0 + Hud().m_flTime);

			break;
		}

		case 37: // resident zb respawn
		{
			int iCanUseSkill2 = READ_BYTE();
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombipenetration", 3, 1);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_RESETSKILL, RESIDENT_ZB);
				HudZombieMod3().ParseIcon("resource\\zombi\\zombietype_resident_zombi", 0, iCanUseSkill2 ? 2:5, 3);
			}

			break;
		}

		case 38: // zb5 zb public skill (atkup)
		{
			float cd = READ_BYTE();
			float time = READ_BYTE();

			gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 3, cd);

			break;
		}

		case 39: // TutorText
		{
			char *str = READ_STRING();
			int color = READ_BYTE();
			float time = READ_BYTE() / 10.0;

			UTIL_TutorText(str, color, time, true);
			break;
		}

		case 40: // zombie init
		{
			HudBinkPlayer().SetPos(0.5 * ScreenWidth, 0.5 * ScreenWidth, 1);
			HudBinkPlayer().SetColor(255, 255, 255);
			HudBinkPlayer().BinkInstance("cstrike\\resource\\hud\\zombie\\origin.bik", 0, 0);
			UTIL_TutorText("#CSBTE_Totur_ZB5_FirstZombie", 1 << 1, 5.0, true);
			break;
		}

		case 41: // zombie init
		{
			HudBinkPlayer().SetPos(0.5 * ScreenWidth, 0.5 * ScreenWidth, 1);
			HudBinkPlayer().SetColor(255, 255, 255);
			HudBinkPlayer().BinkInstance("cstrike\\resource\\hud\\zombie\\infection.bik", 0, 0);
			UTIL_TutorText("#CSBTE_Totur_ZB5_Infected", 1 << 1, 5.0, true);

			break;
		}

		case 42:
		{
			Cmd_Buy_Advanced();
			break;
		}

		case 43:
		{
			HudAmmo().HitForBuff(cl.time);
			break;
		}

		case 44:	// undertaker zb skill init
		{
			if (g_iMod == MOD_ZB5)
			{
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombipile", 1, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 2, 2, 3);
				gHud3D_ZB.SetSkill("resource\\hud3d_v2\\zombie\\ZB_SKILL_1", 3, -1);
			}
			else if (g_iMod == MOD_ZB3)
			{
				HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, UNDERTAKER_ZB);
			}

			break;
		}

		case 45:	// undertaker zb skill use
		{
			float cd = READ_BYTE();

			if (g_iMod == MOD_ZB5)
				gHud3D_ZB.SetSkill("resource\\zombi\\zombieskill_zombipile", 1, 3, cd);
			else if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_zombipile", 1, 3, cd);

			//HudRetina().ParseData(Hud().m_TGA.GetTGA("resource\\zombi\\zombitrap"), 1, 0, 1, 1, 1.0 + Hud().m_flTime);
			HudRetina().ParseData("resource\\zombi\\stone", 1, 1, 1, 1, 1.0 + Hud().m_flTime);
			break;
		}

		case 46:	// hm hs cooldown when host owns buffsg552
		{
			HudZombieMod3().ParseData(7, 1);
			HudZombieMod3().ParseIcon("resource\\zombi\\humanskill_hm_hd", 2, 3, READ_BYTE());
			break;
		}

		case 47:
		{
			DrawFontTextItem rgTempDrawText;

			rgTempDrawText.iCenter = 1;
			rgTempDrawText.x = 0.5 * ScreenWidth;
			rgTempDrawText.y = 0.3 * ScreenHeight;
			rgTempDrawText.color.r = 255;
			rgTempDrawText.color.g = 150;
			rgTempDrawText.color.b = 45;
			rgTempDrawText.iScale = 24;
			rgTempDrawText.flDisplayTime = cl.time + READ_BYTE();
			rgTempDrawText.flStartTime = cl.time;
			rgTempDrawText.fFadeTime = 1.0;
			rgTempDrawText.iFillBg = 0;
			rgTempDrawText.iChanne = 3;
			Q_strcpy(rgTempDrawText.szText, GetLangUtf("#CSO_YourCandidateZombie"));

			Hud().m_FontText.AddElement(rgTempDrawText);

			HudZombieMod3().ParseData(ZB3DATA_BECOME_ZB, 21);

			break;
		}
		case 48: // StrengthenRecovery(Only Retina)
		{
			float cd = READ_BYTE();

			if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_strengthrecovery", 2, 3, cd);

			HudRetina().ParseData("resource\\zombi\\zombiheal", 1, 1, 0, 2, 1.5 + Hud().m_flTime);

			break;
		}
		case 49: // ArmorRecovery
		{
			float cd = READ_BYTE();

			if (g_iMod == MOD_ZB3)
				HudZombieMod3().ParseIcon("resource\\zombi\\zombieskill_armorrecovery", 2, 3, cd);

			HudRetina().ParseData("resource\\zombi\\zombitrap", 1, 0, 1, 1, 1.5 + Hud().m_flTime);
			
			break;
		}
		case 50: // Buff DamageBoard
		{
			int a = READ_BYTE();
			int b = READ_BYTE();

			
			if (((a << 8) + b) > 0)
				HudBuffClassDamage().UpdateDamage(a*255+ b);
			else
				HudBuffClassDamage().Reset();
			break;
		}
		case 51:
		{
			int local = gEngfuncs.GetLocalPlayer()->index;
			int id = READ_BYTE();
			HudZombieMod3().ParseData(13 + 1, id);

		}
		case 52:
		{
			
			int iIndex = READ_BYTE();/*
			cl_entity_s * pEntity = gEngfuncs.GetEntityByIndex(iIndex);

			TEMPENTITY *pEffect;
			pEffect = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(pEntity->origin, IEngineStudio.Mod_ForName("models/ef_gundrop.mdl", false));
			pEffect->entity.curstate.frame = 0;
			pEffect->entity.curstate.framerate = 1;
			pEffect->die = cl.time + 9999.0;

			pEffect->callback = Callback_GunDropEffect;

			pEffect->clientIndex = pEntity->index;
			pEffect->flags |= FTENT_PLYRATTACHMENT | FTENT_CLIENTCUSTOM;
			*/
		}
	}

	return 1;
}

int MsgFunc_SayText(const char *pszName, int iSize, void *pbuf)
{
	if (gConfigs.bEnableNewHud)
	{
		HudSayText().MsgFunc_SayText(pszName, iSize, pbuf);
		return 1;
	}
	
	return pmSayText(pszName, iSize, pbuf);
}

int MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int dest = READ_BYTE();
	char *text = READ_STRING();

	//return 0;

	if (g_iMod == MOD_ZB5)
	{
		if (!strcmp(text, "#CSBTE_ZombiSelectCount"))
		{
			char *str = READ_STRING();
			int count = atoi(str);

			if (count == 1)
				gHud3D_ZB.RoundStart();
		}
	}

	if (g_iMod == MOD_ZB4 || g_iMod == MOD_ZSE)	// MVP board has taken their places
	{
		if (!strcmp(text, "#Terrorists_Win"))
		{
			
			
			//gEngfuncs.pfnPlaySoundByName("sound/vox/z4_win_zombi.wav", 1.0);
			gEngfuncs.pfnClientCmd("spk sound/vox/z4_win_zombi.wav");

			/*
			HudBinkPlayer().SetPos(0.5 * ScreenWidth, 0.4 * ScreenHeight, 1);
			HudBinkPlayer().SetColor(255,255,255);
			HudBinkPlayer().BinkInstance("cstrike\\resource\\hud\\zombie\\zombiewin.bik", 1, 0);
			*/

			return 0;
		}
		else if (!strcmp(text, "#CTs_Win"))
		{
			gEngfuncs.pfnClientCmd("spk sound/vox/z4_win_human.wav");
			/*
			HudBinkPlayer().SetPos(0.5 * ScreenWidth, 0.4 * ScreenHeight, 1);
			HudBinkPlayer().SetColor(255, 255, 255);
			HudBinkPlayer().BinkInstance("cstrike\\resource\\hud\\zombie\\humanwin.bik", 1, 0);
			HudBinkPlayer().BinkInstance("cstrike\\resource\\hud\\zombie\\humanwin.bik", 0, 0);
			*/
			return 0;
		}
	}

	if (g_bAlive)
	{
		if (!strcmp(text, "#Terrorists_Win") || !strcmp(text, "#CTs_Win") || !strcmp(text, "#Round_Draw") || 
			!strcmp(text, "#Target_Bombed") || !strcmp(text, "#Bomb_Defused") || !strcmp(text, "#Target_Saved") || 
			!strcmp(text, "#All_Hostages_Rescued") || !strcmp(text, "#Hostages_Not_Rescued") || 
			!strcmp(text, "#VIP_Escaped") || !strcmp(text, "#VIP_Assassinated") || !strcmp(text, "#VIP_Not_Escaped") || 
			!strcmp(text, "#CTs_PreventEscape") || !strcmp(text, "#Escaping_Terrorists_Neutralized") || !strcmp(text, "#Terrorists_Not_Escaped"))
		{
			if (HudStatistics().m_pKilledShow->value)
			{
				HudStatistics().Open();
				HudStatistics().m_flDie = Hud().m_flTime + 3.0f;
			}
		}
	}

	if (gConfigs.bEnableNewHud)
	{
		HudSayText().MsgFunc_TextMsg(pszName, iSize, pbuf);

		if (dest == HUD_PRINTRADIO || dest == HUD_PRINTTALK)
			return 1;
	}

	return pmTextMsg(pszName, iSize, pbuf);
}

int MsgFunc_RoundTime(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	
	g_iRoundTime = READ_SHORT();
	
	HudRoundTimer().SetTimeEnd(g_iRoundTime + cl.time);
	HudZombieMod4().m_Tip.SetTimeEnd(g_iRoundTime);
	HudZombieMod4().m_ScoreBoard.SetLastScore();
	HudZombieMod4().m_Bink.Stop();
	HudZBSelect().SetDrawTime(-5.1f);
	
	for (int i = 0; i<12; i++)
		HudZombieMod4().m_Icon.AddIcon(i, 0, 0);

	if (g_iRoundTime < 20)
		HudZBCounter().VidInit();
	
	HudZombieMod3().m_iNVG = 0;
	g_iZ4Nvg = 0;

	for (int i = 0; i<8; i++)
		HudZombieMod4().m_Retina[i].SetCanDraw(false);

	return pmRoundTime(pszName, iSize, pbuf);
}

int MsgFunc_ShowTimer(const char *pszName, int iSize, void *pbuf)
{
	return pmShowTimer(pszName, iSize, pbuf);
}

int MsgFunc_HLTV(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int a = READ_BYTE();
	int b = READ_BYTE();

	if(a == 0 && b == 0)
	{		
		HudZBCounter().VidInit();
		gHud3D.m_iMaxHPAP[0] = gHud3D.m_iMaxHPAP[1] = 100;
	}


	return pmHLTV(pszName, iSize, pbuf);
}

#include <iostream>

int MsgFunc_StatusIcon(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	
	int status = READ_BYTE();
	char *string = READ_STRING();

	if (!strcmp(string, "buyzone"))
		g_bInBuyzone = status != 0;

	if (!g_bInBuyzone && (g_iMod == MOD_NONE || g_iMod == MOD_GHOST))
		BTEPanel_BuyMenu_Close();

	return pmStatusIcon(pszName, iSize, pbuf);
}

int MsgFunc_HostagePos(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	READ_BYTE();
	int iHostageIndex = READ_BYTE();
	float origin[3];
	origin[0] = READ_COORD();
	origin[1] = READ_COORD();
	origin[2] = READ_COORD();
		
	VectorCopy(origin, g_vecHostagePos[iHostageIndex - 1]);

	return pmHostagePos(pszName, iSize, pbuf);
}

int MsgFunc_HostageK(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iHostageIndex = READ_BYTE();

	memset(g_vecHostagePos[iHostageIndex - 1], 0, sizeof(g_vecHostagePos[iHostageIndex - 1]));

	return pmHostageK(pszName, iSize, pbuf);
}

extern DHANDLE<CCSBTEMapLoading>g_hCSBTEMapLoading;

int MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf)
{	
	BEGIN_READ(pbuf, iSize);
	int iMod = READ_BYTE();

	if (g_iMod != iMod / 10)
	{
		g_iMod = iMod / 10;
		BTEPanel_BuyMenu_Reset();		
	}

	if(g_iMod == MOD_ZB4)
	{
		Hud().m_HUD_number_0 = Hud().GetSpriteIndex("z4_number_0");
	}

	g_iWeaponLimit = iMod % 10;

	HudTABBoard().ResetLevel();

	if (g_hCSBTEMapLoading)
		g_hCSBTEMapLoading->UpdateGameMode(g_iMod);

	return 1;
}

int MsgFunc_ServerName(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	char *pszServerName = READ_STRING();

	HudTABBoard().ResetServerName(pszServerName);

	if (g_hCSBTEMapLoading)
		g_hCSBTEMapLoading->SetServerName(pszServerName);

	return pmServerName(pszName, iSize, pbuf);
}

int MsgFunc_BombDrop(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	Vector origin;
	origin.x = READ_COORD();
	origin.y = READ_COORD();
	origin.z = READ_COORD();

	bool plant;
	plant = READ_BYTE() == 0 ? false : true;
	/*
	HudTABBoard().m_vecBombOrigin = origin;
	HudTABBoard().m_bDrawBomb = true;
	HudTABBoard().m_bPlantedBomb = plant;

	HudTABBoard().m_flBombPlantedTime = Hud().m_flTime;
	*/
	return pmBombDrop(pszName, iSize, pbuf);
}

int MsgFunc_BombPickup(const char *pszName, int iSize, void *pbuf)
{
	
	return pmBombPickup(pszName, iSize, pbuf);
}

int MsgFunc_FlashBat(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	g_iFlashBat = READ_BYTE();
	return pmFlashBat(pszName, iSize, pbuf);
}

int MsgFunc_Flashlight(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	g_bFlashActive = READ_BYTE();
	g_iFlashBat = READ_BYTE();
	return pmFlashlight(pszName, iSize, pbuf);
}

int MsgFunc_AmmoPickup(const char *pszName, int iSize, void *pbuf)
{
	if (gConfigs.bEnableNewHud)
		return 1;

	return pmAmmoPickup(pszName, iSize, pbuf);
}

int EngFunc_HookUserMsg(char *szMsgName, pfnUserMsgHook pfn)
{
	if (!strcmp(szMsgName, "InitHUD"))
	{
		pmInitHUD = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_InitHUD);
	}
	
	if (!strcmp(szMsgName, "BombDrop"))
	{
		pmBombDrop = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_BombDrop);
	}

	if (!strcmp(szMsgName, "BombPickup"))
	{
		pmBombPickup = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_BombPickup);
	}

	if (!strcmp(szMsgName, "ServerName"))
	{
		pmServerName = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_ServerName);
	}

	if (!strcmp(szMsgName, "GameMode"))
	{
		pmGameMode = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_GameMode);
	}

	if (!strcmp(szMsgName, "HostageK"))
	{
		pmHostageK = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_HostageK);
	}
	
	if (!strcmp(szMsgName, "HostagePos"))
	{
		pmHostagePos = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_HostagePos);
	}


	if (!strcmp(szMsgName, "StatusIcon"))
	{
		pmStatusIcon = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_StatusIcon);
	}
	

	if (!strcmp(szMsgName, "HLTV"))
	{
		pmHLTV = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_HLTV);
	}

	if (!strcmp(szMsgName, "ShowTimer"))
	{
		pmShowTimer = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_ShowTimer);
	}

	if (!strcmp(szMsgName, "RoundTime"))
	{
		pmRoundTime = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_RoundTime);
	}
	
	if (!strcmp(szMsgName, "TextMsg"))
	{
		pmTextMsg = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_TextMsg);
	}

	if (!strcmp(szMsgName, "NVGToggle"))
	{
		pmNVGToggle = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_NVGToggle);
	}
	if (!strcmp(szMsgName, "TeamScore"))
	{
		pmTeamScore = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_TeamScore);
	}
	if (!strcmp(szMsgName, "ResetHUD"))
	{
		pmResetHUD = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_ResetHUD);
	}

	if (!strcmp(szMsgName, "Health"))
	{
		pmHealth = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_Health);
	}

	if (!strcmp(szMsgName, "Battery"))
	{
		pmBattery = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_Battery);
	}

	if (!strcmp(szMsgName, "ArmorType"))
	{
		pmArmorType = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_ArmorType);
	}

	if (!strcmp(szMsgName, "ScoreAttrib"))
	{
		pmScoreAttrib = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_ScoreAttrib);
	}

	if (!strcmp(szMsgName, "HideWeapon"))
	{
		pmHideWeapon = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_HideWeapon);
	}

	if (!strcmp(szMsgName, "WeaponList"))
	{
		pmWeaponList = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_WeaponList);
	}

	if (!strcmp(szMsgName, "CurWeapon"))
	{
		pmCurWeapon = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_CurWeapon);
	}

	if (!strcmp(szMsgName, "AmmoX"))
	{
		pmAmmoX = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_AmmoX);
	}
	if (!strcmp(szMsgName, "Location"))
	{
		pmLocation = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_Location);
	}

	if (!strcmp(szMsgName, "HudTextArgs"))
		gEngfuncs.pfnHookUserMsg("MetaHook", MsgFunc_MetaHook);

	if (!strcmp(szMsgName, "HudTextPro"))
		gEngfuncs.pfnHookUserMsg("MetaHook2", MsgFunc_MetaHook2);	

	if (!strcmp(szMsgName, "DeathMsg"))
	{
		pmDeathMsg = pfn;
		gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_DeathMsg);
		return 0;
	}
	if (!strcmp(szMsgName, "SendAudio"))
	{
		pmSendAudio = pfn;
		gEngfuncs.pfnHookUserMsg(szMsgName,MsgFunc_SendAudio);
		return 0;
	}
	if (!strcmp(szMsgName, "TeamInfo"))
	{
		pmTeamInfo = pfn;
		gEngfuncs.pfnHookUserMsg(szMsgName,MsgFunc_TeamInfo);
		return 0;
	}
	if (!strcmp(szMsgName, "ScoreAttrib"))
	{
		pmScoreAttrib = pfn;
		gEngfuncs.pfnHookUserMsg(szMsgName,MsgFunc_ScoreAttrib);
		return 0;
	}
	if (!strcmp(szMsgName, "ScoreInfo"))
	{
		pmScoreInfo = pfn;
		gEngfuncs.pfnHookUserMsg(szMsgName,MsgFunc_ScoreInfo);
		return 0;
	}
	if (!strcmp(szMsgName, "SetFOV"))
	{
		pmSetFOV = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_SetFOV);
	}
	if (!strcmp(szMsgName, "ShowMenu"))
	{
		pmShowMenu = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_ShowMenu);
	}
	if (!strcmp(szMsgName, "StatusValue"))
	{
		pmStatusValue = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_StatusValue);
	}
	if (!strcmp(szMsgName, "Money"))
	{
		pmMoney = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_Money);
	}
	if (!strcmp(szMsgName, "FlashBat"))
	{
		pmFlashBat = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_FlashBat);
	}
	if (!strcmp(szMsgName, "Flashlight"))
	{
		pmFlashlight = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_Flashlight);
	}
	if (!strcmp(szMsgName, "AmmoPickup"))
	{
		pmAmmoPickup = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_AmmoPickup);
	}
	if (!strcmp(szMsgName, "SayText"))
	{
		pmSayText = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_SayText);
	}
	if (!strcmp(szMsgName, "VGUIMenu"))
	{
		pmVGUIMenu = pfn;
		return gEngfuncs.pfnHookUserMsg(szMsgName, MsgFunc_VGUIMenu);
	}
	return gEngfuncs.pfnHookUserMsg(szMsgName, pfn);
}


