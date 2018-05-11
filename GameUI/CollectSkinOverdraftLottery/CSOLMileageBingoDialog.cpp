
#include "EngineInterface.h"
#include <vgui_controls\PHandle.h>

#include "CSOLMileageBingoDialog.h"

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
#include "vgui_controls/AnimationController.h"

#include "GameUI/BasePanel.h"
#include "GameUI/BinkPanel.h"

#include "STLHelper.h"

struct CCSOLMileageBingoDialog::impl_t
{
	vgui::TextEntry *ItemName;
	vgui::TextEntry *ItemDescription;
	vgui::ImagePanel *Item;

	vgui::Button *MyInfoBtn;
	vgui::Button *OKBtn;
	vgui::ImagePanel *Effect;
	CBinkPanel *bink;
};

CCSOLMileageBingoDialog::CCSOLMileageBingoDialog(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: BaseClass(parent, panelName, showTaskbarIcon), pimpl(std::make_unique<impl_t>())
{
	pimpl->MyInfoBtn = new vgui::Button(this, "MyInfoBtn", "#CSO_GotoMyInfo", this, "vguicancel");
	pimpl->OKBtn = new vgui::Button(this, "OKBtn", "#CSO_OKl_Btn", this, "vguicancel");
	pimpl->Effect = new vgui::ImagePanel(this, "Effect");

	//pimpl->ItemName = new DarkTextEntry(this, "ItemName");
	//pimpl->ItemDescription = new DarkTextEntry(this, "ItemDescription");
	//pimpl->Item = new vgui::ImagePanel(this, "Item");

	LoadControlSettings("Resource/res/popup_mileagebingodlg.res");

	pimpl->Item = dynamic_cast<vgui::ImagePanel *>(FindChildByName("Item"));
	pimpl->ItemName = dynamic_cast<vgui::TextEntry *>(FindChildByName("ItemName"));
	pimpl->ItemDescription = dynamic_cast<vgui::TextEntry *>(FindChildByName("ItemDescription"));

	this->SetMenuButtonVisible(false);
	this->SetCloseButtonVisible(false);
	this->SetSizeable(false);
	this->SetTitle("", false);

	pimpl->bink = new CBinkPanel(this, "BinkPanel");
	pimpl->bink->SetVisible(false);
	pimpl->bink->SetZPos(233);
}

void CCSOLMileageBingoDialog::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{
		return Close();
	}
	BaseClass::OnCommand(command);
}

vgui::Panel *CCSOLMileageBingoDialog::CreateControlByName(const char *controlName)
{
	if (!Q_stricmp(controlName, "MileageBingoDlg"))
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
	else if (!Q_stricmp(controlName, "RoundPanel") || !Q_stricmp(controlName, "IMETextEntry") || !Q_stricmp(controlName, "TextEntry"))
	{
		return new DarkTextEntry(this, "DarkTextEntry");
	}
	return BaseClass::CreateControlByName(controlName);
}

void CCSOLMileageBingoDialog::Activate(void)
{
	BaseClass::Activate();
	BasePanel()->PositionDialog(this);
}

void CCSOLMileageBingoDialog::OnClose(void)
{
	BaseClass::OnClose();
}