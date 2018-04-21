#pragma once

#include <vgui_controls/Frame.h>
#include <memory>

using namespace vgui;

class BluedPopupDialog : public Frame
{
	DECLARE_CLASS_SIMPLE(BluedPopupDialog, Frame);

public:
	BluedPopupDialog(Panel *parent, const char *panelName, bool showTaskbarIcon = true)
		:BaseClass(parent, panelName, showTaskbarIcon){}

protected:
	virtual void ApplySchemeSettings(IScheme *pScheme) override
	{
		BaseClass::ApplySchemeSettings(pScheme);

		m_pTopBackground[0] = vgui::scheme()->GetImage("resource/control/igframe/window_default_top_left_ver2", true);
		m_pTopBackground[1] = vgui::scheme()->GetImage("resource/control/igframe/window_default_top_center_ver2", true);
		m_pTopBackground[2] = vgui::scheme()->GetImage("resource/control/igframe/window_default_top_right_ver2", true);
		m_pCenterBackground[0] = vgui::scheme()->GetImage("resource/control/igframe/window_default_center_left_ver2", true);
		m_pCenterBackground[1] = vgui::scheme()->GetImage("resource/control/igframe/window_default_center_center_ver2", true);
		m_pCenterBackground[2] = vgui::scheme()->GetImage("resource/control/igframe/window_default_center_right_ver2", true);
		m_pBottomBackground[0] = vgui::scheme()->GetImage("resource/control/igframe/window_default_bottom_left_ver2", true);
		m_pBottomBackground[1] = vgui::scheme()->GetImage("resource/control/igframe/window_default_bottom_center_ver2", true);
		m_pBottomBackground[2] = vgui::scheme()->GetImage("resource/control/igframe/window_default_bottom_right_ver2", true);
	}
};