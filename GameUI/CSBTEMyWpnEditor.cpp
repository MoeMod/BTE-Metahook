#include "base.h"
#include "CSBTEMyWpnEditor.h"
#include "common.h"
#include <io.h>

CCSBTEMyWpnElement::CCSBTEMyWpnElement(Panel *parent, const char *name) : ImagePanel(parent, name)
{
	SetImage(scheme()->GetImage("resource/itembox", false));
	m_bShowDesc = false;

	m_pText = new Label(this, "Label", L"柠檬大屌鸡");
	m_pText->SetPaintBackgroundEnabled(false);
	m_pText->SetBounds(10, 80, 200, 20);
	int textwide, texttall;
	m_pText->GetContentSize(textwide, texttall);
	m_pText->SetMouseInputEnabled(false);

	m_pText2 = new Label(this, "Label", "#CSO_Item_Active_Format");
	m_pText2->SetPaintBackgroundEnabled(false);
	m_pText2->SetBounds(10, 96, 200, 20);
	m_pText2->SetFgColor(Color(130, 130, 130, 130));
	m_pText2->SetMouseInputEnabled(false);

	m_pSelectedBackground = new ImagePanel(this, "CSBTESelectedBackground");
	m_pSelectedBackground->SetImage(scheme()->GetImage("resource/set/itemboxselect", false));
	m_pSelectedBackground->SetBounds(0, 0, 235, 134);
	m_pSelectedBackground->SetVisible(false);
	m_pSelectedBackground->SetMouseInputEnabled(false);
	
	m_pItemBackground = new ImagePanel(this, "CSBTEItemBackground");
	m_pItemBackground->SetImage(scheme()->GetImage("gfx/ui/panel/basket_blank_slot", false));
	m_pItemBackground->SetBounds(0, 0, 235, 81);
	m_pItemBackground->SetShouldScaleImage(true);
	m_pItemBackground->SetMouseInputEnabled(false);

	m_pWeaponImage = new ImagePanel(this, "CSBTEWeaponImage");
	m_pWeaponImage->SetImage(scheme()->GetImage("gfx/vgui/cannonex", false));
	m_pWeaponImage->SetBounds(0, 0, 235, 81);
	m_pWeaponImage->SetMouseInputEnabled(false);

	m_pTeamImage = new ImagePanel(this, "CSBTETeamImage");
	m_pTeamImage->SetImage(scheme()->GetImage("resource/teamtype0", false));
	m_pTeamImage->SetBounds(180, 60, 41, 11);
	m_pTeamImage->SetMouseInputEnabled(false);
	
	m_pTextLine = new ImagePanel(this, "CSBTETextLine");
	m_pTextLine->SetImage(scheme()->GetImage("resource/itemline", false));
	m_pTextLine->SetBounds(10, 97, textwide, 1);
	m_pTextLine->SetShouldScaleImage(true);
	m_pTextLine->SetMouseInputEnabled(false);

	m_pApplyButton = new Button(this, "CSBTEModeButton", "#CSO_Equip_Btn");
	m_pApplyButton->SetContentAlignment(Label::a_center);
	m_pApplyButton->SetBounds(160, 110, 60, 24);
	m_pApplyButton->SetContentAlignment(Label::a_center);
	m_pApplyButton->AddActionSignalTarget(this->GetParent());

	m_pDescPanel = new CSBTEDescPanel("假装有内容");
}

void CCSBTEMyWpnElement::SetId(int id)
{
	char szCommand[18];
	sprintf(szCommand, "OnWeaponApply_%d", id);
	m_pApplyButton->SetCommand(szCommand);

	int x = 35 + (id & 1) * 240;
	int y = 40 + (id >> 1) * 140;
	SetBounds(x, y, 235, 134);

}

