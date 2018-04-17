
#include "EngineInterface.h"
#include <vgui_controls\PHandle.h>

#include "CSOLLuckyItemPopupDialog.h"

#include <vgui_controls/Button.h>
#include <vgui_controls/MenuItem.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/URLLabel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/TextImage.h>

#include "cso_controls/TexturedButton.h"
#include "cso_controls/ForceColoredLabel.h"
#include "cso_controls/DarkTextEntry.h"
#include "cso_controls/SelectedImage.h"
#include "cso_controls/CustomFontLabel.h"

#include "CSOLLuckyItemResultDialog.h"

#include "iniparser.h"
#include "LinearMap.h"
#include "vgui_controls/PanelListPanel.h"

struct CCSOLLuckyItemPopupDialog::impl_t
{
	vgui::Button *CloseBtn;
	ForceColoredLabel *BoxButtonLabel;

	DarkTextEntry *DismantlerListBG;
	PanelListPanel *m_pDecoderList;
	DarkTextEntry *ItemListBG;
	PanelListPanel *m_pItemList;
	CBasicIniParser< LinearMap<std::string, std::unordered_map<std::string, std::string>> > m_iniData;

	std::vector<std::string> m_DecoderNames;
	std::vector<int> m_DecoderMaxRatio;

	int m_iSelectedDecoderId;
};

class DecoderItemPanel : public Button
{
	DECLARE_CLASS_SIMPLE(DecoderItemPanel, Button);
public:
	DecoderItemPanel(Panel *parent, const char *name, const char *decoderName)
		:Button(parent, name, "")
	{
		m_pText = new Label(this, "Label", MakeString("#CSO_Item_Name_", decoderName).c_str());
		m_pText->SetPaintBackgroundEnabled(false);
		m_pText->SetBounds(5, 80, 200, 20);
		m_pText->SetMouseInputEnabled(false);

		wchar_t buffer[64];
		swprintf(buffer, 64, g_pVGuiLocalize->Find("#CSO_Item_Remain_Amount_Format"), 10000);

		m_pText2 = new Label(this, "Label", buffer);
		m_pText2->SetPaintBackgroundEnabled(false);
		m_pText2->SetBounds(5, 95, 200, 20);
		m_pText2->SetMouseInputEnabled(false);

		m_pSelectedBackground = new SelectedImage(this, "CSBTESelectedBackground");
		m_pSelectedBackground->SetBounds(0, 0, 225, 81);
		m_pSelectedBackground->SetMouseInputEnabled(false);

		m_pItemBackground = new ImagePanel(this, "CSBTEItemBackground");
		m_pItemBackground->SetImage(scheme()->GetImage("gfx/ui/panel/basket_blank_slot", false));
		m_pItemBackground->SetBounds(0, 0, 225, 81);
		m_pItemBackground->SetShouldScaleImage(true);
		m_pItemBackground->SetMouseInputEnabled(false);

		m_pWeaponImage = new ImagePanel(this, "CSBTEWeaponImage");
		m_pWeaponImage->SetImage(scheme()->GetImage(MakeString("gfx/vgui/basket/", decoderName).c_str(), false));
		m_pWeaponImage->SetBounds(0, 0, 225, 81);
		m_pWeaponImage->SetShouldScaleImage(true);
		m_pWeaponImage->SetMouseInputEnabled(false);

		SetSize(225, 115);
		SetPaintBackgroundEnabled(false);
	}

	virtual void PerformLayout() override
	{
		m_pSelectedBackground->SetVisible(!IsSelected() && IsDepressed());
		return Panel::PerformLayout();
	}

	MESSAGE_FUNC_INT(OnPanelSelected, "PanelSelected", state)
	{
		ForceDepressed(state);
	}

private:
	Label *m_pText;
	Label *m_pText2;
	ImagePanel *m_pWeaponImage;
	ImagePanel *m_pItemBackground;
	SelectedImage *m_pSelectedBackground;
};

