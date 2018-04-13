
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

#include "CSOLLuckyItemResultDialog.h"

struct CCSOLLuckyItemPopupDialog::impl_t
{
	vgui::Button *CloseBtn;
	ForceColoredLabel *BoxButtonLabel;
};

CCSOLLuckyItemPopupDialog::CCSOLLuckyItemPopupDialog(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: BaseClass(parent, panelName, showTaskbarIcon), pimpl(std::make_unique<impl_t>())
{
	pimpl->CloseBtn = new vgui::Button(this, "CloseBtn", "#CSO_ClosePopup", this, "vguicancel");
	pimpl->BoxButtonLabel = new ForceColoredLabel(this, "BoxButtonLabel", "#CSO_BoxButton");

	LoadControlSettings("Resource/res/popup_luckyitem_renewal.res");

	this->SetMenuButtonVisible(false);
	this->SetCloseButtonVisible(false);
	this->SetSizeable(false);
	this->SetTitle("", false);
	pimpl->BoxButtonLabel->SetContentAlignment(Label::a_east);
	pimpl->BoxButtonLabel->SetEnabled(true);
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