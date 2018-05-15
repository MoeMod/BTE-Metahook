
#include "hud.h"
#include "TimerTipCountDown.h"

#include "VGUI/IEngineVGUI.h"
#include "EngineInterface.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/TextImage.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/EditablePanel.h"

#include "parsemsg.h"
#include <string>

using namespace vgui;

class HudTimerTipCountDownPanel : public vgui::ImagePanel
{
public:
	DECLARE_CLASS_SIMPLE(HudTimerTipCountDownPanel, ImagePanel);
	HudTimerTipCountDownPanel() : ImagePanel(nullptr, "HudTimerTipCountDownPanel")
	{
		SetParent(enginevguifuncs->GetPanel(PANEL_CLIENTDLL));

		m_pLabelA = new vgui::Label(this, "Tip", "");
		m_pLabelB = new vgui::Label(this, "Timer", "");

		m_pLabelA->SetAssociatedControl(NULL);
		m_pLabelB->SetAssociatedControl(NULL);

		m_pLabelA->SetBounds(0, 20, 128, 18);
		m_pLabelB->SetBounds(0, 60, 128, 44);

		m_pLabelA->SetContentAlignment(Label::a_north);
		m_pLabelB->SetContentAlignment(Label::a_north);

		this->SetImage("resource\\fun\\zbescape\\countdownbg");
		this->SetBounds(10, ScreenHeight / 3 - 16, 128, 128);
	}
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme) override
	{
		BaseClass::ApplySchemeSettings(pScheme);
		m_pLabelA->SetFont(pScheme->GetFont("InGame20"));
		m_pLabelB->SetFont(pScheme->GetFont("CreditsTitle"));
		m_pLabelA->SetFgColor({ 255, 200, 50, 255 });
		m_pLabelB->SetFgColor({ 255, 200, 50, 255 });
	}
	vgui::Label *m_pLabelA;
	vgui::Label *m_pLabelB;
};

static CHudTimerTipCountDown g_HudTimerTipCountDown;
CHudTimerTipCountDown &HudTimerTipCountDown()
{
	return g_HudTimerTipCountDown;
}

void CHudTimerTipCountDown::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
	gEngfuncs.pfnHookUserMsg("TimerTip", [](const char *pszName, int iSize, void *pbuf) { return HudTimerTipCountDown().MsgFunc_TimerTip(pszName, iSize, pbuf); });
}

int CHudTimerTipCountDown::MsgFunc_TimerTip(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	const char *name = READ_STRING();
	int time = READ_LONG();
	Activate(name, time);
	return 0;
}

void CHudTimerTipCountDown::Activate(const char *name, int time)
{
	m_pPanel->SetVisible(true);
	m_pPanel->m_pLabelA->SetText(name);
	m_flTimeEnd = cl.time + time;
	m_bEnabled = true;
}

void CHudTimerTipCountDown::VidInit(void)
{
	if (!m_pPanel)
		m_pPanel = new HudTimerTipCountDownPanel;
	Reset();
}

void CHudTimerTipCountDown::Draw(float flTime)
{
	if (!m_bEnabled)
		return;
	if (flTime >= m_flTimeEnd)
	{
		Reset();
		return;
	}
	
	static int iLastCount = -1;
	int iCount = int(m_flTimeEnd - flTime) + 1;
	if (iCount != iLastCount)
	{
		m_pPanel->m_pLabelB->SetText(std::to_string(iCount).c_str());
		m_pPanel->SetVisible(true);
		iLastCount = iCount;
	}
}

void CHudTimerTipCountDown::Reset(void)
{
	m_pPanel->SetVisible(false);
	m_bEnabled = false;
}
