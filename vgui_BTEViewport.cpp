#include <VGUI\VGUI.h>
#include <vgui_BTEViewport.h>
#include <vgui_controls\TextImage.h>

void GL_LoadFullPanel(int tid[3][3], char *szName);
int GL_DrawFullPanel(int tid[3][3], int iX, int iY, int iW, int iH, int alpha);

using namespace vgui;

int CSOPanel::m_iTexCSOPanel[3][3];

CSOPanel::CSOPanel(Panel *parent, const char *panelName, bool showTaskbarIcon, bool darkWindow) : Frame(parent, panelName, showTaskbarIcon)
{
	if (!m_iTexCSOPanel[0][0])
	{
		GL_LoadFullPanel(m_iTexCSOPanel, darkWindow ? "window_dark" : "window");
	}
}

void CSOPanel::PaintBackground(void)
{
	int w, h;

	GetSize(w, h);

	surface()->DrawSetColor(10, 10, 10, 150);
	surface()->DrawFilledRect(5, 12, w - 10, h - 15);

	GL_DrawFullPanel(m_iTexCSOPanel, 0, 0, w, h, 255);

	//SubPaintTitle();
}