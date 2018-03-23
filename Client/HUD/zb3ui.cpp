
#include "hud.h"
#include "bte_const.h"
#include "plugins.h"
#include "exportfuncs.h"
#include "configs.h"
#include "bink/bink.h"
#include "msghook.h"
#include "parsemsg.h"
#include "texdraw.h"
#include "gl/gl.h"
#include "triangleapi.h"
#include "TriAPI.h"
#include "calcscreen.h"
#include "Fonts.h"
#include "util.h"
#include "common.h"
#include "TextureManager.h"
#include "LoadTexture.h"

#include "HUD3D_ZB.h"
#include "GameUI/SkillKeyPopupDlg.h"

#include "VGUI/IEngineVGUI.h"
#include "EngineInterface.h"
#include "vgui_controls/Label.h"

#include "zb3ui.h"
#include "DrawRetina.h"
#include "Client/HUD/drawimage.h"
#include "Client/HUD/FontText.h"
#include "Client/HUD/followicon.h"
#include "Client/HUD/DrawTGA.h"

#define CLASS_ZB	1
#define CLASS_HM	2
#define CLASS_HERO	3
#define CLASS_SELECTEDORIGIN 4

void BTEPanel_BuyMenu_Close();

static CHudZombieMod3 g_HudZombieMod3;
CHudZombieMod3 &HudZombieMod3()
{
	return g_HudZombieMod3;
}

void CHudZombieMod3::ParseIcon(char *pszName, int iSlot, int iType, int iData)
{
	
	int iShowiSlot = iSlot;
	bool bIsHuman = (m_iClass == CLASS_HM || m_iClass == CLASS_HERO || m_iClass == CLASS_SELECTEDORIGIN);
	if (bIsHuman && iSlot>0 && iSlot<5)
		iShowiSlot = g_SkillKeyConfig.iHumanSlotAlias[iSlot - 1] + 1;

	gHud3D_ZB.SetSkill(pszName, iShowiSlot, iType, iData);

	// iType
	// 1--NORMAL 2--FLASH 3--COLDDOWN 4--NOTALAILABLE 5--NOTALAILABLE(RED)
	auto &Texture = TextureManager()[va("%s.tga", pszName)];
	if (iSlot == 0)
	{
		m_UISlot[iShowiSlot].iTextureID = Texture;
		m_UISlot[iShowiSlot].iWidth = Texture.w();
		m_UISlot[iShowiSlot].iHeight = Texture.h();
		return;
	}
	m_UISlot[iShowiSlot].iType = iType;
	m_UISlot[iShowiSlot].iTextureID = Texture;
	m_UISlot[iShowiSlot].iWidth = Texture.w();
	m_UISlot[iShowiSlot].iHeight = Texture.h();
	m_UISlot[iShowiSlot].r = m_UISlot[iShowiSlot].g = 255;
	m_UISlot[iShowiSlot].b = 191;
	m_UISlot[iShowiSlot].a = 255;
	m_UISlot[iShowiSlot].flStartDisplayTime = Hud().m_flTime;
	m_UISlot[iShowiSlot].flEndDisplayTime = Hud().m_flTime + iData;
	if(iType == 4)
		m_UISlot[iSlot].a = 100;
	else if (iType == 5)
	{
		m_UISlot[iShowiSlot].r = 230;
		m_UISlot[iShowiSlot].g = m_UISlot[iShowiSlot].b = 150;
	}
	if (bIsHuman)
		m_UISlot[iSlot].szKey = CSkillKeyPopupDlg::m_szHumanKeyInfo[g_SkillKeyConfig.iHumanSkillKey[iShowiSlot]];
	else
		m_UISlot[iSlot].szKey = CSkillKeyPopupDlg::m_szZombieKeyInfo[g_SkillKeyConfig.iZombieSkillKey[iShowiSlot]];
}

#define	HUMAN_SKILL	20
#define	HUMAN_SKILL_ORIGIN	21

#include "BaseUI.h"

void CHudZombieMod3::CheckTeam(int team)
{
	if (team == 1) // CT
	{
		m_iClass = CLASS_HM;
	}
	else if (team == 2)
	{
		m_iClass = CLASS_ZB;
	}
	else m_iClass = 0;
}

void ZB3SkillIconCallback ( struct tempent_s *ent, float frametime, float currenttime )
{
	if (currenttime + 1 > ent->die)
		ent->entity.curstate.renderamt = 255 - 255 * (currenttime + 1 - ent->die);
}

void CHudZombieMod3::FindHumanInRadius(void)
{
	if (!g_bAlive)
		return;
	static cl_entity_t *player = NULL, *pEntity = NULL;
	player = gEngfuncs.GetLocalPlayer();
	m_iAdditionMorale = m_flAdditionMorale = 0;
	if (player)
	{
		if (vPlayer[player->index].team == 2)
			return;
		
		vec3_t vecOrigin = player->origin;
		for (int i = 1; i <= gEngfuncs.GetMaxClients(); i++)
		{
			if (!(pEntity = gEngfuncs.GetEntityByIndex(i)))
				continue;
			if (pEntity == player)
				continue;
			if ((vecOrigin - pEntity->origin).Length() > 230.0)
				continue;
			if (vPlayer[i].team == 2)
				continue;
			if (!vPlayer[i].alive)
				continue;
			m_iAdditionMorale += 10;
		}
	}

}