extern wchar_t *GetWeaponNameFormat(char *name);
void CCSBTEMyWpnElement::SetWeapon(char *szWeaponName, int iTeam)
{
	char szPath[128];
	sprintf(szPath, "gfx/vgui/%s", szWeaponName);
	IImage *pImage = scheme()->GetImage(szPath, false);
	m_pWeaponImage->SetImage(pImage);

	int w, h;
	pImage->GetSize(w, h);
	m_pWeaponImage->SetBounds((235 - w) / 2, (81 - h) / 2, 235, 81);
	
	m_pTeamImage->SetImage(scheme()->GetImage(va("resource/teamtype%i", iTeam), false));
	
	m_pText->SetText(GetWeaponNameFormat(szWeaponName));
	int textwide, texttall;
	m_pText->GetContentSize(textwide, texttall);
	m_pTextLine->SetBounds(10, 97, textwide, 1);
	
	m_pDescPanel->SetSize(240, 240);
	m_pDescPanel->SetDrawWidth(240);
	m_pDescPanel->SetText(va("#CSO_Item_Desc_%s", szWeaponName));
	
}

void CCSBTEMyWpnElement::SetSelected(bool bSelected)
{
	if (bSelected)
	{
		m_pSelectedBackground->SetVisible(true);
		m_pText2->SetText("#CSO_Item_Active_Format");
		m_pApplyButton->SetText("#CSO_Equip_Btn");
	}
	else
	{
		m_pSelectedBackground->SetVisible(false);
		m_pText2->SetText("");
		m_pApplyButton->SetText("#CSO_Use");
	}
		
}

void CCSBTEMyWpnElement::OnMouseDoublePressed(MouseCode code)
{
	if (code == MOUSE_LEFT)
		m_pApplyButton->DoClick();
}

void CCSBTEMyWpnElement::OnCursorEntered()
{
	m_bShowDesc = true;
}

void CCSBTEMyWpnElement::OnCursorExited()
{
	m_bShowDesc = false;
}

void CCSBTEMyWpnElement::OnCursorMoved(int x, int y)
{
	input()->GetCursorPos(x, y);
	m_pDescPanel->SetPos(x, y);
	m_pDescPanel->Paint();
}

char WEAPON_TYPE[8][32] = { "#CSO_Whole", "#CSO_Inventory_Pistol", "#CSO_Inventory_ShotGun", "#CSO_Inventory_SubMachineGun",
"#CSO_Inventory_Rifle", "#CSO_Inventory_MachineGun", "#CSO_Inventory_Knife", "#CSO_Inventory_Equipment" };