class ItemDescPanel : public Panel
{
	DECLARE_CLASS_SIMPLE(ItemDescPanel, Panel);
public:
	ItemDescPanel(Panel *parent, const char *name, const char *ItemName)
		:Panel(parent, name)
	{
		m_pText = new Label(this, "Label", MakeString("#CSO_Item_Name_", ItemName).c_str());
		m_pText->SetPaintBackgroundEnabled(false);
		m_pText->SetBounds(0, 56, 200, 20);
		m_pText->SetMouseInputEnabled(false);

		m_pItemBackground = new ImagePanel(this, "CSBTEItemBackground");
		m_pItemBackground->SetImage(scheme()->GetImage("gfx/ui/panel/basket_blank_slot", false));
		m_pItemBackground->SetBounds(0, 0, 158, 56);
		m_pItemBackground->SetShouldScaleImage(true);
		m_pItemBackground->SetMouseInputEnabled(false);

		m_pWeaponImage = new ImagePanel(this, "CSBTEWeaponImage");

		IImage *pImage = scheme()->GetImage(MakeString("gfx/vgui/basket/", ItemName).c_str(), true);
		if(!pImage)
			pImage = scheme()->GetImage(MakeString("gfx/vgui/", ItemName).c_str(), true);
		m_pWeaponImage->SetImage(pImage);
		m_pWeaponImage->SetBounds(0, 0, 158, 56);
		m_pWeaponImage->SetShouldScaleImage(true);
		m_pWeaponImage->SetMouseInputEnabled(false);

		SetSize(158, 80);
		SetPaintBackgroundEnabled(false);
	}

	void SetItemName(const char *name)
	{
		m_pText->SetText(name);
	}

	void SetItemImage(const char *image)
	{
		m_pWeaponImage->SetImage(image);
	}

private:
	Label * m_pText;
	ImagePanel *m_pWeaponImage;
	ImagePanel *m_pItemBackground;
};

