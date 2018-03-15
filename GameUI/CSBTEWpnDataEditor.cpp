#include "base.h"
#include "CSBTEWpnDataEditor.h"
#include <vgui_controls\PHandle.h>
#include "common.h"
#include <io.h>
#include <string>
#include <fstream>
#include <functional>
#include <string>

#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/TextEntry.h>

extern wchar_t *GetWeaponNameFormat(const std::string &);
int totalwpn;
int wpnID;
// defines Keys' sequence and its object type
// !! ComboBox TBD
using WeaponKeyInfoType = std::pair<const std::string, objtype_t>;
static WeaponKeyInfoType WeaponKeyInfo[] = {
	{ "WeaponID", O_NUMBER },
{ "Special", O_NUMBER },
{ "Type", O_NUMBER },
{ "Menu", O_STRING },
{ "BulletType", O_STRING },
{ "Damage", O_STRING },
{ "DamageZombie", O_STRING },
{ "AttackInterval", O_STRING },
{ "Delay", O_STRING },
{ "MaxClip", O_STRING },
{ "MaxAmmo", O_STRING },
{ "AmmoCost", O_STRING },
{ "Ammo", O_STRING },
{ "Distance", O_STRING },
{ "Angle", O_STRING },
{ "MaxSpeed", O_NUMBER },
{ "ReloadTime", O_STRING },
{ "DeployTime", O_STRING },
{ "KnockBack", O_STRING },
{ "VelocityModifier", O_STRING },
{ "Zoom", O_STRING },
{ "AnimExtention", O_STRING },
{ "Cost", O_STRING },
{ "CanBuy", O_BOOL },
{ "Team", O_STRING },
{ "GameModeLimit", O_NUMBER },
{ "BurstSpeed", O_STRING },
{ "BurstTimes", O_STRING },
{ "WorldModel", O_STRING },

{ "EntitySpawnOrigin", O_STRING },
{ "EntityKnockBack", O_STRING },
{ "EntityDamage", O_STRING },
{ "EntityDamageZombie", O_STRING },
{ "EntityRange", O_STRING },
{ "EntitySpeed", O_STRING },
{ "EntityGravity", O_STRING },
{ "EntityAngle", O_STRING },

{ "AccuracyCalculate", O_NUMBER },
{ "AccuracyDefault", O_NUMBER },
{ "Accuracy", O_STRING },
{ "AccuracyRange", O_STRING },
{ "Spread", O_STRING },
{ "SpreadRun", O_STRING },
{ "AccuracyMul", O_STRING },

{ "Punchangle", O_STRING },
{ "Penetration", O_STRING },
{ "Distance", O_STRING },
{ "ArmorRatio", O_STRING },
{ "RangeModifier", O_STRING },

{ "KickBackWalking", O_STRING },
{ "KickBackNotOnGround", O_STRING },
{ "KickBackDucking", O_STRING },
{ "KickBack", O_STRING },

{ "Event", O_STRING }
};
// UnaryFunction: Compares a fixed string with pair
/*struct KeyEquals
{
using pair_type = std::remove_all_extents_t<decltype(WeaponKeyInfo)>;
const std::string &sz;
bool operator()(const pair_type &pair)
{
return pair.first == sz;
}
};*/
inline std::function<bool(const WeaponKeyInfoType &)> KeyEquals(const std::string &sz)
{
	return std::bind(IsKeyEqualsToTupleElement<0, WeaponKeyInfoType>, std::cref(sz), std::placeholders::_1);
}
// is lhs < rhs? (for sorting)
bool CCSBTEWpnDataEditor::WeaponInfoKey_Less::operator()(const std::string &lhs, const std::string &rhs) const
{
	auto pl = std::find_if(std::begin(WeaponKeyInfo), std::end(WeaponKeyInfo), KeyEquals(lhs));
	auto pr = std::find_if(std::begin(WeaponKeyInfo), std::end(WeaponKeyInfo), KeyEquals(rhs));
	return (pl != pr) ? (pl < pr) : (lhs < rhs);
}

CCSBTEWpnDataEditor::CCSBTEWpnDataEditor(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: Frame(parent, panelName, showTaskbarIcon), m_iniData("weapons.ini")
{
	int sw, sh;
	surface()->GetScreenSize(sw, sh);

	SetSize(720, 500);
	MoveToCenterOfScreen();
	SetTitle("#CSBTE_WpnDataEditor_Title", false);
	SetSizeable(false);
	SetVisible(true);

	m_iniDataIterator = m_iniData.begin();

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

	curWpnID = new Label(this, "CurrentWpnID", "");
	curWpnID->SetBounds(590, 30, 50, 20);

	totalWpnID = new Label(this, "TotalInGameWpnID", "");
	totalWpnID->SetBounds(650, 30, 50, 20);

	Divider = new Label(this, "Divider1", "|");
	Divider->SetBounds(625, 30, 50, 20);

	m_pListPanel = new CPanelListPanel(this, "PanelListPanel");
	m_pListPanel->SetBounds(50, 50, 620, 350);

	CountWpn();
	UpdateCurrentWeapons();
}

void CCSBTEWpnDataEditor::CountWpn()
{
	totalwpn = std::distance(m_iniData.begin(), m_iniData.end());
	wpnID = totalwpn - totalwpn + 1;
	gEngfuncs.Con_Printf("Readed %d Weapons.\n", totalwpn);

	std::string ID = std::to_string(totalwpn);
	totalWpnID->SetText(ID.c_str());

	std::string curID = std::to_string(wpnID);
	curWpnID->SetText(curID.c_str());
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

	//std::list<ControlPair *> CtrlList;
	// dont know why there bugs using list::sort, so build a map to fixit
	//std::map<std::string, ControlPair *, WeaponInfoKey_Less> CtrlMap;
	for (const auto &kv : wpnDataMap)
	{
		ControlPair *pCtrl = new ControlPair(m_pListPanel, "CSBTEWpnDataEditor::ControlPair");

		pCtrl->key = kv.first;

		auto p = std::find_if(std::begin(WeaponKeyInfo), std::end(WeaponKeyInfo), KeyEquals(pCtrl->key));
		pCtrl->type = p == std::end(WeaponKeyInfo) ? O_STRING : p->second;

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
		//CtrlMap.emplace(kv.first, pCtrl);
	}

	//CtrlList.sort([](ControlPair * lhs, ControlPair *rhs) {return WeaponInfoKey_Less()(lhs->key, rhs->key); });
	//std::for_each(CtrlList.begin(), CtrlList.end(), std::bind(&CPanelListPanel::AddItem, m_pListPanel, std::placeholders::_1));
	//for (auto &prprpr : CtrlMap)
	//{
	//	m_pListPanel->AddItem(prprpr.second);
	//}
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

		// ugly switch in cpp
		// dont know if dynamic_cast required
		// using virtual functions to fix that?
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
	++wpnID;

	if (m_iniDataIterator == m_iniData.end())
	{
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

	std::string curID = std::to_string(wpnID);
	curWpnID->SetText(curID.c_str());

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
	--wpnID;

	std::string curID = std::to_string(wpnID);
	curWpnID->SetText(curID.c_str());

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