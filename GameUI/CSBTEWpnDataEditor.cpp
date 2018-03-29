
#include "CSBTEWpnDataEditor.h"
#include <vgui_controls\PHandle.h>
#include "common.h"
#include "metahook.h"
#include "util.h"
#include "Encode.h"

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <string>

#include "tier1/KeyValues.h"
#include <vgui/IVGui.h>

#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/TextEntry.h>

using std::cout;
using std::endl;

extern wchar_t *GetWeaponNameFormat(const std::string &);

// defines Keys' sequence and its object type
// !! ComboBox TBD

class IBaseControlPanel : public vgui::Panel
{
public:
	IBaseControlPanel(vgui::Panel *parent, char const *panelName)
		: Panel(parent, panelName), m_pControl(NULL) {}

public:
	virtual void OnSizeChanged(int wide, int tall) override
	{
		int inset = 4;

		if (m_pControl)
			m_pControl->SetBounds(0, inset, wide, tall - 2 * inset);
	}
	virtual ~IBaseControlPanel() {}
	virtual std::string GetValue() = 0;

public:
	std::string key;
	vgui::Panel *m_pControl;
};

// 有左边文本和右边输入框的基类
class IBaseControlPair : public IBaseControlPanel
{
public:
	IBaseControlPair(vgui::Panel *parent, char const *panelName, char const *szDesc)
		: IBaseControlPanel(parent, panelName), m_pPrompt(NULL) 
	{
		m_pPrompt = new Label(this, "DescLabel", "");
		m_pPrompt->SetContentAlignment(Label::a_west);
		m_pPrompt->SetTextInset(5, 0);
		m_pPrompt->SetText(szDesc);
	}
	virtual void OnSizeChanged(int wide, int tall) override
	{
		int inset = 4;

		int w = wide / 2;

		if (m_pControl)
			m_pControl->SetBounds(w + 20, inset, w - 20, tall - 2 * inset);

		m_pPrompt->SetBounds(0, inset, w + 20, tall - 2 * inset);
	}
public:
	vgui::Label *m_pPrompt;
};

class CControlPairString : public IBaseControlPair
{
public:
	CControlPairString(vgui::Panel *parent, char const *panelName, char const *defaultValue, char const *desc)
		: IBaseControlPair(parent, panelName, desc)
	{
		TextEntry *pEdit = new TextEntry(this, "DescTextEntry");
		pEdit->InsertString(defaultValue);
		m_pControl = (Panel *)pEdit;
	}
	virtual std::string GetValue() override
	{
		char strValue[256];
		auto pEdit = (TextEntry *)m_pControl;
		pEdit->GetText(strValue, sizeof(strValue));
		return{ strValue };
	}
};

class CControlPairNumber : public CControlPairString
{
public:
	CControlPairNumber(vgui::Panel *parent, char const *panelName, char const *defaultValue, char const *desc)
		: CControlPairString(parent, panelName, defaultValue, desc)
	{
		// no need dynamic_cast
		TextEntry *pEdit = static_cast<TextEntry *>(m_pControl);
		pEdit->SetAllowNumericInputOnly(true);
	}
};

class CControlPairBool : public IBaseControlPanel
{
public:
	CControlPairBool(vgui::Panel *parent, char const *panelName, char const *defaultValue, char const *desc)
		: IBaseControlPanel(parent, panelName)
	{
		CheckButton *pBox = new CheckButton(this, "DescCheckButton", desc);
		pBox->SetSelected(!stricmp(defaultValue, "TRUE"));
		m_pControl = (Panel *)pBox;
	}
	virtual std::string GetValue() override
	{
		auto pBox = (CheckButton *)m_pControl;
		return pBox->IsSelected() ? "TRUE" : "FALSE";
	}
};

class CControlPairComboBox : public IBaseControlPair
{
public:
	CControlPairComboBox(vgui::Panel *parent, char const *panelName, char const *desc)
		: IBaseControlPair(parent, panelName, desc)
	{
		ComboBox *pComboBox = new ComboBox(this, "ComboBox", 6, false);
		m_pControl = (Panel *)pComboBox;
	}
	virtual std::string GetValue() override
	{
		auto pComboBox = (ComboBox *)m_pControl;
		
		KeyValues *pKeyValues = pComboBox->GetActiveItemUserData();
		return pKeyValues->GetString("value");
	}
};

using ControlPairFactoryType = std::function<IBaseControlPanel *(vgui::Panel *parent, char const *panelName, char const *defaultValue, char const *desc)>;

template<class T>
IBaseControlPanel *DefaultControlFactory(vgui::Panel *parent, char const *panelName, char const *defaultValue, char const *desc)
{
	return new T(parent, panelName, defaultValue, desc);
}

