
#include "hud.h"
#include "metahook.h"
#include "plugins.h"
#include "gl/gl.h"
#include "TriAPI.h"
#include "triangleapi.h"
#include "configs.h"
#include "BaseUI.h"
#include "util.h"

#include "DrawTGA.h"

static CHudTGAElements g_HudTGAElements;

CHudTGAElements &HudTGAElements()
{
	return g_HudTGAElements;
}

DrawTgaItem g_Tga[MAX_TGA];
imagedata_s g_MHTga[MH_MAX_TGA];

std::vector<Tga> g_Texture;

void CHudTGAElements::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
	memset(g_Tga, 0, sizeof(g_Tga));
	m_iCurrentTGA = 0;
	m_bLoadedTGA = FALSE;
}
void CHudTGAElements::VidInit(void)
{
	if (m_bLoadedTGA)
		return;
	LoadList();
}
int CHudTGAElements::GetTGA(char *pszName, bool f)
{
	char TgaName[512];	

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	char TgaNameLang[512];

	if (!f)
	{
		sprintf(TgaNameLang,"metahook\\tga\\%s_%s.tga",pszName,gConfigs.szLanguage);

		hFind = FindFirstFile(TgaNameLang, &FindFileData);	

		if (hFind == INVALID_HANDLE_VALUE)
			sprintf(TgaName,"metahook\\tga\\%s.tga",pszName);
		else
			sprintf(TgaName,"metahook\\tga\\%s_%s.tga",pszName,gConfigs.szLanguage);
	}
	else
	{
		sprintf(TgaNameLang, "cstrike_%s\\%s.tga", gConfigs.szLanguage, pszName);

		hFind = FindFirstFile(TgaNameLang, &FindFileData);	

		if (hFind == INVALID_HANDLE_VALUE)
			sprintf(TgaName, "cstrike\\%s.tga", pszName);
	}



	for(int i=0;i<MH_MAX_TGA;i++)
	{
		if(!strcmp(TgaName,g_MHTga[i].name))
		{
			return i;
		}
	}
	// Load IT!
	int iTotal = vgui::surface()->CreateNewTextureID();
	if(Image_LoadImage(TgaName,&g_MHTga[m_iCurrentTGA],&iTotal))
	{
		sprintf(g_MHTga[m_iCurrentTGA].name,"%s",TgaName);
		m_iCurrentTGA++;
		return (m_iCurrentTGA-1);
	}
	return 0;
}

void CHudTGAElements::LoadList(void)
{
	LogToFile("===准备加载TGA文件列表===");

	FILE* pTgaList;
	static char szBuff[512];
	szBuff[0] = '\0';
	pTgaList = fopen("metahook\\tga\\TgaFilesList.txt", "rt");
	if (pTgaList == NULL)
	{
		LogToFile("列表打开失败");
		return;
	}
	while (!feof(pTgaList))
	{
		fgets(szBuff, sizeof(szBuff)-1, pTgaList);
		if (szBuff[strlen(szBuff) - 1] == '\n') szBuff[strlen(szBuff) - 1] = '\0';

		if (szBuff[0] == '/' && szBuff[1] == '/' || !szBuff[0]) continue;
		int iTotal = vgui::surface()->CreateNewTextureID();
		if (Image_LoadImage(szBuff, &g_MHTga[m_iCurrentTGA], &iTotal))
		{
			sprintf(g_MHTga[m_iCurrentTGA].name, "%s", szBuff);
			LogToFile(szBuff);
			m_iCurrentTGA++;
		}
	}

	g_iCountBg = FindTexture("resource\\hud\\other\\countbg");

	g_tga_Scope = GetTGA("other\\scope_square");
	g_tga_Scope_Target = GetTGA("other\\sfscopetarget");
	g_tga_custom_scope_m200 = GetTGA("other\\scope_m200");
	g_tga_custom_scope = GetTGA("other\\scope");

	LogToFile("共有%d个TGA/BMP纹理被成功加载", m_iCurrentTGA);

	m_bLoadedTGA = TRUE;
}

int CHudTGAElements::GetBMP(char *pszName)
{
	char TgaName[256];	
	sprintf(TgaName,"cstrike\\%s",pszName);
	for(int i=0;i<MH_MAX_TGA;i++)
	{
		if(!strcmp(TgaName,g_MHTga[i].name))
		{
			return i;
		}
	}
	int iTotal = vgui::surface()->CreateNewTextureID();	
	if(Image_LoadImage(TgaName,&g_MHTga[m_iCurrentTGA],&iTotal))
	{
		sprintf(g_MHTga[m_iCurrentTGA].name,"%s",TgaName);
		m_iCurrentTGA++;
		return (m_iCurrentTGA-1);
	}
	return 0;
}

