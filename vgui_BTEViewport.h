#pragma once

/*
*	Counter-Strike BreakThroughEdition VGUI Viewport
*	Base Panels for game
*/

#include "VGUI/VGUI.h"
#include <vgui_controls\Frame.h>

namespace vgui
{
	class CSOPanel : public Frame
	{
		DECLARE_CLASS_SIMPLE(CSOPanel, Frame);

	public:
		CSOPanel(Panel *parent, const char *panelName, bool showTaskbarIcon = true, bool darkWindow = true);

	public:
		virtual void PaintBackground();
		virtual void PaintBorder() { ; }

	private:
		static int m_iTexCSOPanel[3][3];
	};

}