void CHudZombieMod3::AddHeadIcon(int idx, char *szSprite, float time, float flOffsetZ)
{
	model_t *pModel;
	cl_entity_t *pEntity = NULL;
	TEMPENTITY *pEnt;
	pEntity = gEngfuncs.GetEntityByIndex(idx);
	if (!pEntity) return;

	pModel = IEngineStudio.Mod_ForName(szSprite, 0);
	if (!pModel) return;

	if (pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(pEntity->origin, pModel))
	{
		pEnt->entity.curstate.rendermode = kRenderTransAdd;
		pEnt->entity.curstate.renderamt = 254;
		pEnt->entity.curstate.scale = 1.0;
		pEnt->entity.curstate.aiment = idx;

		pEnt->die = Hud().m_flTime + time;
		pEnt->clientIndex = idx;
		pEnt->tentOffset = Vector(0, 0, 0);
		pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_PLYRATTACHMENT;
		pEnt->callback = ZB3SkillIconCallback;

		Engfunc_Call_AddVisibleEntity(&(pEnt->entity));
	}
}

void CHudZombieMod3::ParseData(int DataType, int iData)
{
	switch (DataType)
	{
		case 0:
		{
			m_flMorale = 3 / 13.0f;
			m_iMoraleInt = 130;
			break;
		}
		case ZB3DATA_HM_MORALE:
		{
			m_flMorale = float(iData) / 13.0f;
			m_iMoraleInt = iData * 10 + 100;
			
			break;
		}
		case ZB3DATA_ZB_LEVEL:
		{
			break;
		}
#if 0
		case ZB3DATA_HM_MORALE_MAX :
		{
			m_iMoraleUp = 1;	
			break;
		}
#endif
		case ZB3DATA_ZB_RAGE:
		{
			m_flRagePercent = float(iData)/100.0;
			if(m_flRagePercent <0.25) m_iRage = -1;
			else if(m_flRagePercent <0.5) m_iRage = 0;
			else if(m_flRagePercent <0.75) m_iRage = 1;
			else m_iRage = 2;
			break;
		}
		case ZB3DATA_RETINA:
		{
			m_iRetinaDraw = 1;
			m_iRetinaClass = iData;
			m_flRetinaStart = Hud().m_flTime;
			break;
		}
		case ZB3DATA_RETINASTOP:
		{
			m_iRetinaDraw = 0;
			break;
		}
		case ZB3DATA_CLEARUI:
		{
			memset(m_UISlot,0,sizeof(m_UISlot));
			break;
		}
		case ZB3DATA_TOPUI:
		{
			m_iSkillType = iData;
			break;
		}
		case ZB3DATA_BECOME_ZB:
		{
			memset(m_UISlot,0,sizeof(m_UISlot));			
			HudRetina().Reset();
			//Hud().m_BuffClassDamage.Reset();
			if (iData != HUMAN_SKILL && iData != HUMAN_SKILL_ORIGIN)
				BTEPanel_BuyMenu_Close();
			switch (iData)
			{
				case HUMAN_SKILL:
				{
					m_iClass = CLASS_HM;
					ParseData(7, 0);
					ParseData(8, 0);
					ParseIcon("resource\\zombi\\humanskill_hm_spd",1,2,3);
					ParseIcon("resource\\zombi\\humanskill_hm_hd",2,2,3);
					break;
				}
				case HUMAN_SKILL_ORIGIN:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_SELECTEDORIGIN;
					ParseIcon("resource\\zombi\\zombietype_defaultzb", 0, 2, 3);
					ParseIcon("resource\\zombi\\humanskill_hm_spd", 1, 2, 3);
					ParseIcon("resource\\zombi\\humanskill_hm_hd", 2, 2, 3);
					
					break;
				}
				case TANK_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_defaultzb",0,2,3);
					ParseIcon("resource\\zombi\\zombieskill_zombicrazy",1,2,3);
					ParseIcon("resource\\zombi\\zombieskill_strengthrecovery", 2, 2, 3);
					break;
				}
				case SPEED_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_lightzb",0,2,3);
					ParseIcon("resource\\zombi\\zombieskill_zombihiding", 1, 2, 3); 
					ParseIcon("resource\\zombi\\zombieskill_jumpupm", 2, 2, 3);
					break;
				}
				case HEAVY_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_heavyzb",0,2,3);
					ParseIcon("resource\\zombi\\zombieskill_zombitrap",1,2,3);
					ParseIcon("resource\\zombi\\zombieskill_armorrecovery", 2, 2, 3);
					break;
				}
				case HEAL_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_doctorzb", 0, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_zombiheal", 1, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_strengthrecovery", 2, 2, 3);
					break;
				}
				case PC_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_pczb",0,2,3);
					ParseIcon("resource\\zombi\\zombieskill_zombismoke",1,2,3);
					ParseIcon("resource\\zombi\\zombieskill_strengthrecovery", 2, 2, 3);
					break;
				}
				case DEIMOS_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_deimoszb",0,2,3);
					ParseIcon("resource\\zombi\\zombieskill_tentacle",1,2,3);
					ParseIcon("resource\\zombi\\zombieskill_strengthrecovery", 2, 2, 3);
					break;
				}
				case DEIMOS2_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_deimos2zb", 0, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_zombicrazy2", 1, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_armorrecovery", 2, 2, 3);
					break;
				}
				case WITCH_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_witchzb", 0, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_zombihook", 1, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_jumpupm", 2, 2, 3);
					break;
				}
				case UNDERTAKER_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_undertakerzb", 0, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_zombipile", 1, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_armorrecovery", 2, 2, 3);
					break;
				}
				case CHINA_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_chinazb",0,2,3);
					ParseIcon("resource\\zombi\\zombieskill_zombistiffen",1,2,3);
					ParseIcon("resource\\zombi\\zombieskill_strengthrecovery", 2, 2, 3);
					break;
				}
				case BOMMER_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_boomerzb",0,2,3);
					ParseIcon("resource\\zombi\\zombieskill_recoverylast",1,2,3);
					ParseIcon("resource\\zombi\\zombieskill_armorrecovery",2,2,3);
					break;
				}
				case RESIDENT_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_residentzb",0,2,3);
					ParseIcon("resource\\zombi\\zombieskill_zombipenetration",1,2,3);
					ParseIcon("resource\\zombi\\zombieskill_jumpupm",2,2,3);
					break;
				}
				break;
			}
		}
		case 11:
		{
			HudRetina().Reset();
			switch (iData)
			{
				case BOMMER_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_boomerzb", 0, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_recoverylast", 1, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_armorrecovery", 2, 2, 3);
					break;
				}
				case RESIDENT_ZB:
				{
					m_iSkillType = 0;
					m_iClass = CLASS_ZB;
					ParseIcon("resource\\zombi\\zombietype_residentzb", 0, 2, 3);
					ParseIcon("resource\\zombi\\zombieskill_jumpupm", 2, 2, 3);
					break;
				}
			}
			break;
		}
		case 12:
		{
			m_iNVG = iData;
			break;
		}
		case 13:
			break;
		case 14:
		{
			AddHeadIcon(iData, "sprites/zb_skill_headshot.spr", 4.0f, 0.0);
			break;
		}
		case 15:
		{
			g_iHookVAngle = iData;
			memset(g_vecVAngle, 0, sizeof(g_vecVAngle));
			if(!iData)
				gEngfuncs.pfnClientCmd("-duck;");
			break;
		}
		case 16:
		{
			g_iShowCustomCrosshair = iData;

			if(iData)
			{
				Hud().m_iHideHUDDisplay |= HIDEWEAPON_CROSSHAIR;
			}
			else
			{
				Hud().m_iHideHUDDisplay &= ~HIDEWEAPON_CROSSHAIR;
			}
			break;
		}
		case 18:
		{
			DrawImageItem rgTempDrawImage;
			rgTempDrawImage.iFunction = 1;
			rgTempDrawImage.iCheck = 1;
			rgTempDrawImage.iCenter = 0;

			char szSprites[2048];
			if(rgTempDrawImage.iCheck > 0)
			{
				sprintf(szSprites, "%s", "zombirecovery");
				rgTempDrawImage.iSprIndex = Hud().GetSpriteIndex(szSprites);
			}
			else
			{
				sprintf(szSprites, "sprites/%s.spr", READ_STRING());
				rgTempDrawImage.hl_hSprites = gEngfuncs.pfnSPR_Load(szSprites);
			}
			rgTempDrawImage.x = 7;
			rgTempDrawImage.y = ScreenHeight - 85;
			rgTempDrawImage.color.r = 255;
			rgTempDrawImage.color.g = 255;
			rgTempDrawImage.color.b = 255;
			rgTempDrawImage.iMode = 3;
			rgTempDrawImage.flStartDisplayTime = Hud().m_flTime;
			rgTempDrawImage.flEndDisplayTime = 1.0 + Hud().m_flTime;
			rgTempDrawImage.iChanne = -1;
			rgTempDrawImage.iLength = -1;

			Hud().m_SPR.AddElement(rgTempDrawImage);

			break;
		}
		case 19:
		{
			DrawFontTextItem rgTempDrawText;
			
			rgTempDrawText.iCenter = 1;
			rgTempDrawText.x = 0.5 * ScreenWidth;
			rgTempDrawText.y = 0.3 * ScreenHeight;
			rgTempDrawText.color.r = 237;
			rgTempDrawText.color.g = 182;
			rgTempDrawText.color.b = 65;
			rgTempDrawText.iScale = 32;
			rgTempDrawText.flDisplayTime = 3.0 + Hud().m_flTime;
			rgTempDrawText.flStartTime = Hud().m_flTime;
			rgTempDrawText.fFadeTime = 1.0;
			rgTempDrawText.iFillBg = 0;
			rgTempDrawText.iChanne = 2;
			rgTempDrawText.flDisplayTime += rgTempDrawText.fFadeTime*2;

			wchar_t *pBuf = vgui::localize()->Find("CSBTE_ZB3_Morale");

			if (!pBuf)
				break;

			wchar_t ch[2][32];
			if (iData < 10)
			{
				ch[1][0] = L'0' + iData;
				ch[1][1] = 0;
			}
			else
			{
				ch[1][0] = L'0' + iData / 10;
				ch[1][1] = L'0' + iData % 10;
				ch[1][2] = 0;
			}

			vgui::localize()->ConstructString(ch[0], 256, pBuf, 1, ch[1]);

			sprintf(rgTempDrawText.szText, "%s", UnicodeToUTF8(ch[0]));
			Hud().m_FontText.AddElement(rgTempDrawText);
		}
	}
}
void CHudZombieMod3::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}
void CHudZombieMod3::VidInit(void)
{
	m_iMoraleIcon = Hud().GetSpriteIndex("ZB3_MoraleIcon");
	m_iMoraleLevel = Hud().GetSpriteIndex("ZB3_MoraleLevel");
	m_iMoraleEffect = Hud().GetSpriteIndex("ZB3_MoraleEffect");
	m_iRadarItem = Hud().GetSpriteIndex("radar_item");
	m_iRageLevel = Hud().GetSpriteIndex("ZB3_RageLevel");
	m_iRageBG = Hud().GetSpriteIndex("ZB3_RageBg");
	m_iRageIndex = Hud().GetSpriteIndex("ZB3_RageEffect000");	
	
	if (!m_pLabelMorale)
	{
		m_pLabelMorale = new vgui::Label(nullptr, "HudZombieMod3_MoraleText", "");
		m_pLabelMorale->SetParent(enginevguifuncs->GetPanel(PANEL_CLIENTDLL));
		IScheme *pScheme = scheme()->GetIScheme(m_pLabelMorale->GetScheme());
		if (pScheme)
		{
			m_pLabelMorale->SetFont(pScheme->GetFont("Default16"));
		}
		m_pLabelMorale->SetPos(ScreenWidth / 2 - 77, ScreenHeight - 113);
	}
	m_pLabelMorale->SetVisible(false);

	m_iNVG = FALSE;

	memset(m_UISlot,0,sizeof(m_UISlot));
	for(int i=1;i<7;i++)
	{
		char szSpr[32];
		sprintf(szSpr,"sprites/rage%d.spr",i);
		m_hRage[i-1] = gEngfuncs.pfnSPR_Load(szSpr);
	}
	m_iRetinaDraw = 0;
	m_iRetinaClass = 0;
	m_flRetinaStart = 0;
	
	m_iRetina[0] = TextureManager()["resource\\zombi\\zombicrazy.tga"];
	m_iRetina[6] = TextureManager()["resource\\zombi\\damagedouble.tga"];
	m_iRetina[4] = TextureManager()["resource\\zombi\\zombiheal.tga"];
	m_iRetina[3] = TextureManager()["resource\\zombi\\zombispeedup.tga"];
	m_iRetina[2] = TextureManager()["resource\\zombi\\zombihiding.tga"];

	//m_iKeySlotBG = TextureManager()["resource\\zombi\\skillslotkeybg.tga"];
	//m_iSkullSlotBG = TextureManager()["resource\\zombi\\skillslotbg.tga"];

	m_iSupplyBox = TextureManager()["resource\\hud\\icon\\supplybox.tga"];
	m_iZombieTrap = TextureManager()["resource\\hud\\icon\\trap_s.tga"];

	m_iClass = CLASS_HM;
	m_iRage = -1;
	m_flMorale = 0.0f;
	m_iMoraleInt = 100;
	m_flRagePercent = 0;
	m_iAdditionMorale = 0;

	m_flRageTimer = 0;
	m_iRageFrame = 0;
	
	ParseIcon("resource\\zombi\\zombietype_defaultzb", 0, 1, 250);
	ParseIcon("resource\\zombi\\humanskill_hm_spd", 1, 4, 250);
	ParseIcon("resource\\zombi\\humanskill_hm_hd", 2, 4, 250);
}
void CHudZombieMod3::DrawRetina(float time)
{
	if(!m_iRetinaDraw)
		return;
	if(!m_iRetinaClass)
		return;
	//if(m_iClass != CLASS_ZB) return;
	
	switch (m_iRetinaClass)
	{
	case 1:
		{
				TEXD_SetRenderMode(kRenderTransTexture);
				TEXD_BindTexture(m_iRetina[m_iRetinaClass-1]);
				float alpha;
				float fSubTime = time;
				fSubTime -=(int)fSubTime;
				fSubTime-=0.5;
				fSubTime=fabsf(fSubTime);	
				alpha =fSubTime+0.5;
				alpha *= 255;
				TEXD_SetColor(255, 255, 255, alpha);

				TEXD_DrawTexture(0, 0, ScreenWidth, ScreenHeight, NULL);
				break;
		}
	case 2:
		{
				TEXD_SetRenderMode(kRenderTransTexture);
				TEXD_BindTexture(m_iRetina[m_iRetinaClass - 1]);
				float alpha;
				if(time-m_flRetinaStart<1.0)
				{
					alpha = time-m_flRetinaStart;
				}
				else
				{
					m_iRetinaDraw = 0;
					break;
				}
				alpha *= 255;
				TEXD_SetColor(255, 255, 255, alpha);

				TEXD_DrawTexture(0, 0, ScreenWidth, ScreenHeight, NULL);
				break;
		}
	case 3:
	case 4:		
	case 5:
		{
				TEXD_SetRenderMode(kRenderTransTexture);
				TEXD_BindTexture(m_iRetina[m_iRetinaClass - 1]);
				TEXD_SetColor(255, 255, 255, 255);
				TEXD_DrawTexture(0, 0, ScreenWidth, ScreenHeight, NULL);

				break;
		}
	case 6:
	case 7:
		{
				TEXD_SetRenderMode(kRenderTransTexture);
				TEXD_BindTexture(m_iRetina[m_iRetinaClass - 1]);
			
				float alpha;
				
				if(m_iRetinaClass==3||m_iRetinaClass==7||m_iRetinaClass==4)
				{
					float fSubTime = time;
					fSubTime -=(int)fSubTime;
					fSubTime-=0.5;
					fSubTime=fabsf(fSubTime);	
					alpha =fSubTime+0.5;
				}
				else
				{
					if(time-m_flRetinaStart<1.0)
					{
						alpha = time-m_flRetinaStart;
					}
					else
					{
						m_iRetinaDraw = 0;
						break;
					}
				}
				alpha *= 255;

				TEXD_SetColor(255, 255, 255, alpha);

				int iW = ScreenWidth / 2;
				int iH = ScreenHeight / 2;
				
				wrect_t rcTexture;
				rcTexture.left = 0;
				rcTexture.top = 0;
				rcTexture.bottom = iH;
				rcTexture.right = iW;
	
				TEXD_DrawTexture(0, 0, iW, iH, &rcTexture, 1.0, 0);
				TEXD_DrawTexture(iW, 0, iW, iH, &rcTexture, 1.0, 1);
				TEXD_DrawTexture(0, iH, iW, iH, &rcTexture, 1.0, 2);
				TEXD_DrawTexture(iW, iH, iW, iH, &rcTexture, 1.0, 3);

				break;
		}
	}
}
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
void CHudZombieMod3::Draw(float time)
{
	if(g_iMod != MOD_ZB3)
		return;
	if(!g_bAlive)
		return;

	Tri_Enable(GL_TEXTURE_2D);
	Tri_Enable(GL_BLEND);
	Tri_Enable(GL_ALPHA_TEST);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	// 视网膜
	DrawRetina(time);

	// 英雄图标
	DrawHeroIcon(time);

	if (m_iClass == CLASS_HM || m_iClass == CLASS_HERO || m_iClass == CLASS_SELECTEDORIGIN)
	{
		// 士气等级
		DrawHumanMorale(time);
	}
	else if(m_iClass == CLASS_ZB)
	{
		DrawZombieRage(time);
	}

	if (!gConfigs.bEnableNewHud)
		DrawSkillBoard(time);
}

