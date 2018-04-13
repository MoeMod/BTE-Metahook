
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

struct CCSOLLuckyItemPopupDialog::impl_t
{
	class MyLabel;
	vgui::Button *CloseBtn;
	vgui::Label *BoxButtonLabel;
};

class CCSOLLuckyItemPopupDialog::impl_t::MyLabel : public vgui::Label
{
	using BaseClass = vgui::Label;
public:
	MyLabel(Panel *parent, const char *panelName, const char *text) : BaseClass(parent, panelName, text) 
	{
		m_bForceTextColor = false;
	}
	MyLabel(Panel *parent, const char *panelName, const wchar_t *text) : BaseClass(parent, panelName, text) 
	{
		m_bForceTextColor = false;
	}

	virtual void ApplySettings(KeyValues *inResourceData) override
	{
		BaseClass::ApplySettings(inResourceData);

		m_bForceTextColor = inResourceData->GetInt("EnableForceTextColor", 0) > 0;
		if (m_bForceTextColor)
		{
			SetForceTextColor(inResourceData->GetColor("ForceTextColor"));
		}

		SetMouseInputEnabled(inResourceData->GetInt("EnableMouseInput", 0) > 0);
	}
	virtual void Paint() override
	{
		if (m_bForceTextColor)
		{
			SetFgColor(m_forceColor);
			SetDisabledFgColor1(m_forceColor);
		}
		BaseClass::Paint();
	}

	bool m_bForceTextColor;
	Color m_forceColor;
	void SetForceTextColor(Color col)
	{
		m_bForceTextColor = true;
		m_forceColor = col;
	}
};

CCSOLLuckyItemPopupDialog::CCSOLLuckyItemPopupDialog(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: BaseClass(parent, panelName, showTaskbarIcon), pimpl(std::make_unique<impl_t>())
{
	pimpl->CloseBtn = new vgui::Button(this, "CloseBtn", "#CSO_ClosePopup", this, "vguicancel");
	pimpl->BoxButtonLabel = new impl_t::MyLabel(this, "BoxButtonLabel", "#CSO_BoxButton");

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
		return Close();
	}
	BaseClass::OnCommand(command);
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
		return new impl_t::MyLabel(this, controlName, "");
	}
	return BaseClass::CreateControlByName(controlName);
}
