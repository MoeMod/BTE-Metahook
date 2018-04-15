#pragma once

#include "cso_controls/BluedPopupDialog.h"
#include <memory>

using namespace vgui;

class CCSOLLuckyItemPopupDialog : public BluedPopupDialog
{
	DECLARE_CLASS_SIMPLE(CCSOLLuckyItemPopupDialog, BluedPopupDialog);

public:
	CCSOLLuckyItemPopupDialog(Panel *parent, const char *panelName = "LuckyItemDlg", bool showTaskbarIcon = true);

protected:
	virtual void OnCommand(const char *command) override;
	virtual vgui::Panel *CreateControlByName(const char *controlName) override;

	void OnOpenDecoder();
	void AddDecoder(const char *name);
private:
	struct impl_t;
	std::unique_ptr<impl_t> pimpl;
};