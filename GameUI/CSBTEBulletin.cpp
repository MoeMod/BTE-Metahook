#include "EngineInterface.h"
#include <vgui_controls\PHandle.h>

#include "CSBTEBulletin.h"

// Controls //
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/RichText.h>
#include "vgui_controls/PanelListPanel.h"
#include "cso_controls/SelectedImage.h"

#include "cso_controls/DarkTextEntry.h"

#include "iniparser.h"
#include "LinearMap.h"

using namespace vgui;

struct CCSBTEBulletin::impl_t 
{
	vgui::Button *Close;
	vgui::Button *Next;

	ImagePanel *Title;
	Label *TitleLabel, *Bulletin, *EventProgress;
	PanelListPanel *m_pNoticeList;
	TextEntry *NoticeText;

	CBasicIniParser< LinearMap<std::string, std::unordered_map<std::string, std::string>> > m_iniData;
};

class NoticePanel : public Button
{
	DECLARE_CLASS_SIMPLE(NoticePanel, Button);
public:
	NoticePanel(Panel *parent, const char *name, const char *noticeName)
		:Button(parent, name, "")
	{
		m_pText = new Label(this, "Label", MakeString("#CSBTE_Notice_", noticeName).c_str());
		m_pText->SetPaintBackgroundEnabled(false);
		m_pText->SetBounds(5, 80, 200, 20);
		m_pText->SetMouseInputEnabled(false);

		m_pNoticeImage = new ImagePanel(this, "CSBTENoticeIcon");
		m_pNoticeImage->SetImage(scheme()->GetImage("", false));
		m_pNoticeImage->SetBounds(0, 0, 225, 81);
		m_pNoticeImage->SetShouldScaleImage(true);
		m_pNoticeImage->SetMouseInputEnabled(false);

		m_pSelectedBackground = new SelectedImage(this, "CSBTESelectedBackground");
		m_pSelectedBackground->SetBounds(0, 0, 225, 81);
		m_pSelectedBackground->SetMouseInputEnabled(false);

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
	ImagePanel *m_pNoticeImage;
	SelectedImage *m_pSelectedBackground;

};

CCSBTEBulletin::CCSBTEBulletin(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: BaseClass(parent, panelName, showTaskbarIcon), pimpl(std::make_unique<impl_t>()) 
{
	pimpl->m_iniData.OpenFile("Bulletin.ini");

	pimpl->Title = new ImagePanel(this, "Title");
	pimpl->m_pNoticeList = new PanelListPanel(this, "TitleContainerClipPanelName");

	//pimpl->TitleLabel = new Label(this, "TitleLabel", "#CSO_Bulletin_Title");
	//pimpl->Bulletin = new Label(this, "Bulletin", "#CSO_Bulletin_Notice");
	//pimpl->EventProgress = new Label(this, "EventProgress", "#CSO_Bulletin_Event");
	
	pimpl->NoticeText = new DarkTextEntry(this, "NoticeText");

	pimpl->Next = new vgui::Button(this, "Next", "#CSO_Bulletin_Next", this, "Next");
	pimpl->Close = new vgui::Button(this, "Close", "#CSO_Bulletin_Close", this, "vguicancel");

	LoadControlSettings("resource\\bulletin\\popup_bulletin.res");

	this->SetMenuButtonVisible(false);
	this->SetCloseButtonVisible(false);
	this->SetSizeable(false);
	this->SetTitle("", false);

	pimpl->m_pNoticeList->SetWide(pimpl->m_pNoticeList->GetWide() + 15);
	pimpl->m_pNoticeList->SetFirstColumnWidth(0);
}

int CCSBTEBulletin::AddNotice(const char *name)
{
	int a = 0;
	auto pPanel = new NoticePanel(pimpl->m_pNoticeList, name, name);
	return a;
}

void CCSBTEBulletin::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{
		return Close();
	}
	else if (!Q_stricmp(command, "Next"))
	{
		// Cause No Next Effect, So Use Close Instead //
		return Close();
	}
}

vgui::Panel *CCSBTEBulletin::CreateControlByName(const char *controlName)
{
	if (!Q_stricmp(controlName, "CTempFrame"))
	{
		return this;
	}
	else if (!Q_stricmp(controlName, "IMETextEntry"))
	{
		return new DarkTextEntry(this, controlName);
	}
	return BaseClass::CreateControlByName(controlName);
}