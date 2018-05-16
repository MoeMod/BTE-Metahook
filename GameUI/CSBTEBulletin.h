#pragma once

#include "vgui_controls/Frame.h"
#include <memory>

class CCSBTEBulletin : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTEBulletin, vgui::Frame);

public:
	CCSBTEBulletin(Panel *parent, const char *panelName = "BulletinDlg", bool showTaskbarIcon = true);
	//CCSBTEBulletin(Panel *parent, const char *panelName = "BulletinDlg", bool showTaskbarIcon = true);

protected:
	virtual void OnCommand(const char *command) override;
	virtual vgui::Panel *CreateControlByName(const char *controlName) override;
	int AddNotice(const char *name);

private:
	struct impl_t;
	std::unique_ptr<impl_t> pimpl;
};