void CHudZombieMod3::Think()
{
	if (m_iClass == CLASS_HM && g_bAlive)
		m_pLabelMorale->SetVisible(true);
	else
		m_pLabelMorale->SetVisible(false);
}

bool CHudZombieMod3::CheckSkillKey(int eventcode, int keynum, const char *pszCurrentBinding)
{
	if (!g_bAlive)
		return false;

	if (m_iClass == CLASS_HM || m_iClass == CLASS_HERO || m_iClass == CLASS_SELECTEDORIGIN)
	{
		for (int i = 0; i < 4; i++)
		{
			//if (GetKeyState(CSkillKeyPopupDlg::m_iHumanKeyVK[g_SkillKeyConfig.iHumanSkillKey[i]])<0)
			if (keynum == CSkillKeyPopupDlg::m_iHumanKeyVK[g_SkillKeyConfig.iHumanSkillKey[i]])
			{
				gEngfuncs.pfnClientCmd(va("BTE_HumanSkill%d", g_SkillKeyConfig.iHumanSlotAlias[i] + 1));
				return true;
			}
		}
		
	}
	else if (m_iClass == CLASS_ZB)
	{
		for (int i = 0; i < 4; i++)
		{
			//if (GetKeyState(CSkillKeyPopupDlg::m_iZombieKeyVK[g_SkillKeyConfig.iZombieSkillKey[i]])<0)
			if (keynum == CSkillKeyPopupDlg::m_iZombieKeyVK[g_SkillKeyConfig.iZombieSkillKey[i]])
			{
				gEngfuncs.pfnClientCmd(va("BTE_ZombieSkill%d", i+1));
				return true;
			}
		}
	}
	return false;
}



