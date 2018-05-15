#include "metahook.h"
#include "bte_const.h"
#include "plugins.h"
#include "triangleapi.h"
#include "imagelib.h"
#include "gl/gl.h"
#include "TriAPI.h"
#include "util.h"
#include "common.h"
#include "LoadTexture.h"
#include "CSBTEMapLoading.h"

#include <io.h>

#define MAX_GAMEMODE_PLUGIN 17
char *g_szModePlugin[MAX_GAMEMODE_PLUGIN] = { "", "none", "td", "zb1", "zbs", "zbu", "hms", "ghost", "gd", "dr", "ze", "zb3", "zb4", "zb2", "dm", "zse", "zb5" };

int GetLocalGameMode()
{
	int iCurMode = -1;
	char szFilename[128];
	for (int i = 0; i < MAX_GAMEMODE_PLUGIN; i++)
	{
		sprintf(szFilename, "cstrike\\addons\\amxmodx\\configs\\plugins-%s.ini", g_szModePlugin[i]);
		if (access(szFilename, 4) != -1)
		{
			if (iCurMode == -1)
				iCurMode = i;
			else
				iCurMode = -2;
		}
	}

	if (iCurMode > 0)
		return iCurMode;
	return 0;
}

CCSBTEMapLoading::CCSBTEMapLoading(Panel *parent, const char *name) : ImagePanel(parent, name)
{
	int ScreenWidth, ScreenHeight;
	surface()->GetScreenSize(ScreenWidth, ScreenHeight);

	m_setPanels.clear();
	//for (int i = 0; i < 18; i++)
	for (auto &p : m_szOriginIcon)
	{
		p = nullptr;
	}
	m_szMapName = new char[2];
	m_szMapName[0] = '\0';
	m_iBgTexIndex = vgui::surface()->CreateNewTextureID();

	SetShouldScaleImage(true);
	SetVisible(false);

	m_pToolTip = new ImagePanel(this, "CSBTEMapLoadingToolTip");
	m_pToolTip->SetShouldScaleImage(true);
	m_pToolTip->SetVisible(false);

	m_pTaskBar = new ImagePanel(this, "ImagePanel");

	m_pTaskBar->SetImage(scheme()->GetImage("resource/taskbar", false));
	m_pTaskBar->SetBounds(0, 0, ScreenWidth, 64);
	m_pTaskBar->SetVisible(true);
	m_pTaskBar->SetShouldScaleImage(true);

	m_pStatusText = new Label(this, "Label", L"");
	m_pStatusText->SetBounds(65 * ScreenWidth / 1024.0, ScreenHeight - 65 * ScreenHeight / 768.0, 200, 15);
	m_pStatusText->SetPaintBackgroundEnabled(false);
	m_pStatusText->SetVisible(true);
	m_pStatusText->SetFgColor(Color(255, 255, 255, 50));
	m_pStatusText->SetTextColorState(Label::CS_DULL);

	m_pTipText = new Label(this, "Label", L"");
	m_pTipText->SetBounds(340 * ScreenWidth / 1024.0, ScreenHeight - 340 * ScreenHeight / 768.0, 400, 400);
	m_pTipText->SetPaintBackgroundEnabled(false);
	m_pTipText->SetVisible(true);
	//m_pTipText->SetFgColor(Color(75, 75, 75, 75));
	m_pTipText->SetTextColorState(Label::CS_DULL);

	m_pServerName = new Label(this, "Label", L"Counter-Strike : BreakThrough Edition - Loading");
	m_pServerName->SetBounds(12 * ScreenWidth / 1024.0, 0 * ScreenHeight / 768.0, 400, 50);
	m_pServerName->SetPaintBackgroundEnabled(false);
	m_pServerName->SetVisible(true);

	m_pProgress = new ProgressBar(this, "Progress");
	m_pProgress->SetBounds(60 * ScreenWidth / 1024.0, ScreenHeight - 40 * ScreenHeight / 768.0, ScreenWidth - 60 * 2 * ScreenWidth / 1024.0, 26);
	m_pProgress->SetPaintBackgroundEnabled(false);
	m_pProgress->SetFgColor(Color(75, 75, 75, 75));
	m_pProgress->SetVisible(false);
}

