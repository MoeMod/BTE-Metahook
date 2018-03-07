#pragma once

#include <vgui_controls/PHandle.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Progressbar.h>

#include <set>

using namespace vgui;

class CCSBTEMapLoading : public ImagePanel
{
	DECLARE_CLASS_SIMPLE(CCSBTEMapLoading, ImagePanel);

public:
	CCSBTEMapLoading(Panel *parent, const char *name);
	~CCSBTEMapLoading();
	
	bool SetMapName(const char *name);
	bool UpdateGameMode(int iGameMode);
	void ClearOrigins();
	void AddOrigin(int iType, int x, int y);
	bool UpdateOrigins(const char *name);
	void SetServerName(const char *szStatusText);
	void SetStatusText(const char *szStatusText);
	bool SetProgressPoint(int progressPoint);
	void SetProgressRange(int min, int max);
	void SetProgressVisible(bool bSet);

	virtual void PaintBackground(); // 重画高清♂无码的载入tga

private:

	std::set<ImagePanel *>m_setPanels;
	char *m_szOriginIcon[18];
	int m_iGameMode;

	int m_iBgTexIndex;

	char *m_szMapName;
	ImagePanel *m_pTaskBar;
	ImagePanel *m_pToolTip;
	Label *m_pServerName;
	Label *m_pStatusText;
	Label *m_pTipText;
	ProgressBar *m_pProgress;
	int m_iRangeMin, m_iRangeMax;
};