void CHudTGAElements::AddElement(DrawTgaItem rgTempDrawImage)
{
	if(rgTempDrawImage.fScale==0.0) rgTempDrawImage.fScale = 1.0;
	rgTempDrawImage.iMHTgaID = GetTGA(rgTempDrawImage.szName);
	if (rgTempDrawImage.iChanne <= 0 || rgTempDrawImage.iChanne >= MAX_TGA)
	{
		for (int i = 0; i < MAX_TGA; i++)
		{
			if (!g_Tga[i].flEndDisplayTime)
			{
				g_Tga[i] = rgTempDrawImage;
				break;
			}
		}
	}
	else
	{
		g_Tga[rgTempDrawImage.iChanne] = rgTempDrawImage;
	}
}
void CHudTGAElements::Draw(float time)
{		 
	for (int i = 0; i < MAX_TGA; i++)
	{
		if (g_MHTga[g_Tga[i].iMHTgaID].texid==-1)
			continue;

		if (!g_Tga[i].flEndDisplayTime)
			continue;

		if (time > g_Tga[i].flEndDisplayTime)
		{
			g_Tga[i].flEndDisplayTime = 0.0;
			continue;
		}

		int r, g, b;
		r = g_Tga[i].r;
		g = g_Tga[i].g;
		b = g_Tga[i].b;
		float fA;

		switch (g_Tga[i].iMode)
		{
			case 1:
			{
				float flHasDisplayTime = time - g_Tga[i].flStartDisplayTime;
				float flNeedDisplayTime = g_Tga[i].flEndDisplayTime - g_Tga[i].flStartDisplayTime;
				fA = flHasDisplayTime / flNeedDisplayTime;
				break;
			}
			case 2:
			{
				float flHasDisplayTime = time - g_Tga[i].flStartDisplayTime;
				float flNeedDisplayTime = g_Tga[i].flEndDisplayTime - g_Tga[i].flStartDisplayTime;
				fA = 1.0 - (flHasDisplayTime / flNeedDisplayTime);
				break;
			}
			case 3:
			{
				float flHasDisplayTime = time - g_Tga[i].flStartDisplayTime;
				float flNeedDisplayTime = g_Tga[i].flEndDisplayTime - g_Tga[i].flStartDisplayTime;
				fA = (flHasDisplayTime / flNeedDisplayTime * 2.0);
				if (fA > 1.0)
					fA = 2.0-fA;
				break;
			}
			default: 
			{
				fA = 1.0;
				break;
			}
		}
		//
		
		static int iX,iY,width,height;
		if(g_Tga[i].w>0 && g_Tga[i].h>0)
		{
			width = g_Tga[i].w;
			height = g_Tga[i].h;
		}
		else
		{
			width = g_MHTga[g_Tga[i].iMHTgaID].width * g_Tga[i].fScale ;
			height = g_MHTga[g_Tga[i].iMHTgaID].height * g_Tga[i].fScale ;
		}

		if (g_Tga[i].iCenter)
		{
			iX = g_Tga[i].x - width/2;
			iY = g_Tga[i].y - height/2;
		}
		else
		{
			iX = g_Tga[i].x;
			iY = g_Tga[i].y;
		}

		Tri_Enable(GL_TEXTURE_2D);
		Tri_BindTexture(GL_TEXTURE_2D, g_MHTga[g_Tga[i].iMHTgaID].texid);
		if(g_iVideoMode)
		{
			glColor4ub(r,g,b,fA*255);
		}
		else gEngfuncs.pTriAPI->Color4ub(r,g,b,255*fA);
		Tri_Enable(GL_BLEND);
		Tri_Enable(GL_ALPHA_TEST);
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		Tri_AlphaFunc(GL_GREATER, 0.0);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		
		gEngfuncs.pTriAPI->TexCoord2f(0,1);
		gEngfuncs.pTriAPI->Vertex3f(iX,iY+height,0);

		gEngfuncs.pTriAPI->TexCoord2f(1,1);
		gEngfuncs.pTriAPI->Vertex3f(iX+width,iY+height,0);
		
		gEngfuncs.pTriAPI->TexCoord2f(1,0);
		gEngfuncs.pTriAPI->Vertex3f(iX+width,iY,0);
		
		gEngfuncs.pTriAPI->TexCoord2f(0,0);
		gEngfuncs.pTriAPI->Vertex3f(iX,iY,0);
		gEngfuncs.pTriAPI->End();
	}
}

int CHudTGAElements::FindTexture(char *pszName)
{
	for (auto iter = g_Texture.cbegin(); iter != g_Texture.cend(); ++iter)
	{
		if (!strcmp(pszName, iter->szName))
		{
			return iter - g_Texture.cbegin();
		}
	}
	
	Tga Data;
	Data.iTexture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(Data.iTexture, pszName, true, true);
	strcpy(Data.szName, pszName);
	vgui::surface()->DrawGetTextureSize(Data.iTexture, Data.iWidth, Data.iHeight);
	g_Texture.push_back(Data);
	return g_Texture.size() - 1;
}