CCSBTEMyWpnEditor::CCSBTEMyWpnEditor(Panel *parent, const char *panelName, bool showTaskbarIcon) : Frame(parent, panelName, showTaskbarIcon)
{
	int sw, sh;
	surface()->GetScreenSize(sw, sh);

	SetTitle("", false);

	SetBounds(0, 0, 575, 600);
	MoveToCenterOfScreen();
	SetSizeable(false);
	SetVisible(true);
	
	m_pPropertySheet = new PropertySheet(this, "PropertySheet");
	m_pPropertySheet->SetBounds(25, 15, 525, 25);
	m_pPropertySheet->AddActionSignalTarget(this);
	m_pPropertySheet->SetPaintBackgroundEnabled(false);

	for (int i = 0; i < 8; i++)
	{
		m_pPropertyPage[i] = new CCSBTEMyWpnPropertyPage(this, "PropertyPage");
		m_pPropertyPage[i]->AddActionSignalTarget(this);
		m_pPropertySheet->AddPage(m_pPropertyPage[i], WEAPON_TYPE[i]);
	}

	for (int i = 0; i < 8; i++)
	{
		m_pElements[i] = new CCSBTEMyWpnElement(this, "CSBTEMyWpnElement");
		
		m_pElements[i]->SetId(i);
		m_pElements[i]->AddActionSignalTarget(this);
	}

	/*
	m_pPageText = new Label(this, "Label", L"0/0");
	m_pPageText->SetPaintBackgroundEnabled(false);
	m_pPageText->SetBounds(270, 640, 100, 50);
	m_pPageText->SetContentAlignment(Label::a_center);*/

	m_pScrollBar = new ScrollBar(this, "VerticalScrollBar", true);
	m_pScrollBar->SetBounds(520, 40, 20, 560);
	m_pScrollBar->AddActionSignalTarget(this);
	m_pScrollBar->SetWide(16);
	

	m_WeaponInfoList.clear();

	char szLineData[32];
	FILE *pFile = NULL;
	WeaponInfo Temp;

	pFile = fopen("cstrike\\addons\\amxmodx\\configs\\weapons.lst", "r");
	if (pFile != NULL)
	{
		while (!feof(pFile))
		{
			fscanf(pFile, "%s", szLineData);
			if (szLineData[0] == ';')
				continue;
			if (!szLineData[0])
				continue;
			/*
			if (g_WeaponData[FindWeaponData(szLineData)].bCanBuy == false)
				continue;
				*/

			Q_strcpy(Temp.szName, szLineData);

			char szBuffer[16];
			GetPrivateProfileString(Temp.szName, "TEAM", "ALL", szBuffer, sizeof(szBuffer), "cstrike/weapons.ini");
			if (szBuffer[0] == 'A')
				Temp.iTeam = ALL;
			else if (szBuffer[0] == 'C')
				Temp.iTeam = CT;
			else if (szBuffer[0] == 'T')
				Temp.iTeam = TR;

			GetPrivateProfileString(Temp.szName, "MENU", "NULL", szBuffer, sizeof(szBuffer), "cstrike/weapons.ini");
			if (szBuffer[0] == 'P')
				Temp.iMenu = PISTOL;
			else if (szBuffer[0] == 'S' && szBuffer[1] == 'H')
				Temp.iMenu = SHOTGUN;
			else if (szBuffer[0] == 'S' && szBuffer[1] == 'M')
				Temp.iMenu = SMG;
			else if (szBuffer[0] == 'R')
				Temp.iMenu = RIFLE;
			else if (szBuffer[0] == 'M')
				Temp.iMenu = MG;
			else if (szBuffer[0] == 'K')
				Temp.iMenu = KNIFE;
			else if (szBuffer[0] == 'E')
				Temp.iMenu = EQUIP;
			else
				Temp.iMenu = NONE;

			Temp.isMyWpn = false;
			
			m_WeaponInfoList.push_back(Temp);
		}
		fclose(pFile);
		pFile = NULL;
	}

	pFile = fopen("cstrike\\addons\\amxmodx\\configs\\weapons_precache.lst", "r");
	if (pFile != NULL)
	{
		while (!feof(pFile))
		{
			fscanf(pFile, "%s", szLineData);
			if (!Q_strncmp(szLineData, "true", 4) || !Q_strncmp(szLineData, "false", 5))
				continue;
			for (auto &x : m_WeaponInfoList)
			{
				if (szLineData[0] == ';')
					continue;
				if (!szLineData[0])
					continue;
				if (!Q_stricmp(x.szName, szLineData))
				{
					x.isMyWpn = true;
					break;
				}
			}
		}
		fclose(pFile);
		pFile = NULL;
	}

	OnPageTabActivated(m_pPropertyPage[0]);
}

void CCSBTEMyWpnEditor::PerformLayout()
{
	BaseClass::PerformLayout();

	
	m_pScrollBar->SetRange(0, m_WeaponInfoDisplayId.size() / 2 + m_WeaponInfoDisplayId.size() % 2 + 1);
	m_pScrollBar->SetRangeWindow(4);
	m_pScrollBar->SetButtonPressedScrollValue(4);
}

//-----------------------------------------------------------------------------
// Called when the scroll bar moves
//-----------------------------------------------------------------------------
void CCSBTEMyWpnEditor::OnScrollBarSliderMoved()
{
	InvalidateLayout();

	int nScrollAmount = m_pScrollBar->GetValue();
	SwitchToLine(nScrollAmount);
}

void CCSBTEMyWpnEditor::OnMouseWheeled(int delta)
{
	int val = m_pScrollBar->GetValue();
	val -= (delta);
	m_pScrollBar->SetValue(val);
}

