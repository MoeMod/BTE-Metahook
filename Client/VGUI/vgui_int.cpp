

#include <vgui/vgui.h>
#include <vgui/ISurface.h>
#include <vgui/ilocalize.h>
#include <KeyValues.h>

#include <vgui_controls/Controls.h>

#include <Interface\vgui\IEngineVGui.h>
#include "BaseViewPort.h"

void GetHudSize(int &w, int &h)
{
	vgui::VPANEL hudParent = enginevgui->GetPanel(vgui::PANEL_CLIENTDLL);

	if (hudParent)
		vgui::ipanel()->GetSize(hudParent, w, h);
	else
		vgui::surface()->GetScreenSize(w, h);
}