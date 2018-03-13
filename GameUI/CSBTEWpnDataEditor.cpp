#include "base.h"
#include "CSBTEWpnDataEditor.h"
#include <vgui_controls\PHandle.h>
#include "common.h"
#include <io.h>
#include <string>
#include <fstream>

#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/TextEntry.h>

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

	m_pSaveButton = new Button(this, "SaveData", "#CSBTE_WpnDataEditor_SaveData");
	m_pSaveButton->SetContentAlignment(Label::a_center);
	m_pSaveButton->SetBounds(275, 425, 200, 50);
	m_pSaveButton->SetCommand("savedata");
	m_pSaveButton->SetVisible(true);

	//Weapons Data Label//
	m_pName = new Label(this, "WeaponName", "#CSBTE_WpnDataEditor_WpnName");
	m_pName->SetBounds(20, 30, 200, 20);

	/*m_plDamage = new Label(this, "WeaponDmgLbl", "#CSBTE_WpnDataEditor_Damage");
	m_plDamageZombie = new Label(this, "WeaponDmgZbLbl", "#CSBTE_WpnDataEditor_DamageZombie");
	m_plDamage->SetBounds(20, 60, 80, 20);
	m_plDamageZombie->SetBounds(300, 60, 80, 20);

	//Weapons Data TextBox//
	m_pDamage = new TextEntry(this, "WeaponDamage");
	m_pDamageZombie = new TextEntry(this, "WeaponDamageZombie");

	m_pDamage->SetBounds(100, 60, 100, 20);
	m_pDamageZombie->SetBounds(400, 60, 100, 20);
	*/
	m_pListPanel = new CPanelListPanel(this, "PanelListPanel");
	m_pListPanel->SetBounds(50, 50, 620, 350);

	UpdateCurrentWeapons();
}

void CCSBTEWpnDataEditor::UpdateCurrentWeapons()
{
	//Read Weapons Data//
	auto &wpnName = m_iniDataIterator->first;
	//auto &wpnDmg = m_iniDataIterator->second["Damage"];
	//auto &wpnDmgZb = m_iniDataIterator->second["DamageZombie"];

	//Set Data To Labels & TextBox//
	m_pName->SetText(GetWeaponNameFormat(wpnName));
	//m_pDamage->SetText(wpnDmg.c_str());
	//m_pDamageZombie->SetText(wpnDmgZb.c_str());

	CreateControls();
}

void CCSBTEWpnDataEditor::CreateControls()
{
	DestroyControls();

	auto &wpnName = m_iniDataIterator->first;
	auto &wpnDataMap = m_iniDataIterator->second;

	ControlPair *pCtrl;

	Panel *objParent = m_pListPanel;

	for (auto &kv : wpnDataMap)
	{
		pCtrl = new ControlPair(objParent, "CSBTEWpnDataEditor::ControlPair");
		pCtrl->type = O_STRING;
		pCtrl->key = kv.first;

		std::string desc = "#CSBTE_WpnDataEditor_";
		desc += kv.first;

		switch (pCtrl->type)
		{
		case O_BOOL:
		{
			CheckButton *pBox = new CheckButton(pCtrl, "DescCheckButton", desc.c_str());
			pBox->SetSelected(kv.second == "TRUE");
			pCtrl->pControl = (Panel *)pBox;
			break;
		}

		case O_STRING:
		case O_NUMBER:
		{
			TextEntry *pEdit = new TextEntry(pCtrl, "DescTextEntry");
			pEdit->InsertString(kv.second.c_str());
			pCtrl->pControl = (Panel *)pEdit;
			break;
		}

		/*case O_LIST:
		{
			ComboBox *pCombo = new ComboBox(pCtrl, "DescComboBox", 5, false);
			pListItem = pObj->pListItems;

			while (pListItem)
			{
				pCombo->AddItem(pListItem->szItemText, NULL);
				pListItem = pListItem->pNext;
			}

			pCombo->ActivateItemByRow((int)pObj->fdefValue);
			pCtrl->pControl = (Panel *)pCombo;
			break;
		}*/

		default: break;
		}

		if (pCtrl->type != O_BOOL)
		{
			pCtrl->pPrompt = new Label(pCtrl, "DescLabel", "");
			pCtrl->pPrompt->SetContentAlignment(Label::a_west);
			pCtrl->pPrompt->SetTextInset(5, 0);
			pCtrl->pPrompt->SetText(desc.c_str());
		}

		pCtrl->SetSize(100, 28);
		m_pListPanel->AddItem(pCtrl);

	}
}

void CCSBTEWpnDataEditor::DestroyControls()
{
	m_pListPanel->DeleteAllItems();
}

void CCSBTEWpnDataEditor::SaveData()
{
	auto &wpnDataMap = m_iniDataIterator->second;

	for (int i = 0; i < m_pListPanel->GetItemCount(); ++i)
	{
		auto pList = static_cast<ControlPair *>(m_pListPanel->GetItem(i));
		const std::string &key = pList->key;

		char szValue[256];
		char strValue[256];

		switch (pList->type)
		{
		case O_BOOL:
		{
			auto pBox = (CheckButton *)pList->pControl;
			sprintf(szValue, "%s", pBox->IsSelected() ? "TRUE" : "FALSE");
			break;
		}
		case O_NUMBER:
		{
			auto pEdit = (TextEntry *)pList->pControl;
			pEdit->GetText(strValue, sizeof(strValue));
			sprintf(szValue, "%s", strValue);
			break;
		}
		case O_STRING:
		{
			auto pEdit = (TextEntry *)pList->pControl;
			pEdit->GetText(strValue, sizeof(strValue));
			sprintf(szValue, "%s", strValue);
			break;
		}
		/*case O_LIST:
		{
			pCombo = (ComboBox *)pList->pControl;
			int activeItem = pCombo->GetActiveItem();
			pItem = pObj->pListItems;
			int n = (int)pObj->fdefValue;

			while (pItem)
			{
				if (!activeItem--)
					break;

				pItem = pItem->pNext;
			}

			if (pItem)
			{
				sprintf(szValue, "%s", pItem->szValue);
			}
			else
			{
				assert(!("Couldn't find string in list, using default value"));
				sprintf(szValue, "%s", pObj->defValue);
			}

			break;
		}*/
		} // switch ends
		wpnDataMap[key] = szValue;
	}
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
		auto msgbox = new CCSBTEWpnDataEditorMessageBox("#CSBTE_WpnDataEditor_Warning", "#CSBTE_WpnDataEditor_WarningMsg", this);
		msgbox->SetOKButtonText("#CSBTE_WpnDataEditor_Yes");
		msgbox->SetOKButtonVisible(true);
		msgbox->SetBounds(GetWide() / 2 - 150, GetTall() / 2 - 100, 300, 200);
		msgbox->DoModal();
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
		auto msgbox = new CCSBTEWpnDataEditorMessageBox("#CSBTE_WpnDataEditor_Warning", "#CSBTE_WpnDataEditor_WarningMsg", this);
		msgbox->SetOKButtonText("#CSBTE_WpnDataEditor_Yes");
		msgbox->SetOKButtonVisible(true);
		msgbox->SetBounds(GetWide() / 2 - 150, GetTall() / 2 - 100, 300, 200);
		msgbox->DoModal();
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
	else
		BaseClass::OnCommand(command);
}

void CCSBTEWpnDataEditor::OnClose()
{
	MarkForDeletion();
	BaseClass::OnClose();
}