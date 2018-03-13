#include "base.h"
#include "CSBTEWpnDataEditor.h"
#include <vgui_controls\PHandle.h>
#include "common.h"
#include <io.h>
#include <string>
#include <fstream>

extern wchar_t *GetWeaponNameFormat(const std::string &);

CCSBTEWpnDataEditor::CCSBTEWpnDataEditor(Panel *parent, const char *panelName, bool showTaskbarIcon) 
	: Frame(parent, panelName, showTaskbarIcon), m_iniData("weapons.ini"), m_iniDataIterator(m_iniData.begin())
{
	int sw, sh;
	surface()->GetScreenSize(sw, sh);

	SetSize(720, 500);
	MoveToCenterOfScreen();
	SetTitle("#CSBTE_WpnDataEditor_Title", false);
	SetSizeable(false);
	SetVisible(true);

	SetLayout();
}

void CCSBTEWpnDataEditor::SetLayout()
{
	msgbox = new CCSBTEWpnDataEditorMessageBox("#CSBTE_WpnDataEditor_Warning", "#CSBTE_WpnDataEditor_WarningMsg", this);
	msgbox->SetOKButtonText("#CSBTE_WpnDataEditor_Yes");
	msgbox->SetOKButtonVisible(true);
	msgbox->SetBounds(GetWide() / 2 - 150, GetTall() / 2 - 100, 300, 200);

	m_pCancelButton = new Button(this, "CancelButton", "#GameUI_Close");
	m_pCancelButton->SetContentAlignment(Label::a_center);
	m_pCancelButton->SetBounds(275, 425, 200, 50);
	m_pCancelButton->SetCommand("vguicancel");
	m_pCancelButton->SetVisible(true);

	m_pNextWpn = new Button(this, "NextWeapon", "#CSBTE_WpnDataEditor_Next");
	m_pNextWpn->SetContentAlignment(Label::a_center);
	m_pNextWpn->SetBounds(500, 425, 200, 50);
	m_pNextWpn->SetCommand("nextwpn");
	m_pNextWpn->SetVisible(true);

	m_pPrevWpn = new Button(this, "PreviousWeapon", "#CSBTE_WpnDataEditor_Previous");
	m_pPrevWpn->SetContentAlignment(Label::a_center);
	m_pPrevWpn->SetBounds(50, 425, 200, 50);
	m_pPrevWpn->SetCommand("prevwpn");
	m_pPrevWpn->SetVisible(true);

	bSaveData = new Button(this, "CancelButton", "#CSBTE_WpnDataEditor_SaveData");
	bSaveData->SetContentAlignment(Label::a_center);
	bSaveData->SetBounds(275, 225, 100, 50);
	bSaveData->SetCommand("savedata");
	bSaveData->SetVisible(true);

	//Weapons Data Label//
	m_pName = new Label(this, "WeaponName", "#CSBTE_WpnDataEditor_WpnName");
	m_plDamage = new Label(this, "WeaponDmgLbl", "#CSBTE_WpnDataEditor_Damage");
	m_plDamageZombie = new Label(this, "WeaponDmgZbLbl", "#CSBTE_WpnDataEditor_DamageZombie");

	m_pName->SetBounds(20, 30, 200, 20);
	m_plDamage->SetBounds(20, 60, 80, 20);
	m_plDamageZombie->SetBounds(300, 60, 80, 20);

	//Weapons Data TextBox//
	m_pDamage = new TextEntry(this, "WeaponDamage");
	m_pDamageZombie = new TextEntry(this, "WeaponDamageZombie");

	m_pDamage->SetBounds(100, 60, 100, 20);
	m_pDamageZombie->SetBounds(400, 60, 100, 20);

	UpdateCurrentWeapons();
}

void CCSBTEWpnDataEditor::UpdateCurrentWeapons()
{
	//Read Weapons Data//
	auto &kv = *m_iniDataIterator;
	auto &wpnName = kv.first;
	auto &wpnDmg = kv.second["Damage"];
	auto &wpnDmgZb = kv.second["DamageZombie"];

	//Set Data To Labels & TextBox//
	m_pName->SetText(GetWeaponNameFormat(wpnName));
	m_pDamage->SetText(wpnDmg.c_str());
	m_pDamageZombie->SetText(wpnDmgZb.c_str());
}

void CCSBTEWpnDataEditor::SaveData()
{
	//m_iniData
	m_iniData.SaveFile();
}

void CCSBTEWpnDataEditor::NextWpn()
{
	//Read Next Weapons//
	++m_iniDataIterator;

	if (m_iniDataIterator == m_iniData.end())
	{
		//msgbox

		//Spawn MessageBox Tells User End Of The Config, Reset To First Weapons?//
		msgbox->Activate();

		//Reset Weapons To Starting//
		m_iniDataIterator = m_iniData.begin();
	}

	UpdateCurrentWeapons();
}

void CCSBTEWpnDataEditor::PrevWpn()
{
	if (m_iniDataIterator == m_iniData.begin())
	{
		//Spawn MessageBox Tells User First Of The Config, No More Wpn Lol?//
		msgbox->Activate();

		//Prevent Bug Exploit, So Reset To First Again//
		m_iniDataIterator = m_iniData.end();
	}

	//Read Prev Weapons//
	--m_iniDataIterator;

	UpdateCurrentWeapons();
}

void CCSBTEWpnDataEditor::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{
		Close();
	}
	else if (!Q_stricmp(command, "savedata"))
	{
		SaveData();
	}
	else if (!Q_stricmp(command, "nextwpn"))
	{
		NextWpn();
	}
	else if (!Q_stricmp(command, "prevwpn"))
	{
		PrevWpn();
	}
}