void CCSBTEMyWpnPropertyPage::OnPageTabActivated(Panel *pageTab)
{
	((CCSBTEMyWpnEditor *)(GetParent()->GetParent()))->OnPageTabActivated(this);
	
}

void CCSBTEMyWpnEditor::OnPageTabActivated(vgui::Panel *panel)
{
	int iPage = 0;
	for (int i = 0; i < 8; i++)
	{
		if (m_pPropertyPage[i] == panel)
		{
			iPage = i;
			break;
		}
	}
	m_WeaponInfoDisplayId.clear();
	for (auto x = m_WeaponInfoList.cbegin(); x != m_WeaponInfoList.cend();x++)
	{
		if (!iPage || x->iMenu == iPage)
			m_WeaponInfoDisplayId.push_back(x - m_WeaponInfoList.cbegin());
	}

	PerformLayout();
	SwitchToLine(0);
	m_pScrollBar->SetValue(0);
}

void CCSBTEMyWpnEditor::SwitchToLine(int iLine)
{
	int iDisplayCount = m_WeaponInfoDisplayId.size();

	if (iLine < 0 || iLine > iDisplayCount / 2 + iDisplayCount % 2 + 1)
		return;

	m_iLine = iLine;

	for (int iSlot = 0; iSlot < 8; iSlot++)
	{
		if (m_iLine * 2 + iSlot >= iDisplayCount)
		{
			m_pElements[iSlot]->SetVisible(false);
		}
		else
		{ 
			WeaponInfo &Weapon = m_WeaponInfoList[m_WeaponInfoDisplayId[m_iLine * 2 + iSlot]];
		
			m_pElements[iSlot]->SetVisible(true);
			m_pElements[iSlot]->SetWeapon(Weapon.szName, Weapon.iTeam);
			m_pElements[iSlot]->SetSelected(Weapon.isMyWpn);
		}
	}
}

void CCSBTEMyWpnEditor::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{ 
		Close();
	}
	else if (!Q_strncmp(command, "OnWeaponApply_", 14))
	{
		int iSlot;
		sscanf(command, "OnWeaponApply_%d", &iSlot);

		//int iWeapon = m_iWeaponDisplayID[m_iLine * 2 + iSlot];
		WeaponInfo &Weapon = m_WeaponInfoList[m_WeaponInfoDisplayId[m_iLine * 2 + iSlot]];

		if (Weapon.isMyWpn)
		{
			Weapon.isMyWpn = false;
			
			if (!MyWpnRewrite())
			{
				MessageBox *msgbox;
				msgbox = new MessageBox("", "#CSBTE_MyWpnEditor_ErrorAdd", this);
				msgbox->SetOKButtonText("#GameUI_OK");
				msgbox->SetOKButtonVisible(true);
				msgbox->SetBounds(GetWide() / 2 - 150, GetTall() / 2 - 100, 300, 200);

				msgbox->Activate();

				Weapon.isMyWpn = true;
				
			}
		}
		else
		{
			Weapon.isMyWpn = true;
			if (!MyWpnRewrite())
			{
				MessageBox *msgbox;
				msgbox = new MessageBox("", "#CSBTE_MyWpnEditor_ErrorRemove", this);
				msgbox->SetOKButtonText("#GameUI_OK");
				msgbox->SetOKButtonVisible(true);
				msgbox->SetBounds(GetWide() / 2 - 150, GetTall() / 2 - 100, 300, 200);

				msgbox->Activate();

				Weapon.isMyWpn = false;
			}
		}
		SwitchToLine(m_iLine);
	}
	else
		BaseClass::OnCommand(command);
}

bool CCSBTEMyWpnEditor::MyWpnRewrite()
{
	FILE *pFile = fopen("cstrike\\addons\\amxmodx\\configs\\weapons_precache.lst", "w");
	if (pFile == NULL)
		return false;
	
	fprintf(pFile, "true\n");
	for (auto &x:m_WeaponInfoList)
	{
		if (!x.isMyWpn)
			continue;
		fprintf(pFile, "%s\n", x.szName);
	}
	fclose(pFile);
	return true;
}