void CHudZombieMod3::DrawHeroIcon(float time)
{
	int iLocal = gEngfuncs.GetLocalPlayer()->index;
	for (int i = 1; i<33; i++)
	{
		if (vPlayer[i].iHero && i != iLocal)
		{
			float vecScreen[2];
			cl_entity_s *p;
			p = gEngfuncs.GetEntityByIndex(i);

			if (!p) continue;
			if (CalcScreen(p->origin, vecScreen))
			{
				float distance = (gEngfuncs.GetLocalPlayer()->origin - p->origin).Length() / 39.37f;
				if (distance < 50.0)
				{
					static auto &HeroIcon = TextureManager()["resource\\hud\\icon\\hero_s.tga"];
					//GL_DrawTGA(HeroIcon, 255, 255, 255, 255, vecScreen[0] - HeroIcon.w() / 2, vecScreen[1] - HeroIcon.h() / 2, 1.0);

					int iX = vecScreen[0] - HeroIcon.w() / 2;
					int iY = vecScreen[1] - HeroIcon.h() / 2;

					HeroIcon.Draw(iX, iY);

					char p1[256];
					wchar_t p2[256];
					sprintf(p1, "[%dm]", (short int)distance);

					g_FontBold.C2W(p2, 255, p1);
					g_FontBold.SetColor(186, 164, 255, 255);
					g_FontBold.SetWidth(16);
					int a = HeroIcon.w() / 2;
					g_FontBold.DrawString(p2, int(vecScreen[0] - g_FontBold.GetLen(p2) / 2 - 1), vecScreen[1] + 60 - HeroIcon.h() / 2, 100);
				}
			}
		}
	}
}

