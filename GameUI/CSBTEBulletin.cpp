#include "engineinterface.h"
#include "vgui_controls\PHandle.h"

#include "CSBTEBulletin.h"

// Controls //
#include "vgui_controls\Button.h"
#include "vgui_controls\ImagePanel.h"
#include "vgui_controls\RichText.h"
#include "vgui_controls\PanelListPanel.h"

#include "cso_controls\DarkTextEntry.h"

struct CCSBTEBulletin::impl_t 
{
	vgui::Button *Close;

	DarkTextEntry *Title;
	PanelListPanel *m_pNoticeList;

};
CCSBTEBulletin::CCSBTEBulletin(Panel *parent, const char *panelName, bool showTaskbarIcon)
	: BaseClass(parent, panelName, showTaskbarIcon), pimpl(std::make_unique<impl_t>()) 
{
	pimpl->Title = new DarkTextEntry(this, "Title");
	pimpl->m_pNoticeList = new PanelListPanel(this, "TitleContainerClipPanelName");

	pimpl->Close = new vgui::Button(this, "Close", "#CSO_Bulletin_Close", this, "vguicancel");

	LoadControlSettings("resource\bulletin\popup_bulletin.res");

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
}

vgui::Panel *CCSBTEBulletin::CreateControlByName(const char *controlName)
{
	if (!Q_stricmp(controlName, "CTempFrame"))
	{
		return this;
	}
}