CCSOLLuckyItemPopupDialog::CCSOLLuckyItemPopupDialog(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: BaseClass(parent, panelName, showTaskbarIcon), pimpl(std::make_unique<impl_t>())
{
	pimpl->m_iniData.OpenFile("decoder.ini");

	pimpl->DismantlerListBG = new DarkTextEntry(this, "DismantlerListBG");
	pimpl->m_pDecoderList = new PanelListPanel(this, "DismantlerContainerClipPanelName");

	pimpl->ItemListBG = new DarkTextEntry(this, "ItemListBG");
	pimpl->m_pItemList = new PanelListPanel(this, "ItemContainerClipPanelName");


	pimpl->CloseBtn = new vgui::Button(this, "CloseBtn", "#CSO_ClosePopup", this, "vguicancel");
	pimpl->BoxButtonLabel = new ForceColoredLabel(this, "BoxButtonLabel", "#CSO_BoxButton");

	LoadControlSettings("Resource/res/popup_luckyitem_renewal.res");

	this->SetMenuButtonVisible(false);
	this->SetCloseButtonVisible(false);
	this->SetSizeable(false);
	this->SetTitle("", false);
	pimpl->BoxButtonLabel->SetContentAlignment(Label::a_east);
	pimpl->BoxButtonLabel->SetEnabled(true);

	for (auto &kvp : pimpl->m_iniData)
	{
		auto szDecoders = kvp.second["Decoder"];
		for (auto szDecoder : ParseString(szDecoders))
		{
			
			int i = AddDecoder(szDecoder.c_str());
			pimpl->m_DecoderMaxRatio[i] += std::stoi(kvp.second["Probability"]);
		}
	}
	if (pimpl->m_pDecoderList->GetItemCount())
		SelectDecoder(0);

	pimpl->m_pDecoderList->SetWide(pimpl->m_pDecoderList->GetWide() + 15);
	pimpl->m_pItemList->SetWide(pimpl->m_pItemList->GetWide() + 15);
	pimpl->m_pDecoderList->SetFirstColumnWidth(0);
	pimpl->m_pItemList->SetFirstColumnWidth(0);
}

int CCSOLLuckyItemPopupDialog::AddDecoder(const char *name)
{
	auto iter = std::find(pimpl->m_DecoderNames.begin(), pimpl->m_DecoderNames.end(), name);
	if (iter != pimpl->m_DecoderNames.end())
	{
		return std::distance(pimpl->m_DecoderNames.begin(), iter);
	}
	int i = pimpl->m_DecoderNames.size();
	pimpl->m_DecoderNames.emplace_back(name);
	pimpl->m_DecoderMaxRatio.push_back(0);
	auto pPanel = new DecoderItemPanel(pimpl->m_pDecoderList, name, name);
	pPanel->SetCommand(MakeString("SelectDecoder ", i).c_str());
	pPanel->AddActionSignalTarget(this);
	pimpl->m_pDecoderList->AddItem(nullptr, pPanel);
	return i;
}

void CCSOLLuckyItemPopupDialog::SelectDecoder(int i)
{
	pimpl->m_iSelectedDecoderId = i;
	pimpl->m_pDecoderList->SetSelectedPanel(pimpl->m_pDecoderList->GetItemPanel(i));

	pimpl->m_pItemList->DeleteAllItems();
	auto &DecoderName = pimpl->m_DecoderNames[i];

	std::string AliasDecoderName = DecoderName;
	if (!stricmp(DecoderName.c_str(), "CashDecoder"))
		AliasDecoderName = "DecoderA";
	else if (!stricmp(DecoderName.c_str(), "EventCashDecoder"))
		AliasDecoderName = "EventDecoderA";

	Label *pName = new CustomFontLabel(pimpl->m_pItemList, "Label", MakeString("#CSO_ItemBox_Name_", AliasDecoderName).c_str(), "DefaultTitle");
	pName->SizeToContents();
	pName->SetTall(pName->GetTall() + 10);
	pName->SetContentAlignment(Label::a_northwest);

	Label *pDesc = new Label(pimpl->m_pItemList, "Label", MakeString("#CSO_ItemBox_Desc_", AliasDecoderName).c_str());
	pDesc->SizeToContents();
	pDesc->SetTall(pDesc->GetTall() + 10);

	pimpl->m_pItemList->AddItem(NULL, pName);
	pimpl->m_pItemList->AddItem(NULL, pDesc);

	vgui::Panel *backupPanel = NULL;
	for (auto &kvp : pimpl->m_iniData)
	{
		auto &kv = kvp.second;
		std::vector<std::string> MyDecoders = ParseString(kv["Decoder"]);
		if (std::find(MyDecoders.begin(), MyDecoders.end(), DecoderName) == MyDecoders.end())
			continue;

		decltype(kv.find(std::declval<const char *>())) iter;
		iter = kv.find("Display");
		if (iter == kv.end() || !std::stoi(iter->second))
			continue;

		ItemDescPanel *pItemDescPanel = new ItemDescPanel(pimpl->m_pItemList, "ItemDescPanel", kvp.first.c_str());
		iter = kv.find("Image");
		if (iter != kv.end())
			pItemDescPanel->SetItemImage(iter->second.c_str());
		iter = kv.find("Name");
		if (iter != kv.end())
			pItemDescPanel->SetItemName(iter->second.c_str());
		
		

		if (backupPanel)
		{
			vgui::Panel *pPairPanel = new Panel(pimpl->m_pItemList);
			backupPanel->SetParent(pPairPanel);
			pItemDescPanel->SetParent(pPairPanel);
			pPairPanel->SetSize(backupPanel->GetWide() + pItemDescPanel->GetWide(), max(backupPanel->GetTall(), pItemDescPanel->GetTall()));
			pItemDescPanel->SetPos(backupPanel->GetWide() + 5, 0);
			pimpl->m_pItemList->AddItem(NULL, pPairPanel);
			backupPanel = nullptr;
		}
		else
		{
			backupPanel = pItemDescPanel;
		}
	}
	if (backupPanel)
	{
		pimpl->m_pItemList->AddItem(NULL, backupPanel);
		backupPanel = nullptr;
	}
}

void CCSOLLuckyItemPopupDialog::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{
		return Close();
	}
	else if (!Q_stricmp(command, "BoxButton"))
	{
		OnOpenDecoder();
		return;
	}
	else if (!strncmp(command, "SelectDecoder ", 14))
	{
		int i = atoi(command + 14);
		SelectDecoder(i);
		return;
	}
	BaseClass::OnCommand(command);
}