void CHudZombieMod3::DrawSkillBoard(float time)
{
	// 上方技能栏
	Tri_Enable(GL_TEXTURE_2D);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static auto &SkullSlotBG = TextureManager()["resource\\zombi\\skillslotbg.tga"];
	static auto &KeySlotBG = TextureManager()["resource\\zombi\\skillslotkeybg.tga"];

	int iX, iY;
	iY = 58;
	iX = ScreenWidth / 2 - SkullSlotBG.w() / 2;
	if (m_iClass == CLASS_ZB || m_iClass == CLASS_SELECTEDORIGIN)
	{ 
		iX -= 35; // from pic size
		//先画头像
		//GL_DrawTGA(m_UISlot[0].iTextureID, 255, 255, 255, 255, iX + 2, iY, 1.0);
		Tri_BindTexture(GL_TEXTURE_2D, m_UISlot[0].iTextureID);
		if (g_iVideoMode)
			glColor4ub(255, 255, 255, 255);
		else
			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(iX, iY + m_UISlot[0].iHeight, 0);
		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[0].iWidth, iY + m_UISlot[0].iHeight, 0);
		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[0].iWidth, iY, 0);
		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(iX, iY, 0);
		gEngfuncs.pTriAPI->End();

		iX += 70;
	}
	// 再画按键
	//GL_DrawTGA(SkullSlotBG.idx(), 255, 255, 255, 255, iX, iY, 1.0);
	SkullSlotBG.Draw(iX, iY);

	for (int i = 1; i<5; i++)
	{
		int r = m_UISlot[i].r;
		int g = m_UISlot[i].g;
		int b = m_UISlot[i].b;
		int a = m_UISlot[i].a;

		if (m_UISlot[i].iType == 1 || m_UISlot[i].iType == 4 || m_UISlot[i].iType == 5)
		{
			Tri_BindTexture(GL_TEXTURE_2D, m_UISlot[i].iTextureID);
			if (g_iVideoMode)
				glColor4ub(r, g, b, a);
			else
				gEngfuncs.pTriAPI->Color4ub(r, g, b, a);
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			gEngfuncs.pTriAPI->Vertex3f(iX, iY + m_UISlot[i].iHeight, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[i].iWidth, iY + m_UISlot[i].iHeight, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[i].iWidth, iY, 0);
			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			gEngfuncs.pTriAPI->Vertex3f(iX, iY, 0);
			gEngfuncs.pTriAPI->End();

		}
		else if (m_UISlot[i].iType == 2)
		{
			float fSubTime = m_UISlot[i].flEndDisplayTime - time;
			if (fSubTime<0)
			{
				m_UISlot[i].iType = 1;
				continue;
			}
			fSubTime -= (int)fSubTime;
			fSubTime -= 0.5;
			fSubTime = fabsf(fSubTime);
			a = fSubTime * 510;
			Tri_BindTexture(GL_TEXTURE_2D, m_UISlot[i].iTextureID);
			if (g_iVideoMode)
				glColor4ub(r, g, b, a);
			else
				gEngfuncs.pTriAPI->Color4ub(r, g, b, a);
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			gEngfuncs.pTriAPI->Vertex3f(iX, iY + m_UISlot[i].iHeight, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[i].iWidth, iY + m_UISlot[i].iHeight, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[i].iWidth, iY, 0);
			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			gEngfuncs.pTriAPI->Vertex3f(iX, iY, 0);
			gEngfuncs.pTriAPI->End();
		}
		else if (m_UISlot[i].iType == 3)
		{
			float NeedDisplay = m_UISlot[i].flEndDisplayTime - m_UISlot[i].flStartDisplayTime;
			float Displayed = time - m_UISlot[i].flStartDisplayTime;
			float fPercent = Displayed / NeedDisplay;
			if (fPercent>1.0)
			{
				m_UISlot[i].flEndDisplayTime = time + 3.0;
				m_UISlot[i].flStartDisplayTime = time;
				m_UISlot[i].iType = 2;
				continue;
			}
			
			r = 255;
			g = 255;
			b = 191;
			a = 50;

			Tri_BindTexture(GL_TEXTURE_2D, m_UISlot[i].iTextureID);
			if (g_iVideoMode)
				glColor4ub(r, g, b, a);
			else
				gEngfuncs.pTriAPI->Color4ub(r, g, b, a);
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			gEngfuncs.pTriAPI->Vertex3f(iX, iY, 0);
			gEngfuncs.pTriAPI->TexCoord2f(0, 1 - fPercent);
			gEngfuncs.pTriAPI->Vertex3f(iX, iY + m_UISlot[i].iHeight* (1.0 - fPercent), 0);
			gEngfuncs.pTriAPI->TexCoord2f(1, 1 - fPercent);
			gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[i].iWidth, iY + m_UISlot[i].iHeight* (1.0 - fPercent), 0);

			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[i].iWidth, iY, 0);
			gEngfuncs.pTriAPI->End();

			
			r = 230;
			g = 150;
			b = 150;
			a = 255;
			if (g_iVideoMode)
				glColor4ub(r, g, b, a);
			else
				gEngfuncs.pTriAPI->Color4ub(r, g, b, a);
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			gEngfuncs.pTriAPI->TexCoord2f(0, 1 - fPercent);
			gEngfuncs.pTriAPI->Vertex3f(iX, iY + m_UISlot[i].iHeight* (1.0 - fPercent), 0);
			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			gEngfuncs.pTriAPI->Vertex3f(iX, iY + m_UISlot[i].iHeight, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[i].iWidth, iY + m_UISlot[i].iHeight, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1, 1 - fPercent);
			gEngfuncs.pTriAPI->Vertex3f(iX + m_UISlot[i].iWidth, iY + m_UISlot[i].iHeight* (1.0 - fPercent), 0);
			gEngfuncs.pTriAPI->End();
		}
		//GL_DrawTGA(KeySlotBG, 255, 255, 255, 255, iX-3, iY-3, 1.0);
		KeySlotBG.Draw(iX - 3, iY - 3);

		g_Font.SetColor(255, 255, 255, 170);
		g_Font.SetWidth(14);

		if (m_iClass != CLASS_ZB)
		{
			if (g_SkillKeyConfig.iHumanSkillKey[i - 1] >= 6)
				g_Font.DrawString(UTF8ToUnicode(CSkillKeyPopupDlg::m_szHumanKeyInfo[g_SkillKeyConfig.iHumanSkillKey[i - 1]]), iX + 3, iY + 13, 100);
			else
				g_Font.DrawString(UTF8ToUnicode(CSkillKeyPopupDlg::m_szHumanKeyInfo[g_SkillKeyConfig.iHumanSkillKey[i - 1]]), iX + 7, iY + 13, 100);
		}
		else
		{
			if (g_SkillKeyConfig.iHumanSkillKey[i - 1] >= 7)
				g_Font.DrawString(UTF8ToUnicode(CSkillKeyPopupDlg::m_szZombieKeyInfo[g_SkillKeyConfig.iZombieSkillKey[i - 1]]), iX + 3, iY + 13, 100);
			else
				g_Font.DrawString(UTF8ToUnicode(CSkillKeyPopupDlg::m_szZombieKeyInfo[g_SkillKeyConfig.iZombieSkillKey[i - 1]]), iX + 7, iY + 13, 100);
		}
		iX += 58;
	}
}