CCSBTEMapLoading::~CCSBTEMapLoading()
{
	ClearOrigins();
	delete[] m_szMapName;
}

void CCSBTEMapLoading::SetStatusText(const char *szStatusText)
{
	m_pStatusText->SetText(szStatusText);
}

void CCSBTEMapLoading::SetServerName(const char *szServerName)
{
	m_pServerName->SetText(szServerName);
}

bool CCSBTEMapLoading::SetProgressPoint(int progressPoint)
{
	int nOldDrawnSegments = m_pProgress->GetDrawnSegmentCount();
	m_pProgress->SetProgress((float)progressPoint / (m_iRangeMax - m_iRangeMin));
	int nNewDrawSegments = m_pProgress->GetDrawnSegmentCount();
	return (nOldDrawnSegments != nNewDrawSegments);
}

void CCSBTEMapLoading::SetProgressRange(int min, int max)
{
	m_iRangeMin = min;
	m_iRangeMax = max;
}

void CCSBTEMapLoading::SetProgressVisible(bool bSet)
{
	m_pProgress->SetVisible(bSet);
}

void CCSBTEMapLoading::PaintBackground()
{
	Panel::PaintBackground();
	
	Tri_Enable(GL_TEXTURE_2D);
	Tri_BindTexture(GL_TEXTURE_2D, m_iBgTexIndex);
	if (g_iVideoMode)
	{
		glColor4ub(255, 255, 255, 255);
	}
	else gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	
	Tri_Enable(GL_BLEND);
	Tri_Enable(GL_ALPHA_TEST);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Tri_AlphaFunc(GL_GREATER, 0.0);
	
	int ScreenWidth, ScreenHeight;
	surface()->GetScreenSize(ScreenWidth, ScreenHeight);
	/*
	gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
	vgui::surface()->DrawSetTexture(m_iBgTexIndex);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	vgui::surface()->DrawTexturedRect(0, 0, ScreenWidth, ScreenHeight);
	*/
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(0, float(ScreenHeight), 0);
	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(float(ScreenWidth), float(ScreenHeight), 0);
	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(float(ScreenWidth), 0, 0);
	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(0, 0, 0);
	gEngfuncs.pTriAPI->End();
}

bool CCSBTEMapLoading::SetMapName(const char *name)
{
	delete[] m_szMapName;
	m_szMapName = new char[strlen(name) + 1];
	strcpy(m_szMapName, name);

	
	if (access(va("cstrike\\resource\\maploading\\loadingbg_%s.tga", name), 4) == -1)
	{
		return false;
	}

	SetImage(scheme()->GetImage(va("resource/maploading/loadingbg_%s", name), false));

	//imagedata_t ImageData;
	//Image_LoadImage(va("cstrike\\resource\\maploading\\loadingbg_%s.tga", name), &ImageData, &m_iBgTexIndex);
	m_iBgTexIndex = GL_LoadTexture(va("resource\\maploading\\loadingbg_%s.tga", name));

	int ScreenWidth, ScreenHeight;
	surface()->GetScreenSize(ScreenWidth, ScreenHeight);
	SetBounds(0, 0, ScreenWidth, ScreenHeight);
	
	m_pStatusText->SetVisible(true);
	SetVisible(true);
	MakePopup(false, false);
	MoveToFront();

	UpdateGameMode(GetLocalGameMode());
	UpdateOrigins(name);
	return true;
}

void CCSBTEMapLoading::ClearOrigins()
{
	for (auto pPanel : m_setPanels)
	{
		delete pPanel;
	}
	m_setPanels.clear();

	for (int i = 0; i < 18; i++)
	{
		delete[] m_szOriginIcon[i];
	}
}

