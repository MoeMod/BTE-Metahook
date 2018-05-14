#pragma once

#include "cso_controls/BluedPopupDialog.h"

using namespace vgui;

class CCSBTEBulletin : public BluedPopupDialog
{
	DECLARE_CLASS_SIMPLE(CCSBTEBulletin, BluedPopupDialog);

public:
	CCSBTEBulletin(Panel *parent, const char *panelName = "BulletinDlg", bool showTaskbarIcon = true);
	//CCSBTEBulletin(Panel *parent, const char *panelName = "BulletinDlg", bool showTaskbarIcon = true);

protected:
	virtual void OnCommand(const char *command) override;
	virtual vgui::Panel *CreateControlByName(const char *controlName) override;

private:
	struct impl_t;
	std::unique_ptr<impl_t> pimpl;
};