void CHudZombieMod3::DrawHumanMorale(float time)
{
	// 符号
	int iX, iY;
	iX = ScreenWidth / 2;
	iX -= 130;
	//iY = 390/*ScreenHeight * 0.86*/;

	//int yoffset = ScreenHeight * 0.14;

	iY = ScreenHeight - 100;//max(90,yoffset);

	FindHumanInRadius();

	int iMorale = m_iAdditionMorale;

	if (iMorale > 120)
		iMorale = 120;

	int iRealMorale = iMorale;

	if (iRealMorale % 10)
		iRealMorale -= 5;

	float flMorale = m_flMorale + iRealMorale / 130.0f;

	gHud3D_ZB.SetAtk(1, (m_iMoraleInt - 30 + iMorale) / 100.0f);

	if (flMorale < 0.4)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMoraleIcon), 0, 177, 0);
		//g_FontBold.SetColor(0, 177, 0, 255);
		m_pLabelMorale->SetFgColor({ 0, 177, 0, 255 });
	}
	else if (flMorale < 0.6)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMoraleIcon), 137, 191, 20);
		//g_FontBold.SetColor(137, 191, 20, 255);
		m_pLabelMorale->SetFgColor({ 137, 191, 20, 255 });
	}
	else if (flMorale < 0.8)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMoraleIcon), 250, 229, 0);
		//g_FontBold.SetColor(250, 229, 0, 255);
		m_pLabelMorale->SetFgColor({ 250, 229, 0, 255 });
	}
	else if (flMorale < 1.0)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMoraleIcon), 243, 127, 1);
		//g_FontBold.SetColor(243, 127, 1, 255);
		m_pLabelMorale->SetFgColor({ 243, 127, 1, 255 });
	}
	else if (flMorale = 1.0)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMoraleIcon), 255, 3, 0);
		//g_FontBold.SetColor(255, 3, 0, 255);
		m_pLabelMorale->SetFgColor({ 255, 3, 0, 255 });
	}
	else if (flMorale > 1.0)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMoraleIcon), 127, 40, 208);
		//g_FontBold.SetColor(127, 40, 208, 255);
		m_pLabelMorale->SetFgColor({ 127, 40, 208, 255 });
	}
	//gEngfuncs.pfnSPR_DrawAdditive(m_iMoraleIcon,iX,iY,&Hud().GetSpriteRect(m_iMoraleIcon));
	gEngfuncs.pfnSPR_DrawAdditive(0, iX, iY, &Hud().GetSpriteRect(m_iMoraleIcon));
	if (iMorale == 230)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMoraleEffect), 255, 255, 255);
		gEngfuncs.pfnSPR_DrawAdditive(0, iX - 17, iY - 16, &Hud().GetSpriteRect(m_iMoraleEffect));
	}
	iX += 53;
	iY += 13;
	static wrect_t *TempRect = &Hud().GetSpriteRect(m_iMoraleLevel);
	wrect_t ModifyRect;
	ModifyRect.bottom = TempRect->bottom;
	ModifyRect.left = TempRect->left;
	ModifyRect.right = TempRect->right;
	ModifyRect.top = TempRect->top;
	int iLen = TempRect->right - ModifyRect.left;
	ModifyRect.right = iLen + ModifyRect.left;
	// 背景条
	gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMoraleLevel), 50, 50, 50);
	gEngfuncs.pfnSPR_DrawAdditive(0, iX, iY, &ModifyRect);

	// 当前条
	if (flMorale != 0)
	{
		iLen = TempRect->right - ModifyRect.left;
		iLen = iLen * flMorale;
		ModifyRect.right = iLen + ModifyRect.left;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMoraleLevel), 255, 255, 255);
		gEngfuncs.pfnSPR_DrawAdditive(0, iX, iY, &ModifyRect);
	}

	// 汉字
	//g_FontBold.SetWidth(16);
	//iX += 00;
	//iY -= 18;
	static wchar_t wszText[256];
	if (iMorale)
	{
		swprintf(wszText, vgui::localize()->Find("#CSO_ZB3_AttackRate2"), max(100, m_iMoraleInt - 30), iMorale);
	}
	else
	{
		swprintf(wszText, vgui::localize()->Find("#CSO_ZB3_AttackRate"), max(100, m_iMoraleInt - 30));
	}
	//g_FontBold.DrawString(wszText, iX, iY, 1000);
	m_pLabelMorale->SetText(wszText);
	m_pLabelMorale->SizeToContents();
}