void CCSOLLuckyItemPopupDialog::OnOpenDecoder()
{
	int i = pimpl->m_iSelectedDecoderId;
	int iMaxRatio = pimpl->m_DecoderMaxRatio[i];
	if (!iMaxRatio)
		return;
	int iRandom = engine->pfnRandomLong(0, iMaxRatio - 1);
	int iCur = 0;
	auto iterSelectedItem = pimpl->m_iniData.end();
	for(auto iter = pimpl->m_iniData.begin(); iter != pimpl->m_iniData.end(); ++iter)
	{
		auto &kv = iter->second;

		std::vector<std::string> &&decoders = ParseString(kv["Decoder"]);
		if (std::find(decoders.begin(), decoders.end(), pimpl->m_DecoderNames[i]) == decoders.end())
			continue;

		iCur += std::stoi(kv["Probability"]);
		if (iCur >= iRandom)
		{
			iterSelectedItem = iter;
			break;
		}
	}

	Assert(iterSelectedItem != pimpl->m_iniData.end());
	auto *pResultDialog = new CCSOLLuckyItemResultDialog(this);

	auto &ItemName = iterSelectedItem->first;
	auto &kv = iterSelectedItem->second;
	auto iter = kv.end();
	IImage *pImage = NULL;
	if ((iter = kv.find("Image")) != kv.end())
	{
		pImage = vgui::scheme()->GetImage(iter->second.c_str(), true);
	}
	else
	{
		IImage *pImage = scheme()->GetImage(MakeString("gfx/vgui/basket/", ItemName).c_str(), true);
		if (!pImage)
			pImage = scheme()->GetImage(MakeString("gfx/vgui/", ItemName).c_str(), true);
	}
		
	auto DisplayItemName = MakeString("#CSO_Item_Name_", ItemName);
	if ((iter = kv.find("Name")) != kv.end())
		DisplayItemName = iter->second;

	auto DisplayItemDesc = MakeString("#CSO_Item_Desc_", ItemName);
	if ((iter = kv.find("Desc")) != kv.end())
		DisplayItemDesc = iter->second;

	pResultDialog->SetAssociatedItem(DisplayItemName.c_str(), DisplayItemDesc.c_str(), pImage);

	if ((iter = kv.find("Limit")) != kv.end())
	{
		wchar_t *format = vgui::localize()->Find(MakeString("#CSO_Gachapon_", iter->second, "_Text_Format").c_str());
		static wchar_t NameBuffer[128];
		if((iter = kv.find("Amount")) != kv.end())
			swprintf(NameBuffer, format, std::stoi(iter->second));
		else
			swprintf(NameBuffer, format);

		wchar_t *localizedName = vgui::localize()->Find(DisplayItemName.c_str());
		if (localizedName)
			pResultDialog->SetItemName(MakeStringW(localizedName, L" ", NameBuffer).c_str());
	}

	pResultDialog->Activate();

	if((iter = kv.find("Type")) != kv.end())
		pResultDialog->ActivateAnimation(std::stoi(iter->second));
}

vgui::Panel *CCSOLLuckyItemPopupDialog::CreateControlByName(const char *controlName)
{
	if (!Q_stricmp(controlName, "CTempFrame"))
	{
		return this;
	}
	else if (!Q_stricmp(controlName, "BackgroundMenuButton"))
	{
		return new TexturedButton(this, controlName);
	}
	else if (!Q_stricmp(controlName, "Label"))
	{
		return new ForceColoredLabel(this, controlName, "");
	}
	return BaseClass::CreateControlByName(controlName);
}