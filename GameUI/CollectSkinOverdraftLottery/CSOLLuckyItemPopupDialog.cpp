
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

#include "cso_controls/TexturedButton.h"
#include "cso_controls/ForceColoredLabel.h"
#include "cso_controls/DarkTextEntry.h"
#include "cso_controls/SelectedImage.h"

#include "CSOLLuckyItemResultDialog.h"

#include "iniparser.h"
#include "vgui_controls/PanelListPanel.h"

struct CCSOLLuckyItemPopupDialog::impl_t
{
	vgui::Button *CloseBtn;
	ForceColoredLabel *BoxButtonLabel;

	DarkTextEntry *DismantlerListBG;
	PanelListPanel *m_pDecoderList;
	CIniParser m_iniData;
	std::vector<std::string> m_Decoders;
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
		//m_pSelectedBackground->SetImage(scheme()->GetImage("resource/clanpoint_bg", false));
		m_pSelectedBackground->SetBounds(0, 0, 225, 81);
		//m_pSelectedBackground->SetVisible(false);
		//m_pSelectedBackground->SetShouldScaleImage(true);
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
		return Panel::PerformLayout();
	}

private:
	Label *m_pText;
	Label *m_pText2;
	ImagePanel *m_pWeaponImage;
	ImagePanel *m_pItemBackground;
	SelectedImage *m_pSelectedBackground;
};

CCSOLLuckyItemPopupDialog::CCSOLLuckyItemPopupDialog(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: BaseClass(parent, panelName, showTaskbarIcon), pimpl(std::make_unique<impl_t>())
{
	pimpl->m_iniData.OpenFile("decoder.ini");

	pimpl->DismantlerListBG = new DarkTextEntry(this, "DismantlerListBG");
	pimpl->m_pDecoderList = new PanelListPanel(this, "DismantlerContainerClipPanelName");


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
			AddDecoder(szDecoder.c_str());
		}
	}

	pimpl->m_pDecoderList->SetWide(260);
}

void CCSOLLuckyItemPopupDialog::AddDecoder(const char *name)
{
	if (std::find(pimpl->m_Decoders.begin(), pimpl->m_Decoders.end(), name) != pimpl->m_Decoders.end())
	{
		return;
	}
	int i = pimpl->m_Decoders.size();
	pimpl->m_Decoders.emplace_back(name);
	auto pPanel = new DecoderItemPanel(pimpl->m_pDecoderList, name, name);
	pimpl->m_pDecoderList->AddItem(nullptr, pPanel);
	pimpl->m_pDecoderList->SetFirstColumnWidth(0);
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
	BaseClass::OnCommand(command);
}

void CCSOLLuckyItemPopupDialog::OnOpenDecoder()
{
	auto *pResultDialog = new CCSOLLuckyItemResultDialog(this);
	pResultDialog->Activate();
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