void CHudZombieMod3::DrawZombieRage(float time)
{
	// 背景
	int iX, iY;
	iX = ScreenWidth / 2 - gEngfuncs.pfnSPR_Width(Hud().GetSprite(m_iRageBG), 0) / 2;
	iY = ScreenHeight*0.97 - gEngfuncs.pfnSPR_Height(Hud().GetSprite(m_iRageBG), 0) / 2;

	gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iRageBG), 255, 255, 255);
	gEngfuncs.pfnSPR_DrawAdditive(0, iX, iY, &Hud().GetSpriteRect(m_iRageBG));

	// 动态头像
	if (m_flRageTimer < time)
	{
		m_flRageTimer = time + 0.1;
		m_iRageFrame++;
		if (m_iRageFrame>7) m_iRageFrame = 0;
	}
	if (m_iRage != -1)
	{
		int iRageIndex = 8 * m_iRage + m_iRageFrame;
		gEngfuncs.pfnSPR_Set(m_hRage[(iRageIndex / 4)], 255, 255, 255);
		gEngfuncs.pfnSPR_DrawAdditive(0, iX, iY - 3, &Hud().GetSpriteRect(iRageIndex + m_iRageIndex));
	}
	// 怒气槽
	// 背景条
	iX += 76;
	iY += 50;
	gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iRageLevel), 50, 50, 50);
	gEngfuncs.pfnSPR_DrawAdditive(0, iX, iY, &Hud().GetSpriteRect(m_iRageLevel));
	// 当前条
	if (m_flRagePercent != 0)
	{
		static wrect_t *TempRect = &Hud().GetSpriteRect(m_iRageLevel);
		wrect_t ModifyRect;
		ModifyRect.bottom = TempRect->bottom;
		ModifyRect.left = TempRect->left;
		ModifyRect.right = TempRect->right;
		ModifyRect.top = TempRect->top;
		int iLen = ModifyRect.right - ModifyRect.left;
		iLen = iLen * m_flRagePercent + 1;
		ModifyRect.right = iLen + ModifyRect.left;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iRageLevel), 255, 255, 255);
		gEngfuncs.pfnSPR_DrawAdditive(0, iX, iY, &ModifyRect);
	}

	gHud3D_ZB.SetAtk(0, m_flRagePercent);
}