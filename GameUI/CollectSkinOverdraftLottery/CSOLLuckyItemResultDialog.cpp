
#include "EngineInterface.h"
#include <vgui_controls\PHandle.h>

#include "CSOLLuckyItemResultDialog.h"

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

struct CCSOLLuckyItemResultDialog::impl_t
{
	vgui::TextEntry *ItemName;
	vgui::TextEntry *ItemDescription;
	vgui::ImagePanel *Item;

	vgui::Button *MyInfoBtn;
	vgui::Button *OKBtn;
	vgui::ImagePanel *Effect;
	CBinkPanel *bink;
};

CCSOLLuckyItemResultDialog::CCSOLLuckyItemResultDialog(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: BaseClass(parent, panelName, showTaskbarIcon), pimpl(std::make_unique<impl_t>())
{
	pimpl->MyInfoBtn = new vgui::Button(this, "MyInfoBtn", "#CSO_GotoMyInfo", this, "vguicancel");
	pimpl->OKBtn = new vgui::Button(this, "OKBtn", "#CSO_OKl_Btn", this, "vguicancel");
	pimpl->Effect = new vgui::ImagePanel(this, "Effect");

	//pimpl->ItemName = new DarkTextEntry(this, "ItemName");
	//pimpl->ItemDescription = new DarkTextEntry(this, "ItemDescription");
	//pimpl->Item = new vgui::ImagePanel(this, "Item");

	LoadControlSettings("Resource/res/popup_luckyitemresult_renewal.res");

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

void CCSOLLuckyItemResultDialog::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{
		return Close();
	}
	BaseClass::OnCommand(command);
}

vgui::Panel *CCSOLLuckyItemResultDialog::CreateControlByName(const char *controlName)
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
	else if (!Q_stricmp(controlName, "RoundPanel") || !Q_stricmp(controlName, "IMETextEntry") || !Q_stricmp(controlName, "TextEntry"))
	{
		return new DarkTextEntry(this, "DarkTextEntry");
	}
	return BaseClass::CreateControlByName(controlName);
}

void CCSOLLuckyItemResultDialog::Activate(void)
{
	BaseClass::Activate();
	BasePanel()->PositionDialog(this);
	input()->SetAppModalSurface(GetVPanel());

	pimpl->bink->SetVisible(false);
	pimpl->Effect->SetVisible(false);
	pimpl->Effect->SetAlpha(0);
}

void CCSOLLuckyItemResultDialog::ActivateAnimation(int type)
{
	pimpl->bink->CloseBink();
	pimpl->bink->OpenBink(MakeString("resource\\gachapon\\congratulation", type, ".bik").c_str(), BINKSURFACE32RA);
	pimpl->Effect->SetImage(MakeString("resource\\gachapon\\result_congratulation", type).c_str());
	engine->pfnClientCmd((char *)MakeString("speak events/congratulation", type).c_str());
	//engine->pfnClientCmd("fmod sound/events/congratulation3.wav");

	int w, h;
	GetSize(w, h);

	pimpl->bink->Resize();
	int w2, h2;
	pimpl->bink->GetSize(w2, h2);

	pimpl->bink->SetBounds(w / 2 - w2 / 2, h / 2 - h2 / 2, w2, h2);
	pimpl->bink->SetVisible(true);
	pimpl->bink->Play();

	int TargetX, TargetY, TargetW, TargetH;

	pimpl->Effect->GetBounds(TargetX, TargetY, TargetW, TargetH);
	TargetW *= 1.115;
	TargetH *= 1.115;
	TargetX -= 11;
	TargetY -= 68;
	TargetH = TargetW;

	// void RunAnimationCommand(vgui::Panel *panel, const char *variable, float targetValue, float startDelaySeconds, float durationSeconds, Interpolators_e interpolator, float animParameter = 0 );
	GetAnimationController()->RunAnimationCommand(pimpl->bink, "xpos", TargetX, 2.5f, 1.0f, AnimationController::INTERPOLATOR_LINEAR);
	GetAnimationController()->RunAnimationCommand(pimpl->bink, "ypos", TargetY, 2.5f, 1.0f, AnimationController::INTERPOLATOR_LINEAR);
	GetAnimationController()->RunAnimationCommand(pimpl->bink, "wide", TargetW, 2.5f, 1.0f, AnimationController::INTERPOLATOR_LINEAR);
	GetAnimationController()->RunAnimationCommand(pimpl->bink, "tall", TargetH, 2.5f, 1.0f, AnimationController::INTERPOLATOR_LINEAR);
	GetAnimationController()->RunAnimationCommand(pimpl->bink, "alpha", 0, 3.5f, 0.0f, AnimationController::INTERPOLATOR_SIMPLESPLINE);
	GetAnimationController()->RunAnimationCommand(pimpl->Effect, "alpha", 255, 3.5f, 0.0f, AnimationController::INTERPOLATOR_SIMPLESPLINE);

	pimpl->bink->SetVisible(true);
	pimpl->Effect->SetVisible(true);
}

void CCSOLLuckyItemResultDialog::OnClose(void)
{
	BaseClass::OnClose();

	vgui::input()->SetAppModalSurface(NULL);
}

void CCSOLLuckyItemResultDialog::SetAssociatedItem(const char *name, const char *info, const char *image)
{
	pimpl->ItemName->SetText(name);
	pimpl->ItemDescription->SetText(info);
	pimpl->Item->SetImage(image);
}

void CCSOLLuckyItemResultDialog::SetAssociatedItem(const char *name, const char *info, vgui::IImage *image)
{
	pimpl->ItemName->SetText(name);
	pimpl->ItemDescription->SetText(info);
	pimpl->Item->SetImage(image);
}

void CCSOLLuckyItemResultDialog::SetItemName(const char *name)
{
	pimpl->ItemName->SetText(name);
}
void CCSOLLuckyItemResultDialog::SetItemName(const wchar_t *name)
{
	pimpl->ItemName->SetText(name);
}