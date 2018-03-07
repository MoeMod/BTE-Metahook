#include "base.h"
#include "CSBTEZombieDNA.h"
#include <vgui_controls\PHandle.h>
#include <bits32.h>
#include <common.h>

const int CCSBTEZombieDNA::m_iDNAButtonPos[19][2] = {
	{ 175, 200 },
	{ 125, 250 },
	{ 125, 315 },
	{ 70, 345 },
	{ 15, 315 },
	{ 15, 250 },
	{ 70, 220 },
	{ 185, 145 },
	{ 130, 110 },
	{ 130, 45 },
	{ 185, 20 },
	{ 250, 45 },
	{ 250, 110 },
	{ 250, 250 },
	{ 305, 220 },
	{ 360, 250 },
	{ 360, 315 },
	{ 305, 345 },
	{ 250, 315 }
};

void CCSBTEZombieDNAButton::Paint(void)
{
	//BaseClass::Paint();
	return;
}

void CCSBTEZombieDNAButton::PaintBackground(void)
{
	//Panel::PaintBackground();

	int iState = m_bSelected;

	if (m_pImage[iState])
	{
		surface()->DrawSetColor(255, 255, 255, 255);
		m_pImage[iState]->SetPos(0, 0);

		// Image size is stored in the bitmap, so temporarily set its size
		// to our panel size and then restore after we draw it.

		int imageWide, imageTall;
		m_pImage[iState]->GetSize(imageWide, imageTall);
		int wide, tall;
		GetSize(wide, tall);
		m_pImage[iState]->SetSize(wide, tall);
		m_pImage[iState]->Paint();
		m_pImage[iState]->SetSize(imageWide, imageTall);

	}
}

void CCSBTEZombieDNAButton::SetId(int id)
{
	for (int i = 0; i < 2; i++)
	{
		m_pImage[i] = scheme()->GetImage(va("resource/zombiedna/zbehn_%02d_%s", id, i?"on":"off"), false);
	}
	m_iId = id;
}

void CCSBTEZombieDNAButton::SetSelected(bool x)
{
	m_bSelected = x;
}

void CCSBTEZombieDNAButton::OnMouseDoublePressed(MouseCode code)
{
	CCSBTEZombieDNA *pParent = (CCSBTEZombieDNA *)GetParent();
	if (code == MOUSE_LEFT)
		pParent->SetDNA(m_iId, !m_bSelected);
}

CCSBTEZombieDNA::CCSBTEZombieDNA(Panel *parent, const char *panelName, bool showTaskbarIcon) : Frame(parent, panelName, showTaskbarIcon)
{
	const char *pszInfo;
	pszInfo = gEngfuncs.LocalPlayerInfo_ValueForKey("bte_dna");
	m_bitsDNA = 0;
	if (pszInfo)
	{
		unsigned bitsDesiredDNA = 0;
		sscanf(pszInfo, "%d", &bitsDesiredDNA);

		for (int i = 0; i < 9; i++)
		{
			if (!bitsDesiredDNA)
				break;
			int iSlot = BitsGetFirst(bitsDesiredDNA);
			BitsUnSet(bitsDesiredDNA, iSlot);
			BitsSet(m_bitsDNA, iSlot);
		}
	}

	SetBounds(0, 0, 480, 600);
	MoveToCenterOfScreen();
	SetSizeable(false);
	SetVisible(true);
	SetTitle("#CSBTE_ZombieDNA", false);
	
	int x = 30;
	int y = 50;
	
	m_pBackground = new CBinkPanel(this, "BinkPanel");
	m_pBackground->SetBounds(x, y, 0, 0);
	m_pBackground->OpenBink("resource/zombiedna/ehnzbbg.bik", BINKSURFACE32RA);
	m_pBackground->Resize();
	m_pBackground->SetLoop(true);
	m_pBackground->Play();

	m_pCountLabel = new Label(this, "Label", L"ίχίχίχ");
	m_pCountLabel->SetBounds(x, y, 240, 20);

	for (int i = 0; i < 19; i++)
	{
		m_pDNAButton[i] = new CCSBTEZombieDNAButton(this, "DNAButton");
		if (i)
			m_pDNAButton[i]->SetBounds(x + m_iDNAButtonPos[i][0], y + m_iDNAButtonPos[i][1], 46, 46);
		else
			m_pDNAButton[i]->SetBounds(x + m_iDNAButtonPos[i][0], y + m_iDNAButtonPos[i][1], 73, 70);
		m_pDNAButton[i]->SetId(i);

		if (BitsGet(m_bitsDNA, i))
			m_pDNAButton[i]->SetSelected(true);
	}

	UpdateLabel();

	m_pCancelButton = new Button(this, "CancelButton", "#GameUI_Close");
	m_pCancelButton->SetContentAlignment(Label::a_center);
	m_pCancelButton->SetBounds(160, 520, 200, 50);
	m_pCancelButton->SetCommand("vguicancel");
}

CCSBTEZombieDNA::~CCSBTEZombieDNA()
{
	delete m_pBackground;
	delete m_pCancelButton;
	delete m_pCountLabel;
	for (int i = 0; i < 19; i++)
	{
		delete m_pDNAButton[i];
	}
}

void CCSBTEZombieDNA::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{ 
		Close();
	}
	else
		BaseClass::OnCommand(command);
}

void CCSBTEZombieDNA::SetDNA(int id, bool bEnabled)
{
	if (!!BitsGet(m_bitsDNA, id) == bEnabled)
		return;
	if (bEnabled && BitsCount(m_bitsDNA) >= MAX_DNA)
		return;

	BitsSwitch(m_bitsDNA, id);
	m_pDNAButton[id]->SetSelected(bEnabled);
	ApplySettings();
	UpdateLabel();
}

void CCSBTEZombieDNA::ApplySettings()
{
	gEngfuncs.PlayerInfo_SetValueForKey("bte_dna", va("%d", m_bitsDNA));
	gEngfuncs.pfnClientCmd(va("setinfo bte_dna %d", m_bitsDNA));

}

void CCSBTEZombieDNA::UpdateLabel()
{
	wchar_t wszBuffer[32];
	swprintf(wszBuffer, g_pVGuiLocalize->Find("CSO_DNA_RemainPoint"), MAX_DNA - BitsCount(m_bitsDNA));
	m_pCountLabel->SetText(wszBuffer);
}