#include "CSBTEUpdateDialog.h"

CCSBTEUpdateDialog::CCSBTEUpdateDialog(Panel *parent, const char *panelName, bool showTaskbarIcon) : Frame(parent, panelName, showTaskbarIcon)
{
	int sw, sh;
	surface()->GetScreenSize(sw, sh);

	SetTitle("#CSBTE_UpdateNews_Title", false);

	SetSize(740, 700);
	MoveToCenterOfScreen();
	SetSizeable(false);
	SetVisible(true);

	m_pCancelButton = new Button(this, "CancelButton", "#GameUI_Close");
	m_pCancelButton->SetContentAlignment(Label::a_center);
	m_pCancelButton->SetBounds(260, 625, 200, 50);
	m_pCancelButton->SetCommand("vguicancel");
	m_pCancelButton->SetVisible(true);

	m_pUpdateNews = new RichText(this, "UpdateNews");
	m_pUpdateNews->SetPaintBackgroundEnabled(false);
	m_pUpdateNews->SetBounds(25, 30, 700, 600);
	m_pUpdateNews->SetText("#CSBTE_UpdateNews_Data");
}

void CCSBTEUpdateDialog::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{
		Close();
	}
}