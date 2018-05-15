#include "EngineInterface.h"
#include <vgui_controls\PHandle.h>

#include "CSBTEBulletin.h"

// Controls //
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/RichText.h>
#include "vgui_controls/PanelListPanel.h"

#include "cso_controls/DarkTextEntry.h"

using namespace vgui;

struct CCSBTEBulletin::impl_t 
{
	vgui::Button *Close;
	vgui::Button *Next;

	ImagePanel *Title;
	PanelListPanel *m_pNoticeList;

};

CCSBTEBulletin::CCSBTEBulletin(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: BaseClass(parent, panelName, showTaskbarIcon), pimpl(std::make_unique<impl_t>()) 
{
	pimpl->Title = new ImagePanel(this, "Title");
	pimpl->m_pNoticeList = new PanelListPanel(this, "TitleContainerClipPanelName");

	pimpl->Next = new vgui::Button(this, "Next", "#CSO_Bulletin_Next", this, "Next");
	pimpl->Close = new vgui::Button(this, "Close", "#CSO_Bulletin_Close", this, "vguicancel");

	LoadControlSettings("resource\\bulletin\\popup_bulletin.res");

	this->SetMenuButtonVisible(false);
	this->SetCloseButtonVisible(false);
	this->SetSizeable(false);
	this->SetTitle("", false);
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