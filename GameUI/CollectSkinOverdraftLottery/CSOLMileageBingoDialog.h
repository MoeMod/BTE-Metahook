#pragma once

#include "cso_controls/BluedPopupDialog.h"
#include <memory>

class vgui::IImage;

class CCSOLMileageBingoDialog : public BluedPopupDialog
{
	DECLARE_CLASS_SIMPLE(CCSOLMileageBingoDialog, BluedPopupDialog);

public:
	CCSOLMileageBingoDialog(Panel *parent, const char *panelName = "MileageBingoDialog", bool showTaskbarIcon = true);

public:
	virtual void OnCommand(const char *command) override;
	virtual vgui::Panel *CreateControlByName(const char *controlName) override;
	virtual void Activate(void) override;
	virtual void OnClose(void) override;

private:
	struct impl_t;
	std::unique_ptr<impl_t> pimpl;

};