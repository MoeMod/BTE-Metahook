#include "base.h"
#include "CSBTEWpnDataEditor.h"
#include <vgui_controls\PHandle.h>
#include "common.h"
#include <io.h>
#include <string>
#include <fstream>

//char WEAPON_TYPE[8][32] = { "#CSO_Whole", "#CSO_Inventory_Pistol", "#CSO_Inventory_ShotGun", "#CSO_Inventory_SubMachineGun", "#CSO_Inventory_Rifle", "#CSO_Inventory_MachineGun", "#CSO_Inventory_Knife", "#CSO_Inventory_Equipment" };
extern wchar_t *GetWeaponNameFormat(const std::string &name);
CCSBTEWpnDataEditor::CCSBTEWpnDataEditor(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: Frame(parent, panelName, showTaskbarIcon), m_iniData("weapons.ini"), m_iniDataIterator(m_iniData.begin())
{
	int sw, sh;
	surface()->GetScreenSize(sw, sh);

	SetTitle("#CSBTE_WpnDataEditor_Title", false);

	SetSize(740, 700);
	MoveToCenterOfScreen();
	SetSizeable(false);
	SetVisible(true);

	m_pCancelButton = new Button(this, "CancelButton", "#GameUI_Close");
	m_pCancelButton->SetContentAlignment(Label::a_center);
	m_pCancelButton->SetBounds(260, 625, 200, 50);
	m_pCancelButton->SetCommand("vguicancel");
	m_pCancelButton->SetVisible(true);

	m_pNextWpn = new Button(this, "NextWeapon", "#CSBTE_WpnDataEditor_Next");
	m_pNextWpn->SetContentAlignment(Label::a_center);
	m_pNextWpn->SetBounds(500, 625, 200, 50);
	m_pNextWpn->SetCommand("nextwpn");
	m_pNextWpn->SetVisible(true);

	m_pName = new Label(this, "WeaponName", "#CSBTE_WpnDataEditor_WpnName");
	m_pName->SetBounds(25, 30, 200, 20);
	m_pName->SetPaintBackgroundEnabled(false);

	UpdateCurrentWeapon();
}

void CCSBTEWpnDataEditor::UpdateCurrentWeapon()
{
	//Read Weapons Data//
	auto &kv = *m_iniDataIterator; // dereference iterator to get weapon data

	auto &wpnName = kv.first;
	auto &wpnData = kv.second;

	m_pName->SetText(GetWeaponNameFormat(wpnName));

	//m_pDamage = new Label(this, "WeaponsDamage", wpnData.c_str());
}

void CCSBTEWpnDataEditor::NextWpn()
{
	//Read Next Weapons//
	++m_iniDataIterator;

	if (m_iniDataIterator == m_iniData.end())
	{
		CCSBTEWpnDataEditorMessageBox *msgbox;
		msgbox = new CCSBTEWpnDataEditorMessageBox(L"#CSBTE_WpnDataEditor_Warning", L"#CSBTE_WpnDataEditor_WarningMsg", this);
		msgbox->SetOKButtonText(L"Yes");
		msgbox->SetCancelButtonText(L"Cancel");
		msgbox->SetOKButtonVisible(true);
		msgbox->SetCancelButtonVisible(true);
		msgbox->SetBounds(GetWide() / 2 - 150, GetTall() / 2 - 100, 300, 200);
		msgbox->Activate();

		// reset iterator to begin
		m_iniDataIterator = m_iniData.begin();
	}

	UpdateCurrentWeapon();

}

void CCSBTEWpnDataEditor::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{
		Close();
	}
	else if (!Q_stricmp(command, "nextwpn"))
	{
		NextWpn();
	}
}