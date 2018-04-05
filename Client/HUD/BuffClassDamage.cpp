
#include "hud.h"
#include "DrawTargaImage.h"
#include "texdraw.h"
#include "BaseUI.h"
#include "BuffClassDamage.h"

#include "VGUI/IEngineVGUI.h"
#include "EngineInterface.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/EditablePanel.h"

class HudBuffClassDamagePanel : public vgui::EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE(HudBuffClassDamagePanel, ImagePanel);
	HudBuffClassDamagePanel() : EditablePanel(nullptr, "SimpleDamageMeterPanel")
	{
		SetParent(enginevguifuncs->GetPanel(PANEL_CLIENTDLL));

		m_pLabelA = new vgui::Label(this, "myDamage", "");
		m_pLabelB = new vgui::Label(this, "myDamageTotal", "");
		LoadControlSettings("Resource\\hud\\simpledamagemeter.res");

		IScheme *pSchemeA = scheme()->GetIScheme(m_pLabelA->GetScheme());
		if (pSchemeA)
		{
			m_pLabelA->SetFont(pSchemeA->GetFont("ChatDefault"));
		}
		m_pLabelA->SetFgColor({ 255, 255, 255, 255 });
		
		IScheme *pSchemeB = scheme()->GetIScheme(m_pLabelB->GetScheme());
		if (pSchemeB)
		{
			m_pLabelB->SetFont(pSchemeB->GetFont("ChatDefault"));

		}
		m_pLabelB->SetFgColor({ 255, 73, 176, 255 });
		this->SetPos(ScreenWidth - 1024 + 854, 20);
	}
	vgui::Label *m_pLabelA;
	vgui::Label *m_pLabelB;
};

static CHudBuffClassDamage g_HudBuffClassDamage;
CHudBuffClassDamage &HudBuffClassDamage()
{
	return g_HudBuffClassDamage;
}

void CHudBuffClassDamage::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}

void CHudBuffClassDamage::VidInit(void)
{
	if (!m_pPanel)
		m_pPanel = new HudBuffClassDamagePanel;
	Reset();
}

void CHudBuffClassDamage::Reset(void)
{
	m_iLastDamage = 0;
	m_iTotalDamage = 0;
	m_pPanel->m_pLabelA->SetText("");
	m_pPanel->m_pLabelB->SetText("");
	m_pPanel->SetVisible(false);
}

void CHudBuffClassDamage::UpdateDamage(int iDamage)
{
	m_iLastDamage = iDamage;
	m_iTotalDamage += iDamage;
	char buffer[32];
	sprintf(buffer, "%d", m_iLastDamage);
	m_pPanel->m_pLabelA->SetText(buffer);
	sprintf(buffer, "%d", m_iTotalDamage);
	m_pPanel->m_pLabelB->SetText(buffer);
	m_pPanel->SetVisible(true);
}