void CCSBTEMapLoading::AddOrigin(int iType, int x, int y)
{
	if (iType < 0 || iType >= 18)
		return;
	if (!m_szOriginIcon[iType])
		return;

	if ((m_iGameMode == MOD_ZB) || (m_iGameMode == MOD_ZB2) || (m_iGameMode == MOD_ZB3) || (m_iGameMode == MOD_ZB4) || (m_iGameMode == MOD_ZE) || (m_iGameMode == MOD_ZSE) || (m_iGameMode == MOD_ZB5) || (m_iGameMode == MOD_DM) || (m_iGameMode == MOD_ZBS) || (m_iGameMode == MOD_ZB) || (m_iGameMode == MOD_Z4E))
	{
		if (!stricmp(m_szOriginIcon[iType], "CT") || !stricmp(m_szOriginIcon[iType], "TR"))
			return;
	}
	if ((m_iGameMode != MOD_NONE))
	{
		if (!stricmp(m_szOriginIcon[iType], "C4A") || !stricmp(m_szOriginIcon[iType], "C4B") || !stricmp(m_szOriginIcon[iType], "Hostage") || !stricmp(m_szOriginIcon[iType], "Rescue") || !stricmp(m_szOriginIcon[iType], "VIP"))
			return;
	}
	if ((m_iGameMode != MOD_ZB4))
	{
		if (!stricmp(m_szOriginIcon[iType], "weapon"))
			return;
	}

	IImage *pImage = scheme()->GetImage(va("resource/maploading/icon/loadingicon_%s", m_szOriginIcon[iType]), false);
	if (!pImage)
		return;

	ImagePanel *pPanel = new ImagePanel(this, "CSBTEMapLoadingOrigin");
	m_setPanels.insert(pPanel);

	int w, h;
	pImage->GetSize(w, h);
	pPanel->SetImage(pImage);
	//pPanel->SetBounds((x + w / 2) * ScreenWidth / 1024.0, (y + h / 2) * ScreenHeight / 768.0, w, h);
	int ScreenWidth, ScreenHeight;
	surface()->GetScreenSize(ScreenWidth, ScreenHeight);
	pPanel->SetBounds((x + w / 2) * ScreenWidth / 1024.0 - w / 2, (y + h / 2)* ScreenHeight / 768.0 - h / 2, w, h);
	pPanel->SetShouldScaleImage(false);
	pPanel->SetVisible(true);
}

bool CCSBTEMapLoading::UpdateOrigins(const char *name)
{
	ClearOrigins();

	FILE *pFile = fopen("cstrike\\resource\\maploading\\maploading.csv", "r");
	if (!pFile)
		return false;
	char szBuffer[256], iLine = 0;
	while (!feof(pFile))
	{
		fgets(szBuffer, sizeof(szBuffer), pFile);
		
		int iCurSection = 0;

		char *pCurrentPos = strtok(szBuffer, ",\n");
		char *pLastPos = szBuffer;

		int iIconType, iX, iY;
		do
		{
			if (!iLine && iCurSection<18)
			{
				m_szOriginIcon[iCurSection] = new char[strlen(pLastPos) + 1];
				strcpy(m_szOriginIcon[iCurSection], pLastPos);
			}
			else if (iCurSection == 1)
			{
				if (stricmp(pLastPos, name))
				{ 
					break;
				}
			}
			else if (iCurSection > 2)
			{
				if (sscanf(pLastPos, "%d %d %d", &iIconType, &iX, &iY) == 3)
				{
					AddOrigin(iIconType, iX, iY);
				}
			}

			
			iCurSection++;
		} while (pLastPos = strtok(NULL, ",\n"));

		iLine++;
	}
	return true;
}

