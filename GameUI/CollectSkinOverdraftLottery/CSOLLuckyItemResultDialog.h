#pragma once

#include "cso_controls/BluedPopupDialog.h"
#include <memory>

using namespace vgui;

class CCSOLLuckyItemResultDialog : public BluedPopupDialog
{
	DECLARE_CLASS_SIMPLE(CCSOLLuckyItemResultDialog, BluedPopupDialog);

public:
	CCSOLLuckyItemResultDialog(Panel *parent, const char *panelName = "LuckyItemResultDlg", bool showTaskbarIcon = true);

public:
	virtual void OnCommand(const char *command) override;
	virtual vgui::Panel *CreateControlByName(const char *controlName) override;
	virtual void Activate(void) override;
	virtual void OnClose(void) override;

private:
	struct impl_t;
	std::unique_ptr<impl_t> pimpl;

};