auto ControlPairNumberFactory = DefaultControlFactory<CControlPairNumber>;
auto ControlPairStringFactory = DefaultControlFactory<CControlPairString>;
auto ControlPairBoolFactory = DefaultControlFactory<CControlPairBool>;

class ControlPairComboBoxFactoryHelper
{
public:
	ControlPairComboBoxFactoryHelper(const std::initializer_list<std::string> &ValueList)
	{
		for (auto &str : ValueList)
		{
			m_List.emplace_back(str, str);
		}
	}
	IBaseControlPanel *operator()(vgui::Panel *parent, char const *panelName, char const *defaultValue, char const *desc)
	{
		auto pCtrl = new CControlPairComboBox(parent, panelName, desc);
		auto pComboBox = static_cast<ComboBox *>(pCtrl->m_pControl);
		// defaultValue
		for (auto &pair : m_List)
		{
			auto pkv = new KeyValues("ControlPairValue", "value", pair.second.c_str());
			pComboBox->AddItem(pair.first.c_str(), pkv);
		}
		auto p = std::find_if(m_List.begin(), m_List.end(), std::bind(IsKeyEqualsToTupleElement<1, std::pair<std::string, std::string>>, std::cref(defaultValue), std::placeholders::_1));
		pComboBox->ActivateItem(p - m_List.begin());
		return pCtrl;
	}
	std::vector<std::pair<std::string, std::string>> m_List; // { display, real_key}
};


using WeaponKeyInfoType = std::pair<const std::string, ControlPairFactoryType>;
static WeaponKeyInfoType WeaponKeyInfo[] = {
{ "WeaponID", ControlPairNumberFactory },
{ "Special", ControlPairNumberFactory },
{ "Type", ControlPairNumberFactory },
{ "Menu", ControlPairComboBoxFactoryHelper{ "PISTOL", "SHOTGUN", "SMG", "RIFLE", "MG", "EQUIP", "KNIFE" } },
{ "BulletType", ControlPairStringFactory },
{ "Damage", ControlPairStringFactory },
{ "DamageZombie", ControlPairStringFactory },
{ "AttackInterval", ControlPairStringFactory },
{ "Delay", ControlPairStringFactory },
{ "MaxClip", ControlPairStringFactory },
{ "MaxAmmo", ControlPairStringFactory },
{ "AmmoCost", ControlPairStringFactory },
{ "Ammo", ControlPairStringFactory },
{ "Distance", ControlPairStringFactory },
{ "Angle", ControlPairStringFactory },
{ "MaxSpeed", ControlPairNumberFactory },
{ "ReloadTime", ControlPairStringFactory },
{ "DeployTime", ControlPairStringFactory },
{ "KnockBack", ControlPairStringFactory },
{ "VelocityModifier", ControlPairStringFactory },
{ "Zoom", ControlPairStringFactory },
{ "AnimExtention", ControlPairStringFactory },
{ "Cost", ControlPairStringFactory },
{ "CanBuy", ControlPairBoolFactory },
{ "Team", ControlPairComboBoxFactoryHelper{ "ALL", "CT", "TR" } },
{ "GameModeLimit", ControlPairNumberFactory },
{ "BurstSpeed", ControlPairStringFactory },
{ "BurstTimes", ControlPairStringFactory },
{ "WorldModel", ControlPairStringFactory },

{ "EntitySpawnOrigin", ControlPairStringFactory },
{ "EntityKnockBack", ControlPairStringFactory },
{ "EntityDamage", ControlPairStringFactory },
{ "EntityDamageZombie", ControlPairStringFactory },
{ "EntityRange", ControlPairStringFactory },
{ "EntitySpeed", ControlPairStringFactory },
{ "EntityGravity", ControlPairStringFactory },
{ "EntityAngle", ControlPairStringFactory },

{ "AccuracyCalculate", ControlPairNumberFactory },
{ "AccuracyDefault", ControlPairNumberFactory },
{ "Accuracy", ControlPairStringFactory },
{ "AccuracyRange", ControlPairStringFactory },
{ "Spread", ControlPairStringFactory },
{ "SpreadRun", ControlPairStringFactory },
{ "AccuracyMul", ControlPairStringFactory },

{ "Punchangle", ControlPairStringFactory },
{ "Penetration", ControlPairStringFactory },
{ "Distance", ControlPairStringFactory },
{ "ArmorRatio", ControlPairStringFactory },
{ "RangeModifier", ControlPairStringFactory },

{ "KickBackWalking", ControlPairStringFactory },
{ "KickBackNotOnGround", ControlPairStringFactory },
{ "KickBackDucking", ControlPairStringFactory },
{ "KickBack", ControlPairStringFactory },

{ "Event", ControlPairStringFactory }
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
	for (auto app : m_iniData)
	{
		auto &&name = app.first.c_str();
		m_wpnNames.emplace_back(name, GetWeaponNameFormat(name));
	}

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

	//Search Weapons//
	m_pWeaponSearch = new CCSBTEWpnDataEditorSearchBar(this, "Search Weapons");
	m_pWeaponSearch->SetBounds(480, 30, 100, 20);
	m_pWeaponSearch->SetAllowNonAsciiCharacters(true);

	/*m_pSearchWpn = new Button(this, "Search Weapons", "Search");
	m_pSearchWpn->SetContentAlignment(Label::a_center);
	m_pSearchWpn->SetBounds(420, 30, 100, 20);
	m_pSearchWpn->SetCommand("searchwpnname");
	m_pSearchWpn->SetVisible(true);*/

	int totalwpn = m_iniData.size();
	//gEngfuncs.Con_Printf("Readed %d Weapons.\n", totalwpn);
	std::cout << "Readed " << totalwpn << " Weapons." << std::endl;

	UpdateCurrentWeapons();
}