bool CCSBTEMapLoading::UpdateGameMode(int iGameMode)
{
	IImage *pImage = NULL;
	char *pszText = NULL;

	m_iGameMode = iGameMode;

	if (iGameMode == MOD_ZB)
	{
		pImage = scheme()->GetImage("resource/maploading/tip/tooltip_zombie", false);
		pszText = "#CSO_MaploadingTip_ZM";
	}
	else if (iGameMode == MOD_ZB2)
	{
		pImage = scheme()->GetImage(va("resource/maploading/tip/tooltip_zombie2_0%d", RANDOM_LONG(1, 5)), false);
		pszText = "#CSO_MaploadingTip_ZM2";
	}
	else if (iGameMode == MOD_ZB3)
	{
		pImage = scheme()->GetImage(va("resource/maploading/tip/tooltip_zombie3_0%d", RANDOM_LONG(1, 3)), false);
		pszText = "#CSO_MaploadingTip_ZM3";
	}
	else if (iGameMode == MOD_ZB4)
	{
		pImage = scheme()->GetImage(va("resource/maploading/tip/tooltip_zombie4_0%d", RANDOM_LONG(1, 7)), false);
		pszText = "#CSO_MaploadingTip_ZM4";
	}
	else if (iGameMode == MOD_ZE || iGameMode == MOD_Z4E)
	{
		pImage = scheme()->GetImage(va("resource/maploading/tip/tooltip_zombieescape_0%d", RANDOM_LONG(1, 3)), false);
		pszText = "#CSO_MaploadingTip_ZE";
	}
	else if (iGameMode == MOD_TDM)
	{
		pImage = scheme()->GetImage("resource/maploading/tip/tooltip_teamdeathmatch", false);
		pszText = "#CSO_MaploadingTip_TDM";
	}
	else if (iGameMode == MOD_DM)
	{
		pImage = scheme()->GetImage("resource/maploading/tip/tooltip_deathmatch", false);
		pszText = "#CSO_MaploadingTip_DM";
	}
	else if (iGameMode == MOD_GD)
	{
		pImage = scheme()->GetImage(va("resource/maploading/tip/tooltip_gdm_0%d", RANDOM_LONG(1, 2)), false);
		pszText = va("#CSO_MaploadingTip_GDM_0%i", RANDOM_LONG(1, 2));
	}
	else if (iGameMode == MOD_ZBU)
	{
		pImage = scheme()->GetImage("resource/maploading/tip/tooltip_zombieunite", false);
		pszText = "#CSO_MaploadingTip_ZTDM";
	}
	else if (iGameMode == MOD_NONE || iGameMode == MOD_GHOST)
	{
		if (!strnicmp(m_szMapName, "de_", 3))
		{
			pImage = scheme()->GetImage("resource/maploading/tip/tooltip_original_de", false);
			pszText = "#CSO_MaploadingTip_ORIGINAL_DE";
		}
		else if (!strnicmp(m_szMapName, "dm_", 3))
		{
			pImage = scheme()->GetImage("resource/maploading/tip/tooltip_original_dm", false);
			pszText = "#CSO_MaploadingTip_ORIGINAL_DM";
		}
		else if (!strnicmp(m_szMapName, "as_", 3))
		{
			pImage = scheme()->GetImage("resource/maploading/tip/tooltip_original_as", false);
			pszText = "#CSO_MaploadingTip_ORIGINAL_AS";
		}
		else if (!strnicmp(m_szMapName, "cs_", 3))
		{
			pImage = scheme()->GetImage("resource/maploading/tip/tooltip_original_cs", false);
			pszText = "#CSO_MaploadingTip_ORIGINAL_CS";
		}
		else if (!strnicmp(m_szMapName, "fy_", 3))
		{
			pImage = scheme()->GetImage("resource/maploading/tip/tooltip_fightyard", false);
			pszText = "#CSO_MaploadingTip_FightYard";
		}
	}

	if (pszText)
	{
		m_pTipText->SetText(pszText);
		m_pTipText->SetVisible(true);
		int w, h;
		m_pTipText->GetContentSize(w, h);

		int ScreenWidth, ScreenHeight;
		surface()->GetScreenSize(ScreenWidth, ScreenHeight);
		m_pTipText->SetBounds(340 * ScreenWidth / 1024.0, ScreenHeight - h -(50) * ScreenHeight / 768.0, w, h);
	}
	else
	{
		m_pTipText->SetVisible(true);
	}


	if (pImage)
	{
		int iWidth, iHeight;
		pImage->GetSize(iWidth, iHeight);

		m_pToolTip->SetBounds(15, 70, iWidth, iHeight);
		m_pToolTip->SetImage(pImage);
		m_pToolTip->SetVisible(true);
		return true;
	}
	else
	{
		m_pToolTip->SetVisible(false);
		return false;
	}
	
}