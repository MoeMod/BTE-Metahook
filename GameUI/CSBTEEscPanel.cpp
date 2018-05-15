#include "BaseUI.h"
#include "CSBTEEscPanel.h"
#include "BasePanel.h"

#include "cso_controls/TexturedButton.h"

CCSBTEEscPanel::CCSBTEEscPanel(Panel *parent, const char *panelName, bool showTaskbarIcon) : Frame(parent, panelName, showTaskbarIcon)
{
	SetCloseButtonVisible(false);
	SetMenuButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetTitleBarVisible(false);
	SetSize(118, 418);

	BasePanel()->PositionDialog(this);
	
	int x = 32;
	int y = 20;
	int w = 62;
	int h = 64;

	m_pShooting = new CCSBTEEscPanelButton(this, "ShootingButton", "");
	m_pShooting->SetImage("resource/taskbar/btn_shooting@c", "resource/taskbar/btn_shooting@n", "resource/taskbar/btn_shooting@o");
	m_pShooting->SetBounds(x, y + 64 * 0, w, h);
	m_pShooting->SetCommand("");
	m_pShooting->AddActionSignalTarget(this);

	m_pRecording = new CCSBTEEscPanelButton(this, "RecordingButton", "");
	m_pRecording->SetImage("resource/taskbar/btn_recording@c", "resource/taskbar/btn_recording@n", "resource/taskbar/btn_recording@o");
	m_pRecording->SetBounds(x, y + 64 * 1, w, h);
	m_pRecording->SetCommand("");
	m_pRecording->AddActionSignalTarget(this);

	m_pReport = new CCSBTEEscPanelButton(this, "ReportButton", "");
	m_pReport->SetImage("resource/taskbar/btn_report@c", "resource/taskbar/btn_report@n", "resource/taskbar/btn_report@o");
	m_pReport->SetBounds(x, y + 64 * 2, w, h);
	m_pReport->SetCommand("");
	m_pReport->AddActionSignalTarget(this);

	m_pOption = new CCSBTEEscPanelButton(this, "OptionButton", "");
	m_pOption->SetImage("resource/taskbar/btn_option@c", "resource/taskbar/btn_option@n", "resource/taskbar/btn_option@o");
	m_pOption->SetBounds(x, y + 64 * 3, w, h);
	m_pOption->SetCommand("OpenOptionsDialog");
	m_pOption->AddActionSignalTarget(this);

	m_pExit = new CCSBTEEscPanelButton(this, "ExitButton", "");
	m_pExit->SetImage("resource/taskbar/btn_exit@c", "resource/taskbar/btn_exit@n", "resource/taskbar/btn_exit@o");
	m_pExit->SetBounds(x, y + 64 * 4, w, h);
	m_pExit->SetCommand("Exit");
	m_pExit->AddActionSignalTarget(this);

	m_pCancel = new CCSBTEEscPanelButton(this, "CancelButton", "");
	m_pCancel->SetImage("resource/taskbar/btn_cancel@c", "resource/taskbar/btn_cancel@n", "resource/taskbar/btn_cancel@o");
	m_pCancel->SetBounds(x, y + 64 * 5, w, h);
	m_pCancel->SetCommand("Cancel");
	m_pCancel->AddActionSignalTarget(this);

}

CCSBTEEscPanel::~CCSBTEEscPanel()
{
	delete m_pShooting;
	delete m_pRecording;
	delete m_pReport;
	delete m_pOption;
	delete m_pExit;
	delete m_pCancel;
}

void CCSBTEEscPanel::PaintBackground()
{
	//gEngfuncs.pfnFillRGBA(0, 0, ScreenWidth, ScreenHeight, 75, 75, 75, 75);
	BaseClass::PaintBackground();
}

void CCSBTEEscPanel::OnCommand(const char *command)
{
	if (!stricmp(command, "Cancel"))
	{
		vgui::surface()->RestrictPaintToSinglePanel(NULL);
		baseuifuncs->HideGameUI();
	}
	else if (!stricmp(command, "Exit"))
	{
		//gEngfuncs.pfnClientCmd("disconnect");
		MessageBox *msgbox;
		msgbox = new MessageBox("#CSO_GameLeaveConfirmation_Title", "#CSO_GameLeaveConfimation_Text", this);
		msgbox->SetOKButtonText("#CSO_GameLeave_Btn");
		msgbox->SetOKButtonVisible(true);
		msgbox->SetCancelButtonText("#CSO_Cancel");
		msgbox->SetCancelButtonVisible(true);
		msgbox->SetCommand("Disconnect");
		msgbox->AddActionSignalTarget(this);
		msgbox->DoModal(this);
		msgbox->Activate();

	}
	else if (!stricmp(command, "OpenOptionsDialog"))
	{
		//if (staticPanel)
		//	CBasePanel_RunMenuCommand(staticPanel, 0, "OpenOptionsDialog");
		BasePanel()->RunMenuCommand("OpenOptionsDialog");
	}
	else if (!stricmp(command, "Disconnect"))
	{
		gEngfuncs.pfnClientCmd("disconnect");

		MessageBox *msgbox;
		msgbox = new MessageBox("#CSO_ServerMessage_Title", "#CSO_Warning_ROOM_LEAVE_PENALTY", this->GetParent());
		msgbox->SetOKButtonText("#CSO_OKl_Btn");
		msgbox->SetOKButtonVisible(true);
		msgbox->Activate();
	}
}