void CCSBTEWpnDataEditor::CountWpn()
{
	int totalwpn = m_iniData.size();
	int wpnID = std::distance(m_iniData.begin(), m_iniDataIterator) + 1;

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
	CountWpn();
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
		auto p = std::find_if(std::begin(WeaponKeyInfo), std::end(WeaponKeyInfo), KeyEquals(kv.first));
		

		ControlPairFactoryType ControlPairFactory = p == std::end(WeaponKeyInfo) ? ControlPairStringFactory : p->second;

		std::string desc = "#CSBTE_WpnDataEditor_";
		desc += kv.first;

		IBaseControlPanel *pCtrl = ControlPairFactory(m_pListPanel, "CSBTEWpnDataEditor::ControlPair", kv.second.c_str(), desc.c_str());

		pCtrl->key = kv.first;

		pCtrl->SetSize(100, 28);
		m_pListPanel->AddItem(pCtrl);
		//CtrlMap.emplace(kv.first, pCtrl);
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
		auto pList = static_cast<IBaseControlPanel *>(m_pListPanel->GetItem(i));
		const std::string &key = pList->key;
		const std::string &&value = pList->GetValue();

		char szValue[256];
		char strValue[256];

		// ugly switch in cpp
		// dont know if dynamic_cast required
		// using virtual functions to fix that?
		
		wpnDataMap[key] = value;
	}
	m_iniData.SaveFile();
}

void CCSBTEWpnDataEditor::NextWpn()
{
	//Read Next Weapons//
	++m_iniDataIterator;

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

void CCSBTEWpnDataEditor::OnTextChanged(void)
{
	const size_t buffersize = 64;
	static wchar_t wszBuffer[buffersize];
	m_pWeaponSearch->GetText(wszBuffer, buffersize);
	std::string input = ToUpper(UnicodeToANSI(wszBuffer));

	m_pWeaponSearch->GetMenu()->ClearCurrentlyHighlightedItem();
	m_pWeaponSearch->RemoveAll();

	// to be optimized
	auto ends = m_wpnNames.end();
	auto fnCanFind = [&input](const std::pair<std::string, std::wstring> &to_find){
		return (strstr(to_find.first.c_str(), input.c_str()) != NULL) || (wcsstr(to_find.second.c_str(), wszBuffer) != NULL);
	};
	for (auto iter = m_wpnNames.begin(); (iter = std::find_if(iter, ends, fnCanFind)) != ends; ++iter)
	{
		const char *&&szRealWpnName = iter->first.c_str();
		const wchar_t *&&szWeaponDisplayName = iter->second.c_str();
		m_pWeaponSearch->AddItem(szWeaponDisplayName, new KeyValues(szRealWpnName));
	}
	
	m_pWeaponSearch->GetMenu()->Panel::SetVisible(true);
	m_pWeaponSearch->GetMenu()->MoveToFront();
	m_pWeaponSearch->GetMenu()->PerformLayout();
	m_pWeaponSearch->MoveToFront();
}

void CCSBTEWpnDataEditorSearchBar::OnMenuItemSelected()
{
	KeyValues *msg = new KeyValues("MenuItemSelected");
	msg->SetString("weapon", GetActiveItemUserData()->GetName());
	PostActionSignal(msg);

	BaseClass::OnMenuItemSelected();
	HideMenu();
}

void CCSBTEWpnDataEditor::OnMenuItemSelected(const char * weapon)
{
	m_iniDataIterator = m_iniData.GetMap().find(weapon);
	Assert(m_iniDataIterator != m_iniData.end());
	UpdateCurrentWeapons();
}